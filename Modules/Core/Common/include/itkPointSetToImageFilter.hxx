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
#ifndef itkPointSetToImageFilter_hxx
#define itkPointSetToImageFilter_hxx


#include "itkBoundingBox.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNumericTraits.h"
#include "itkMath.h"

namespace itk
{

template <typename TInputPointSet, typename TOutputImage>
PointSetToImageFilter<TInputPointSet, TOutputImage>::PointSetToImageFilter()
  : m_InsideValue(NumericTraits<ValueType>::OneValue())
  , m_OutsideValue(ValueType{})
{
  this->SetNumberOfRequiredInputs(1);
  this->m_Size.Fill(0);
  this->m_Origin.Fill(0.0);
  this->m_Spacing.Fill(1.0);
  this->m_Direction.SetIdentity();
}

template <typename TInputPointSet, typename TOutputImage>
void
PointSetToImageFilter<TInputPointSet, TOutputImage>::SetInput(const InputPointSetType * input)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(0, const_cast<InputPointSetType *>(input));
}

/** Connect one of the operands  */
template <typename TInputPointSet, typename TOutputImage>
void
PointSetToImageFilter<TInputPointSet, TOutputImage>::SetInput(unsigned int index, const TInputPointSet * pointset)
{
  // Process object is not const-correct so the const_cast is required here
  this->ProcessObject::SetNthInput(index, const_cast<TInputPointSet *>(pointset));
}

/** Get the input point-set */
template <typename TInputPointSet, typename TOutputImage>
auto
PointSetToImageFilter<TInputPointSet, TOutputImage>::GetInput() -> const InputPointSetType *
{
  return itkDynamicCastInDebugMode<const TInputPointSet *>(this->GetPrimaryInput());
}

/** Get the input point-set */
template <typename TInputPointSet, typename TOutputImage>
auto
PointSetToImageFilter<TInputPointSet, TOutputImage>::GetInput(unsigned int idx) -> const InputPointSetType *
{
  return itkDynamicCastInDebugMode<const TInputPointSet *>(this->ProcessObject::GetInput(idx));
}

template <typename TInputPointSet, typename TOutputImage>
void
PointSetToImageFilter<TInputPointSet, TOutputImage>::SetSpacing(const float * v)
{
  this->SetSpacing(SpacingType(v));
}

template <typename TInputPointSet, typename TOutputImage>
void
PointSetToImageFilter<TInputPointSet, TOutputImage>::SetSpacing(const double * v)
{
  this->SetSpacing(SpacingType(v));
}

template <typename TInputPointSet, typename TOutputImage>
void
PointSetToImageFilter<TInputPointSet, TOutputImage>::SetOrigin(const float * v)
{
  this->SetOrigin(PointType(v));
}

template <typename TInputPointSet, typename TOutputImage>
void
PointSetToImageFilter<TInputPointSet, TOutputImage>::SetOrigin(const double * v)
{
  this->SetOrigin(PointType(v));
}

//----------------------------------------------------------------------------

/** Update */
template <typename TInputPointSet, typename TOutputImage>
void
PointSetToImageFilter<TInputPointSet, TOutputImage>::GenerateData()
{
  itkDebugMacro("PointSetToImageFilter::Update() called");

  // Get the input and output pointers
  const InputPointSetType * InputPointSet = this->GetInput();
  const OutputImagePointer  OutputImage = this->GetOutput();

  using BoundingBoxType = BoundingBox<typename InputPointSetType::PointIdentifier,
                                      InputPointSetDimension,
                                      typename InputPointSetType::CoordinateType,
                                      typename InputPointSetType::PointsContainer>;
  auto bb = BoundingBoxType::New();
  bb->SetPoints(InputPointSet->GetPoints());
  bb->ComputeBoundingBox();

  // Generate the image
  double   origin[InputPointSetDimension];
  SizeType size;
  for (unsigned int i = 0; i < InputPointSetDimension; ++i)
  {
    size[i] = static_cast<SizeValueType>(bb->GetBounds()[2 * i + 1] - bb->GetBounds()[2 * i]);
    origin[i] = bb->GetBounds()[2 * i];
  }

  typename OutputImageType::RegionType region;

  // If the size of the output has been explicitly specified, the filter
  // will set the output size to the explicit size, otherwise the size from the
  // spatial
  // PointSet's bounding box will be used as default.

  bool specified = false;
  for (unsigned int i = 0; i < OutputImageDimension; ++i)
  {
    if (m_Size[i] != SizeValueType{})
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

  OutputImage->SetRegions(region);

  // If the spacing has been explicitly specified, the filter
  // will set the output spacing to that explicit spacing, otherwise the spacing
  // from
  // the point-set is used as default.

  specified = false;
  for (unsigned int i = 0; i < OutputImageDimension; ++i)
  {
    if (Math::NotExactlyEquals(m_Spacing[i], typename NumericTraits<SpacingType>::ValueType{}))
    {
      specified = true;
      break;
    }
  }

  if (specified)
  {
    OutputImage->SetSpacing(this->m_Spacing); // set spacing
  }

  specified = false;
  for (unsigned int i = 0; i < OutputImageDimension; ++i)
  {
    if (Math::NotExactlyEquals(m_Origin[i], typename NumericTraits<PointType>::ValueType{}))
    {
      specified = true;
      break;
    }
  }

  if (specified)
  {
    for (unsigned int i = 0; i < OutputImageDimension; ++i)
    {
      origin[i] = m_Origin[i]; // set origin
    }
  }

  OutputImage->SetOrigin(origin);         //   and origin
  OutputImage->SetDirection(m_Direction); //   and Direction
  OutputImage->Allocate();                // allocate the image
  OutputImage->FillBuffer(m_OutsideValue);

  using PointIterator = typename InputPointSetType::PointsContainer::ConstIterator;
  PointIterator       pointItr = InputPointSet->GetPoints()->Begin();
  const PointIterator pointEnd = InputPointSet->GetPoints()->End();

  typename OutputImageType::IndexType index;

  while (pointItr != pointEnd)
  {
    if (OutputImage->TransformPhysicalPointToIndex(pointItr.Value(), index))
    {
      OutputImage->SetPixel(index, m_InsideValue);
    }
    ++pointItr;
  }

  itkDebugMacro("PointSetToImageFilter::Update() finished");
} // end update function

template <typename TInputPointSet, typename TOutputImage>
void
PointSetToImageFilter<TInputPointSet, TOutputImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Size : " << m_Size << std::endl;
  os << indent << "Origin: " << m_Origin << std::endl;
  os << indent << "Spacing: " << m_Spacing << std::endl;
  os << indent << "Direction: " << m_Direction << std::endl;
  os << indent << "Inside Value : " << static_cast<typename NumericTraits<ValueType>::PrintType>(m_InsideValue)
     << std::endl;
  os << indent << "Outside Value : " << static_cast<typename NumericTraits<ValueType>::PrintType>(m_OutsideValue)
     << std::endl;
}
} // end namespace itk

#endif
