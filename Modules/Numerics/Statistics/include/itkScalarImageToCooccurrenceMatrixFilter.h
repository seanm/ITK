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
#ifndef itkScalarImageToCooccurrenceMatrixFilter_h
#define itkScalarImageToCooccurrenceMatrixFilter_h

#include "itkImage.h"
#include "itkHistogram.h"
#include "itkVectorContainer.h"
#include "itkNumericTraits.h"
#include "itkProcessObject.h"

namespace itk::Statistics
{
/** \class ScalarImageToCooccurrenceMatrixFilter
 *  \brief This class computes a co-occurrence matrix (histogram) from
 * a given image and a mask image if provided. Cooccurrence matrices are
 * used for image texture description.
 *
 * This filters creates a grey-level co-occurrence matrix from a N-D scalar
 * image. This is the first step in texture description a la Haralick. (See
 * \cite haralick1973 and \cite haralick1979.
 *
 * The basic idea is as follows:
 * Given an image and an offset (e.g. (1, -1) for a 2-d image), grey-level
 * co-occurrences are pairs of intensity values for a specific pixel and the
 * pixel at that offset from the specified pixel. These co-occurrences can provide
 * information about the visual texture of an image region -- for example, an
 * eight-bit image of alternating pixel-wide white and black vertical lines
 * would have a large number of (0, 255) and (255, 0) co-occurrences for offset
 * (1, 0).
 *
 * The offset (or offsets) along which the co-occurrences are calculated can be
 * set by the user. Traditionally, only one offset is used per histogram, and
 * offset components in the range [-1, 1] are used. For rotation-invariant features,
 * averages of features computed over several histograms with different offsets
 * are generally used, instead of computing features from one histogram created
 * with several offsets. Additionally, instead of using offsets of two or more
 * pixels in any direction, multi-resolution techniques (e.g. image pyramids)
 * are generally used to deal with texture at different spatial resolutions.
 *
 * This class calculates a 2-d histogram of all the co-occurrence pairs in the
 * given image's requested region, for a given set of offsets. That is, if a given
 * offset falls outside of the requested region at a particular point, that
 * co-occurrence pair will not be added to the matrix.
 *
 * The number of histogram bins on each axis can be set (defaults to 256). Also,
 * by default the histogram min and max corresponds to the largest and smallest
 * possible pixel value of that pixel type. To customize the histogram bounds
 * for a given image, the max and min pixel values that will be placed in the
 * histogram can be set manually. NB: The min and max are INCLUSIVE.
 *
 * Further, the type of histogram frequency container used is an optional template
 * parameter. By default, a dense container is used, but for images with little
 * texture or in cases where the user wants more histogram bins, a sparse container
 * can be used for the histogram instead.
 *
 * WARNING: This probably won't work for pixels of double or long-double type
 * unless you set the histogram min and max manually. This is because the largest
 * histogram bin by default has max value of the largest possible pixel value
 * plus 1. For double and long-double types, whose "RealType" as defined by the
 * NumericTraits class is the same, and thus cannot hold any larger values,
 * this would cause a float overflow.
 *
 * \sa MaskedScalarImageToCooccurrenceMatrixFilter
 * \sa HistogramToTextureFeaturesFilter
 * \sa ScalarImageTextureCalculator
 *
 * \author Zachary Pincus and Glenn Pierce
 *
 * \ingroup ITKStatistics
 */

template <typename TImageType,
          typename THistogramFrequencyContainer = DenseFrequencyContainer2,
          typename TMaskImageType = TImageType>
class ITK_TEMPLATE_EXPORT ScalarImageToCooccurrenceMatrixFilter : public ProcessObject
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(ScalarImageToCooccurrenceMatrixFilter);

  /** Standard type alias */
  using Self = ScalarImageToCooccurrenceMatrixFilter;
  using Superclass = ProcessObject;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(ScalarImageToCooccurrenceMatrixFilter);

  /** standard New() method support */
  itkNewMacro(Self);

