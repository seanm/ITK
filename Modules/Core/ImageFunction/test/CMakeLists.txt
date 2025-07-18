itk_module_test()
set(
  ITKImageFunctionTests
  itkGaussianBlurImageFunctionTest.cxx
  itkVectorInterpolateImageFunctionTest.cxx
  itkVarianceImageFunctionTest.cxx
  itkMedianImageFunctionTest.cxx
  itkBinaryThresholdImageFunctionTest.cxx
  itkBSplineDecompositionImageFilterTest.cxx
  itkBSplineInterpolateImageFunctionTest.cxx
  itkBSplineResampleImageFunctionTest.cxx
  itkScatterMatrixImageFunctionTest.cxx
  itkMeanImageFunctionTest.cxx
  itkGaussianDerivativeImageFunctionTest.cxx
  itkCentralDifferenceImageFunctionTest.cxx
  itkCentralDifferenceImageFunctionOnVectorTest.cxx
  itkImageAdaptorInterpolateImageFunctionTest.cxx
  itkCovarianceImageFunctionTest.cxx
  itkRayCastInterpolateImageFunctionTest.cxx
  itkNearestNeighborExtrapolateImageFunctionTest.cxx
  itkVectorMeanImageFunctionTest.cxx
  itkMahalanobisDistanceThresholdImageFunctionTest.cxx
  itkInterpolateTest.cxx
  itkRGBInterpolateImageFunctionTest.cxx
  itkWindowedSincInterpolateImageFunctionTest.cxx
  itkLinearInterpolateImageFunctionTest.cxx
  itkNeighborhoodOperatorImageFunctionTest.cxx
  itkNearestNeighborInterpolateImageFunctionTest.cxx
  itkGaussianInterpolateImageFunctionTest.cxx
  itkLabelImageGaussianInterpolateImageFunctionTest.cxx
  itkVectorLinearInterpolateNearestNeighborExtrapolateImageFunctionTest.cxx
  itkCentralDifferenceImageFunctionSpeedTest.cxx
  itkCentralDifferenceImageFunctionOnVectorSpeedTest.cxx
)

createtestdriver(ITKImageFunction "${ITKImageFunction-Test_LIBRARIES}" "${ITKImageFunctionTests}")

itk_add_test(
  NAME itkGaussianBlurImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkGaussianBlurImageFunctionTest
)
itk_add_test(
  NAME itkVectorInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkVectorInterpolateImageFunctionTest
)
itk_add_test(
  NAME itkVarianceImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkVarianceImageFunctionTest
)
itk_add_test(
  NAME itkMedianImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkMedianImageFunctionTest
)
itk_add_test(
  NAME itkBinaryThresholdImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkBinaryThresholdImageFunctionTest
)
itk_add_test(
  NAME itkBSplineDecompositionImageFilterTest
  COMMAND
    ITKImageFunctionTestDriver
    itkBSplineDecompositionImageFilterTest
    3
    -0.26794919243112281
)
itk_add_test(
  NAME itkBSplineInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkBSplineInterpolateImageFunctionTest
)
itk_add_test(
  NAME itkBSplineResampleImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkBSplineResampleImageFunctionTest
)
itk_add_test(
  NAME itkScatterMatrixImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkScatterMatrixImageFunctionTest
)
itk_add_test(
  NAME itkMeanImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkMeanImageFunctionTest
)
itk_add_test(
  NAME itkGaussianDerivativeImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkGaussianDerivativeImageFunctionTest
)
itk_add_test(
  NAME itkCentralDifferenceImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkCentralDifferenceImageFunctionTest
)
itk_add_test(
  NAME itkCentralDifferenceImageFunctionOnVectorTest
  COMMAND
    ITKImageFunctionTestDriver
    itkCentralDifferenceImageFunctionOnVectorTest
)
itk_add_test(
  NAME itkImageAdaptorInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkImageAdaptorInterpolateImageFunctionTest
)
itk_add_test(
  NAME itkCovarianceImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkCovarianceImageFunctionTest
)
itk_add_test(
  NAME itkRayCastInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkRayCastInterpolateImageFunctionTest
)
itk_add_test(
  NAME itkNearestNeighborExtrapolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkNearestNeighborExtrapolateImageFunctionTest
)
itk_add_test(
  NAME itkVectorMeanImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkVectorMeanImageFunctionTest
)
itk_add_test(
  NAME itkMahalanobisDistanceThresholdImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkMahalanobisDistanceThresholdImageFunctionTest
)
itk_add_test(
  NAME itkInterpolateTest
  COMMAND
    ITKImageFunctionTestDriver
    itkInterpolateTest
)
itk_add_test(
  NAME itkRGBInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkRGBInterpolateImageFunctionTest
)

itk_add_test(
  NAME itkWindowedSincInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkWindowedSincInterpolateImageFunctionTest
)
itk_add_test(
  NAME itkLinearInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkLinearInterpolateImageFunctionTest
)
itk_add_test(
  NAME itkNeighborhoodOperatorImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkNeighborhoodOperatorImageFunctionTest
)
itk_add_test(
  NAME itkNearestNeighborInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkNearestNeighborInterpolateImageFunctionTest
)
itk_add_test(
  NAME itkGaussianInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkGaussianInterpolateImageFunctionTest
)
itk_add_test(
  NAME itkLabelImageGaussianInterpolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkLabelImageGaussianInterpolateImageFunctionTest
)

itk_add_test(
  NAME itkVectorLinearInterpolateNearestNeighborExtrapolateImageFunctionTest
  COMMAND
    ITKImageFunctionTestDriver
    itkVectorLinearInterpolateNearestNeighborExtrapolateImageFunctionTest
)

set(ITKImageFunctionGTests itkSumOfSquaresImageFunctionGTest.cxx)
creategoogletestdriver(ITKImageFunction "${ITKImageFunction-Test_LIBRARIES}" "${ITKImageFunctionGTests}")
