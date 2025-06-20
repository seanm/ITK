/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkImageToParametricSpaceFilter_hxx
#define itkImageToParametricSpaceFilter_hxx

#include "itkNumericTraits.h"
#include "itkImageIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkProgressReporter.h"

namespace itk
{
/**
 *
 */
template <typename TInputImage, typename TOutputMesh>
ImageToParametricSpaceFilter<TInputImage, TOutputMesh>::ImageToParametricSpaceFilter()
  : m_ComputeIndices(true)
{
  // Modify superclass default values, can be overridden by subclasses
  this->SetNumberOfRequiredInputs(PointDimension);
}

/**
 *
 */
template <typename TInputImage, typename TOutputMesh>
void
ImageToParametricSpaceFilter<TInputImage, TOutputMesh>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "ComputeIndices: " << m_ComputeIndices << std::endl;
}

/**
 *
 */
template <typename TInputImage, typename TOutputMesh>
void
ImageToParametricSpaceFilter<TInputImage, TOutputMesh>::GenerateOutputInformation()
{
  const OutputMeshPointer      mesh = this->GetOutput();
  const PointsContainerPointer points = mesh->GetPoints();

  PointDataContainerPointer pointData;
  if (mesh->GetPointData())
  {
    pointData = mesh->GetPointData();
  }
  else
  { // Create
    pointData = PointDataContainer::New();
  }

  const InputImageConstPointer image = this->GetInput(0);
  const SizeValueType          numberOfPixels = image->GetRequestedRegion().GetNumberOfPixels();

  points->Reserve(numberOfPixels);
  pointData->Reserve(numberOfPixels);
  mesh->SetPointData(pointData);
}

/**
 *
 */
template <typename TInputImage, typename TOutputMesh>
void
ImageToParametricSpaceFilter<TInputImage, TOutputMesh>::GenerateData()
{
  const OutputMeshPointer         mesh = this->GetOutput();
  const PointsContainerPointer    points = mesh->GetPoints();
  const PointDataContainerPointer pointData = PointDataContainer::New();
  InputImageConstPointer          image = this->GetInput(0);
  const InputImageRegionType      region = image->GetRequestedRegion();

  const SizeValueType numberOfPixels = region.GetNumberOfPixels();

  points->Reserve(numberOfPixels);
  pointData->Reserve(numberOfPixels);

  mesh->SetPointData(pointData);
  mesh->SetBufferedRegion(mesh->GetRequestedRegion());

  // support progress methods/callbacks
  ProgressReporter progress(this, 0, numberOfPixels);

  for (unsigned int component = 0; component < PointDimension; ++component)
  {
    image = this->GetInput(component);
    ImageRegionConstIterator<InputImageType> itr(image, image->GetRequestedRegion());

    PointsContainerIterator point = points->Begin();

    while (!itr.IsAtEnd())
    {
      (point.Value())[component] = itr.Get();
      ++itr;
      ++point;
      progress.CompletedPixel();
    }
  }

  if (m_ComputeIndices)
  {
    PointDataContainerIterator data = pointData->Begin();
    image = this->GetInput(0);
    ImageRegionConstIteratorWithIndex<InputImageType> itr(image, image->GetRequestedRegion());
    while (!itr.IsAtEnd())
    {
      //  The data at each point is the index
      //  of the corresponding pixel on the image.
      using MeshPixelType = typename OutputMeshType::PixelType;
      MeshPixelType point;
      image->TransformIndexToPhysicalPoint(itr.GetIndex(), point);
      (data.Value()) = point;
      ++itr;
      ++data;
    }
  }
}
} // end namespace itk

#endif