  using ImageType = TImageType;
  using ImagePointer = typename ImageType::Pointer;
  using ImageConstPointer = typename ImageType::ConstPointer;
  using PixelType = typename ImageType::PixelType;
  using RegionType = typename ImageType::RegionType;
  using RadiusType = typename ImageType::SizeType;
  using OffsetType = typename ImageType::OffsetType;
  using OffsetVector = VectorContainer<unsigned char, OffsetType>;
  using OffsetVectorPointer = typename OffsetVector::Pointer;
  using OffsetVectorConstPointer = typename OffsetVector::ConstPointer;
  using MaskImageType = TMaskImageType;
  using MaskPointer = typename MaskImageType::Pointer;
  using MaskConstPointer = typename MaskImageType::ConstPointer;
  using MaskPixelType = typename MaskImageType::PixelType;

  using MeasurementType = typename NumericTraits<PixelType>::RealType;

  using HistogramType = Histogram<MeasurementType, THistogramFrequencyContainer>;
  using HistogramPointer = typename HistogramType::Pointer;
  using HistogramConstPointer = typename HistogramType::ConstPointer;
  using MeasurementVectorType = typename HistogramType::MeasurementVectorType;

  static constexpr unsigned int DefaultBinsPerAxis = 256;

  /** Get/Set the offset or offsets over which the co-occurrence pairs will be computed.
      Calling either of these methods clears the previous offsets. */
  /** @ITKStartGrouping */
  itkSetConstObjectMacro(Offsets, OffsetVector);
  itkGetConstObjectMacro(Offsets, OffsetVector);
  /** @ITKEndGrouping */
  void
  SetOffset(const OffsetType offset);

  /** Set number of histogram bins along each axis */
  /** @ITKStartGrouping */
  itkSetMacro(NumberOfBinsPerAxis, unsigned int);
  itkGetConstMacro(NumberOfBinsPerAxis, unsigned int);
  /** @ITKEndGrouping */
  /** Set the min and max (inclusive) pixel value that will be placed in the
    histogram */
  void
  SetPixelValueMinMax(PixelType min, PixelType max);

  itkGetConstMacro(Min, PixelType);
  itkGetConstMacro(Max, PixelType);

  /** Set the calculator to normalize the histogram (divide all bins by the
    total frequency). Normalization is off by default. */
  /** @ITKStartGrouping */
  itkSetMacro(Normalize, bool);
  itkGetConstMacro(Normalize, bool);
  itkBooleanMacro(Normalize);
  /** @ITKEndGrouping */
  /** Method to set/get the image */
  using Superclass::SetInput;
  void
  SetInput(const ImageType * image);

  const ImageType *
  GetInput() const;

  /** Method to set/get the mask image */
  void
  SetMaskImage(const MaskImageType * image);

  const MaskImageType *
  GetMaskImage() const;

  /** method to get the Histogram */
  const HistogramType *
  GetOutput() const;

  /** Set the pixel value of the mask that should be considered "inside" the
    object. Defaults to one. */
  /** @ITKStartGrouping */
  itkSetMacro(InsidePixelValue, MaskPixelType);
  itkGetConstMacro(InsidePixelValue, MaskPixelType);
  /** @ITKEndGrouping */
protected:
  ScalarImageToCooccurrenceMatrixFilter();
  ~ScalarImageToCooccurrenceMatrixFilter() override = default;
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  virtual void
  FillHistogram(RadiusType radius, RegionType region);

  virtual void
  FillHistogramWithMask(RadiusType radius, RegionType region, const MaskImageType * maskImage);

  /** Standard itk::ProcessObject subclass method. */
  using DataObjectPointer = DataObject::Pointer;

  using DataObjectPointerArraySizeType = ProcessObject::DataObjectPointerArraySizeType;
  using Superclass::MakeOutput;
  DataObjectPointer
  MakeOutput(DataObjectPointerArraySizeType idx) override;

  /** This method causes the filter to generate its output. */
  void
  GenerateData() override;

private:
  void
  NormalizeHistogram();

  OffsetVectorConstPointer m_Offsets{};
  PixelType                m_Min{};
  PixelType                m_Max{};

  unsigned int          m_NumberOfBinsPerAxis{};
  MeasurementVectorType m_LowerBound{};
  MeasurementVectorType m_UpperBound{};
  bool                  m_Normalize{};

  MaskPixelType m_InsidePixelValue{};
};
} // namespace itk::Statistics

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkScalarImageToCooccurrenceMatrixFilter.hxx"
#endif

#endif
