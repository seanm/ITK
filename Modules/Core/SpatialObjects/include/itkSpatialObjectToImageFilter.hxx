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
#ifndef itkSpatialObjectToImageFilter_hxx
#define itkSpatialObjectToImageFilter_hxx

#include "itkImageRegionIteratorWithIndex.h"
#include "itkProgressReporter.h"
#include "itkMath.h"

namespace itk
{

template <typename TInputSpatialObject, typename TOutputImage>
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SpatialObjectToImageFilter()
  : m_ChildrenDepth(TInputSpatialObject::MaximumDepth)
{
  this->SetNumberOfRequiredInputs(1);
  m_Size.Fill(0);
  m_Index.Fill(0);
  m_Direction.SetIdentity();

  for (unsigned int i = 0; i < OutputImageDimension; ++i)
  {
    m_Spacing[i] = 1.0;
    m_SpacingVector[i] = 1.0;
    m_Origin[i] = 0.;
    m_OriginPoint[i] = 0.;
  }

  m_InsideValue = 0;
  m_OutsideValue = 0;
  m_UseObjectValue = false;
}

template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SetInput(const InputSpatialObjectType * input)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0, const_cast<InputSpatialObjectType *>(input));
}

template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SetInput(unsigned int                index,
                                                                        const TInputSpatialObject * object)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(index, const_cast<TInputSpatialObject *>(object));
}

template <typename TInputSpatialObject, typename TOutputImage>
auto
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::GetInput() -> const InputSpatialObjectType *
{
  return static_cast<const TInputSpatialObject *>(this->GetPrimaryInput());
}

template <typename TInputSpatialObject, typename TOutputImage>
auto
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::GetInput(unsigned int idx)
  -> const InputSpatialObjectType *
{
  return static_cast<const TInputSpatialObject *>(this->ProcessObject::GetInput(idx));
}


template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SetSpacing(const SpacingType & spacing)
{
  if (ContainerCopyWithCheck(m_Spacing, spacing, TOutputImage::ImageDimension))
  {
    this->Modified();
    for (unsigned int i = 0; i < TOutputImage::ImageDimension; ++i)
    {
      if (spacing[i] <= 0)
      {
        itkExceptionMacro("Zero-valued spacing and negative spacings are not supported and may result in undefined "
                          "behavior.\nRefusing to change spacing from "
                          << this->m_Spacing << " to " << spacing);
      }
    }
  }
}


template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SetSpacing(const double * spacing)
{
  if (ContainerCopyWithCheck(m_Spacing, spacing, TOutputImage::ImageDimension))
  {
    this->Modified();
    for (unsigned int i = 0; i < TOutputImage::ImageDimension; ++i)
    {
      if (spacing[i] <= 0)
      {
        itkExceptionMacro("Zero-valued spacing and negative spacings are not supported and may result in undefined "
                          "behavior.\nRefusing to change spacing from "
                          << this->m_Spacing << " to " << spacing);
      }
    }
  }
}

template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SetSpacing(const float * spacing)
{
  if (ContainerCopyWithCheck(m_Spacing, spacing, TOutputImage::ImageDimension))
  {
    this->Modified();
    for (unsigned int i = 0; i < TOutputImage::ImageDimension; ++i)
    {
      if (spacing[i] <= 0)
      {
        itkExceptionMacro("Zero-valued spacing and negative spacings are not supported and may result in undefined "
                          "behavior.\nRefusing to change spacing from "
                          << this->m_Spacing << " to " << spacing);
      }
    }
  }
}

template <typename TInputSpatialObject, typename TOutputImage>
const double *
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::GetSpacing() const
{
  return m_Spacing;
}

template <typename TInputSpatialObject, typename TOutputImage>
const typename TOutputImage::SpacingType &
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::GetSpacingVector() const
{
  for (unsigned int i = 0; i < OutputImageDimension; ++i)
  {
    m_SpacingVector[i] = m_Spacing[i];
  }
  return m_SpacingVector;
}


template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SetOrigin(const PointType & origin)
{
  if (ContainerCopyWithCheck(m_Origin, origin, OutputImageDimension))
  {
    this->Modified();
  }
}


template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SetOrigin(const double * origin)
{
  if (ContainerCopyWithCheck(m_Origin, origin, OutputImageDimension))
  {
    this->Modified();
  }
}

template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SetOrigin(const float * origin)
{
  if (ContainerCopyWithCheck(m_Origin, origin, OutputImageDimension))
  {
    this->Modified();
  }
}

template <typename TInputSpatialObject, typename TOutputImage>
const double *
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::GetOrigin() const
{
  return m_Origin;
}

