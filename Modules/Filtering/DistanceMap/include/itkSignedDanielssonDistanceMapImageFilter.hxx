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
#ifndef itkSignedDanielssonDistanceMapImageFilter_hxx
#define itkSignedDanielssonDistanceMapImageFilter_hxx

#include "itkProgressAccumulator.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"

namespace itk
{
/**
 *    Constructor
 */
template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
SignedDanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::
  SignedDanielssonDistanceMapImageFilter()

{
  // Make the outputs (distance map, voronoi map, distance vectors).
  ProcessObject::MakeRequiredOutputs(*this, 3);
}

/** This is overloaded to create the VectorDistanceMap output image */
template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
auto
SignedDanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::MakeOutput(
  DataObjectPointerArraySizeType idx) -> DataObjectPointer
{
  if (idx == 1)
  {
    return VoronoiImageType::New().GetPointer();
  }
  if (idx == 2)
  {
    return VectorImageType::New().GetPointer();
  }
  return Superclass::MakeOutput(idx);
}

/**
 *  Return the distance map Image pointer
 */
template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
auto
SignedDanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::GetDistanceMap() -> OutputImageType *
{
  return dynamic_cast<OutputImageType *>(this->ProcessObject::GetOutput(0));
}

/**
 *  Return Closest Points Map
 */
template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
auto
SignedDanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::GetVoronoiMap() -> VoronoiImageType *
{
  return dynamic_cast<VoronoiImageType *>(this->ProcessObject::GetOutput(1));
}

/**
 *  Return the distance vectors
 */
template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
auto
SignedDanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::GetVectorDistanceMap()
  -> VectorImageType *
{
  return dynamic_cast<VectorImageType *>(this->ProcessObject::GetOutput(2));
}

/**
 *  Compute Distance and Voronoi maps by calling
 * DanielssonDistanceMapImageFilter twice.
 */
template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
void
SignedDanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::GenerateData()
{
  // Set up mini pipeline filter
  auto progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  using FilterType = DanielssonDistanceMapImageFilter<InputImageType, OutputImageType, VoronoiImageType>;
  auto filter1 = FilterType::New();
  auto filter2 = FilterType::New();

  filter1->SetUseImageSpacing(m_UseImageSpacing);
  filter2->SetUseImageSpacing(m_UseImageSpacing);
  filter1->SetSquaredDistance(m_SquaredDistance);
  filter2->SetSquaredDistance(m_SquaredDistance);

  // Invert input image for second Danielsson filter
  using InputPixelType = typename InputImageType::PixelType;
  using FunctorType = Functor::InvertIntensityFunctor<InputPixelType>;
  using InverterType = UnaryFunctorImageFilter<InputImageType, InputImageType, FunctorType>;

  auto inverter = InverterType::New();

  inverter->SetInput(this->GetInput());

  // Dilate the inverted image by 1 pixel to give it the same boundary
  // as the uninverted input.

  using StructuringElementType = BinaryBallStructuringElement<InputPixelType, InputImageDimension>;

  using DilatorType = BinaryDilateImageFilter<InputImageType, InputImageType, StructuringElementType>;

  auto dilator = DilatorType::New();

  StructuringElementType structuringElement;
  structuringElement.SetRadius(1); // 3x3 structuring element
  structuringElement.CreateStructuringElement();
  dilator->SetKernel(structuringElement);
  dilator->SetDilateValue(1);

  filter1->SetInput(this->GetInput());
  dilator->SetInput(inverter->GetOutput());
  filter2->SetInput(dilator->GetOutput());

  // Subtract Distance maps results of the two Danielsson filters
  using SubtracterType = SubtractImageFilter<OutputImageType, OutputImageType, OutputImageType>;

  auto subtracter = SubtracterType::New();

  if (m_InsideIsPositive)
  {
    subtracter->SetInput1(filter2->GetDistanceMap());
    subtracter->SetInput2(filter1->GetDistanceMap());
  }
  else
  {
    subtracter->SetInput2(filter2->GetDistanceMap());
    subtracter->SetInput1(filter1->GetDistanceMap());
  }

  subtracter->Update();
  filter1->Update();
  filter2->Update();

  // Register progress
  progress->RegisterInternalFilter(filter1, .5f);

  // Graft outputs
  this->GraftNthOutput(0, subtracter->GetOutput());

  // we must use not use this->GetOutput method because the
  // output's are of different types then ImageSource
  this->GraftNthOutput(1, filter1->GetVoronoiMap());
  this->GraftNthOutput(2, filter1->GetVectorDistanceMap());
}

// end GenerateData()

/**
 *  Print Self
 */
template <typename TInputImage, typename TOutputImage, typename TVoronoiImage>
void
SignedDanielssonDistanceMapImageFilter<TInputImage, TOutputImage, TVoronoiImage>::PrintSelf(std::ostream & os,
                                                                                            Indent         indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Signed Danielson Distance: " << std::endl;
  os << indent << "Use Image Spacing : " << m_UseImageSpacing << std::endl;
  os << indent << "Squared Distance  : " << m_SquaredDistance << std::endl;
  os << indent << "Inside is positive  : " << m_InsideIsPositive << std::endl;
}
} // end namespace itk

#endif
