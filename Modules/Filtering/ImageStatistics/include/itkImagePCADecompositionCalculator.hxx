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
#ifndef itkImagePCADecompositionCalculator_hxx
#define itkImagePCADecompositionCalculator_hxx

#include "itkImageRegionConstIterator.h"
#include "itkPrintHelper.h"

namespace itk
{

template <typename TInputImage, typename TBasisImage>
ImagePCADecompositionCalculator<TInputImage, TBasisImage>::ImagePCADecompositionCalculator()
  : m_MeanImage(nullptr)
  , m_Image(nullptr)
{}

template <typename TInputImage, typename TBasisImage>
void
ImagePCADecompositionCalculator<TInputImage, TBasisImage>::SetBasisImages(const BasisImagePointerVector & v)
{
  itkDebugMacro("setting BasisImages");
  this->m_BasisMatrixCalculated = false;
  // We need this modified setter function so that the calculator
  // can cache the basis set between calculations. Note that computing the
  // basis matrix from the input images is rather expensive, and the basis
  // images are likely to be changed less often than the input images. So
  // it makes sense to try to cache the pre-computed matrix.

  this->m_BasisImages = v;
  this->Modified();
}

template <typename TInputImage, typename TBasisImage>
void
ImagePCADecompositionCalculator<TInputImage, TBasisImage>::Compute()
{
  if (!m_BasisMatrixCalculated)
  {
    this->CalculateBasisMatrix();
  }
  this->CalculateRecenteredImageAsVector();
  m_Projection = m_BasisMatrix * m_ImageAsVector;
}

template <typename TInputImage, typename TBasisImage>
void
ImagePCADecompositionCalculator<TInputImage, TBasisImage>::CalculateBasisMatrix()
{
  m_Size = m_BasisImages[0]->GetRequestedRegion().GetSize();
  m_NumPixels = 1;
  for (unsigned int i = 0; i < BasisImageDimension; ++i)
  {
    m_NumPixels *= m_Size[i];
  }

  m_BasisMatrix = BasisMatrixType(static_cast<unsigned int>(m_BasisImages.size()), m_NumPixels);

  int i = 0;
  for (auto basis_it = m_BasisImages.begin(); basis_it != m_BasisImages.end(); ++basis_it)
  {
    if ((*basis_it)->GetRequestedRegion().GetSize() != m_Size)
    {
      itkExceptionMacro("All basis images must be the same size!");
    }

    ImageRegionConstIterator<BasisImageType> image_it(*basis_it, (*basis_it)->GetRequestedRegion());
    int                                      j = 0;
    for (image_it.GoToBegin(); !image_it.IsAtEnd(); ++image_it)
    {
      m_BasisMatrix(i, j++) = image_it.Get();
    }
    ++i;
  }
  m_BasisMatrixCalculated = true;
  m_ImageAsVector.set_size(m_NumPixels);
}

template <typename TInputImage, typename TBasisImage>
void
ImagePCADecompositionCalculator<TInputImage, TBasisImage>::CalculateRecenteredImageAsVector()
{
  if (m_Image->GetRequestedRegion().GetSize() != m_Size)
  {
    itkExceptionMacro("Input image must be the same size as the basis images!");
  }

  ImageRegionConstIterator<InputImageType> image_it(m_Image, m_Image->GetRequestedRegion());
  typename BasisVectorType::iterator       vector_it;
  for (image_it.GoToBegin(), vector_it = m_ImageAsVector.begin(); !image_it.IsAtEnd(); ++image_it, ++vector_it)
  {
    *vector_it = static_cast<BasisPixelType>(image_it.Get());
  }

  if (m_MeanImage)
  {
    ImageRegionConstIterator<BasisImageType> mimage_it(m_MeanImage, m_MeanImage->GetRequestedRegion());
    for (mimage_it.GoToBegin(), vector_it = m_ImageAsVector.begin(); !mimage_it.IsAtEnd(); ++mimage_it, ++vector_it)
    {
      *vector_it -= (mimage_it.Get());
    }
  }
}

template <typename TInputImage, typename TBasisImage>
void
ImagePCADecompositionCalculator<TInputImage, TBasisImage>::SetBasisFromModel(ModelPointerType model)
{
  BasisImagePointerVector images;
  unsigned int            nImages = model->GetNumberOfPrincipalComponentsRequired();

  images.reserve(nImages);
  for (unsigned int i = 1; i <= nImages; ++i)
  {
    images.push_back(model->GetOutput(i));
  }
  this->SetBasisImages(images);
  this->SetMeanImage(model->GetOutput(0));
}

template <typename TInputImage, typename TBasisImage>
void
ImagePCADecompositionCalculator<TInputImage, TBasisImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  using namespace print_helper;

  Superclass::PrintSelf(os, indent);

  os << indent << "Projection: " << m_Projection << std::endl;
  os << indent << "ImageAsVector: " << m_ImageAsVector << std::endl;
  os << indent << "BasisImages: " << m_BasisImages << std::endl;

  itkPrintSelfObjectMacro(MeanImage);

  os << indent << "m_Size: " << static_cast<typename NumericTraits<BasisSizeType>::PrintType>(m_Size) << std::endl;

  itkPrintSelfObjectMacro(Image);

  os << indent << "BasisMatrix: " << m_BasisMatrix << std::endl;
  itkPrintSelfBooleanMacro(BasisMatrixCalculated);
  os << indent << "NumPixels: " << static_cast<typename NumericTraits<SizeValueType>::PrintType>(m_NumPixels)
     << std::endl;
}
} // end namespace itk

#endif