template <typename TInputSpatialObject, typename TOutputImage>
const typename TOutputImage::PointType &
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::GetOriginPoint() const
{
  for (unsigned int i = 0; i < OutputImageDimension; ++i)
  {
    m_OriginPoint[i] = m_Origin[i];
  }
  return m_OriginPoint;
}


template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::SetDirection(const DirectionType & dir)
{
  m_Direction = dir;
  this->Modified();
}

template <typename TInputSpatialObject, typename TOutputImage>
auto
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::GetDirection() const -> const DirectionType &
{
  return m_Direction;
}


template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::GenerateData()
{
  itkDebugMacro("SpatialObjectToImageFilter::Update() called");

  // Get the input and output pointers
  const InputSpatialObjectType * InputObject = this->GetInput();
  const OutputImagePointer       OutputImage = this->GetOutput();

  // Generate the image
  SizeType size;

  InputObject->ComputeFamilyBoundingBox(m_ChildrenDepth);
  for (unsigned int i = 0; i < ObjectDimension; ++i)
  {
    size[i] = static_cast<SizeValueType>(InputObject->GetFamilyBoundingBoxInWorldSpace()->GetMaximum()[i] -
                                         InputObject->GetFamilyBoundingBoxInWorldSpace()->GetMinimum()[i]);
  }

  typename OutputImageType::RegionType region;

  // If the size of the output has been explicitly specified, the filter
  // will set the output size to the explicit size, otherwise the size from the
  // spatial
  // object's bounding box will be used as default.

  bool specified = false;
  for (unsigned int i = 0; i < OutputImageDimension; ++i)
  {
    if (m_Size[i] != 0)
    {
      specified = true;
      break;
    }
  }

  if (specified)
  {
    region.SetSize(m_Size);
  }
  else
  {
    region.SetSize(size);
  }

  region.SetIndex(m_Index);

  OutputImage->SetRegions(region);    // set the region
  OutputImage->SetSpacing(m_Spacing); // set spacing
  OutputImage->SetOrigin(m_Origin);   //   and origin
  OutputImage->SetDirection(m_Direction);
  OutputImage->Allocate(); // allocate the image

  using myIteratorType = itk::ImageRegionIteratorWithIndex<OutputImageType>;

  myIteratorType it(OutputImage, region);

  itk::Point<double, ObjectDimension>      objectPoint;
  itk::Point<double, OutputImageDimension> imagePoint;

  ProgressReporter progress(this, 0, OutputImage->GetRequestedRegion().GetNumberOfPixels());

  while (!it.IsAtEnd())
  {
    // ValueAtInWorldSpace requires the point to be in physical coordinate i.e
    OutputImage->TransformIndexToPhysicalPoint(it.GetIndex(), imagePoint);
    for (unsigned int i = 0; i < ObjectDimension; ++i)
    {
      objectPoint[i] = imagePoint[i];
    }

    double val = 0;

    const bool evaluable = InputObject->ValueAtInWorldSpace(objectPoint, val, m_ChildrenDepth);
    if (Math::NotExactlyEquals(m_InsideValue, ValueType{}) || Math::NotExactlyEquals(m_OutsideValue, ValueType{}))
    {
      if (evaluable)
      {
        if (m_UseObjectValue)
        {
          it.Set(static_cast<ValueType>(val));
        }
        else
        {
          it.Set(m_InsideValue);
        }
      }
      else
      {
        it.Set(m_OutsideValue);
      }
    }
    else
    {
      it.Set(static_cast<ValueType>(val));
    }
    ++it;
    progress.CompletedPixel();
  }

  itkDebugMacro("SpatialObjectToImageFilter::Update() finished");
} // end update function

template <typename TInputSpatialObject, typename TOutputImage>
void
SpatialObjectToImageFilter<TInputSpatialObject, TOutputImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Index: " << m_Index << std::endl;
  os << indent << "Size: " << m_Size << std::endl;

  os << indent << "Spacing: " << m_Spacing[0];
  for (unsigned int i = 1; i < OutputImageDimension; ++i)
  {
    os << ", " << m_Spacing[i];
  }
  os << std::endl;

  os << indent << "SpacingVector: " << m_SpacingVector << std::endl;

  os << indent << "Origin: " << m_Origin[0];
  for (unsigned int i = 1; i < OutputImageDimension; ++i)
  {
    os << ", " << m_Origin[i];
  }
  os << std::endl;

  os << indent << "OriginPoint: " << m_OriginPoint << std::endl;

  os << indent << "Direction: " << m_Direction << std::endl;
  os << indent << "ChildrenDepth: " << m_ChildrenDepth << std::endl;
  os << indent << "InsideValue: " << m_InsideValue << std::endl;
  os << indent << "OutsideValue: " << m_OutsideValue << std::endl;
  itkPrintSelfBooleanMacro(UseObjectValue);
}
} // end namespace itk

#endif
