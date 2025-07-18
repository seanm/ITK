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
#include "itkConjugateGradientLineSearchOptimizerv4.h"
#include "itkTestingMacros.h"

/**
 *  \class ConjugateGradientLineSearchOptimizerv4TestMetric for test
 *
 *  The objectif function is the quadratic form:
 *
 *  1/2 x^T A x - b^T x
 *
 *  Where A is a matrix and b is a vector
 *  The system in this example is:
 *
 *     | 3  2 ||x|   | 2|   |0|
 *     | 2  6 ||y| + |-8| = |0|
 *
 *
 *   the solution is the vector | 2 -2 |
 *
 */
class ConjugateGradientLineSearchOptimizerv4TestMetric : public itk::ObjectToObjectMetricBase
{
public:
  using Self = ConjugateGradientLineSearchOptimizerv4TestMetric;
  using Superclass = itk::ObjectToObjectMetricBase;
  using Pointer = itk::SmartPointer<Self>;
  using ConstPointer = itk::SmartPointer<const Self>;
  itkNewMacro(Self);
  itkOverrideGetNameOfClassMacro(ConjugateGradientLineSearchOptimizerv4TestMetric);

  enum
  {
    SpaceDimension = 2
  };

  using ParametersType = Superclass::ParametersType;
  using ParametersValueType = Superclass::ParametersValueType;
  using DerivativeType = Superclass::DerivativeType;
  using MeasureType = Superclass::MeasureType;

  ConjugateGradientLineSearchOptimizerv4TestMetric()

  {
    m_Parameters.SetSize(SpaceDimension);
    m_Parameters.Fill(0);
  }

  void
  Initialize() override
  {}

  void
  GetDerivative(DerivativeType & derivative) const override
  {
    MeasureType value = NAN;
    GetValueAndDerivative(value, derivative);
  }

  void
  GetValueAndDerivative(MeasureType & value, DerivativeType & derivative) const override
  {
    if (derivative.Size() != 2)
    {
      derivative.SetSize(2);
    }

    const double x = m_Parameters[0];
    const double y = m_Parameters[1];

    std::cout << "GetValueAndDerivative( ";
    std::cout << x << ' ';
    std::cout << y << ") = " << std::endl;

    value = 0.5 * (3 * x * x + 4 * x * y + 6 * y * y) - 2 * x + 8 * y;

    std::cout << "value: " << value << std::endl;

    /* The optimizer simply takes the derivative from the metric
     * and adds it to the transform after scaling. So instead of
     * setting a 'minimize' option in the gradient, we return
     * a minimizing derivative. */
    derivative[0] = -(3 * x + 2 * y - 2);
    derivative[1] = -(2 * x + 6 * y + 8);
    std::cout << "derivative: " << derivative << " iteration " << m_Iterations << std::endl;
    m_Iterations++;
  }

  MeasureType
  GetValue() const override
  {
    const double x = m_Parameters[0];
    const double y = m_Parameters[1];

    const MeasureType value = 0.5 * (3 * x * x + 4 * x * y + 6 * y * y) - 2 * x + 8 * y;

    return value;
  }

  void
  UpdateTransformParameters(const DerivativeType & update, ParametersValueType) override
  {
    m_Parameters += update;
  }

  unsigned int
  GetNumberOfParameters() const override
  {
    return SpaceDimension;
  }

  unsigned int
  GetNumberOfLocalParameters() const override
  {
    return SpaceDimension;
  }

  bool
  HasLocalSupport() const override
  {
    return false;
  }

  /* These Set/Get methods are only needed for this test derivation that
   * isn't using a transform */
  void
  SetParameters(ParametersType & parameters) override
  {
    m_Parameters = parameters;
  }

  const ParametersType &
  GetParameters() const override
  {
    return m_Parameters;
  }

private:
  ParametersType       m_Parameters;
  mutable unsigned int m_Iterations{ 0 };
};

///////////////////////////////////////////////////////////
int
ConjugateGradientLineSearchOptimizerv4RunTest(itk::ConjugateGradientLineSearchOptimizerv4::Pointer & itkOptimizer)
{
  try
  {
    std::cout << "currentPosition before optimization: " << itkOptimizer->GetCurrentPosition() << std::endl;
    itkOptimizer->StartOptimization();
    std::cout << "currentPosition after optimization: " << itkOptimizer->GetCurrentPosition() << std::endl;
  }
  catch (const itk::ExceptionObject & e)
  {
    std::cout << "Exception thrown ! " << std::endl;
    std::cout << "An error occurred during Optimization" << std::endl;
    std::cout << "Location    = " << e.GetLocation() << std::endl;
    std::cout << "Description = " << e.GetDescription() << std::endl;
    return EXIT_FAILURE;
  }

  using ParametersType = ConjugateGradientLineSearchOptimizerv4TestMetric::ParametersType;
  ParametersType finalPosition = itkOptimizer->GetMetric()->GetParameters();
  std::cout << "Solution        = (";
  std::cout << finalPosition[0] << ',';
  std::cout << finalPosition[1] << ')' << std::endl;

  //
  // check results to see if it is within range
  //
  constexpr double trueParameters[2] = { 2, -2 };
  for (unsigned int j = 0; j < 2; ++j)
  {
    if (itk::Math::abs(finalPosition[j] - trueParameters[j]) > 0.01)
    {
      std::cerr << "Results do not match: " << std::endl
                << "expected: " << trueParameters[0] << ", " << trueParameters[1] << std::endl
                << "returned: " << finalPosition << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
///////////////////////////////////////////////////////////
int
itkConjugateGradientLineSearchOptimizerv4Test(int, char *[])
{
  std::cout << "Gradient Descent Object Optimizer Test ";
  std::cout << std::endl << std::endl;

  using OptimizerType = itk::ConjugateGradientLineSearchOptimizerv4;

  using ScalesType = OptimizerType::ScalesType;

  // Declaration of an itkOptimizer
  auto itkOptimizer = OptimizerType::New();

  // Declaration of the Metric
  const ConjugateGradientLineSearchOptimizerv4TestMetric::Pointer metric =
    ConjugateGradientLineSearchOptimizerv4TestMetric::New();

  itkOptimizer->SetMetric(metric);

  using ParametersType = ConjugateGradientLineSearchOptimizerv4TestMetric::ParametersType;

  const unsigned int spaceDimension = metric->GetNumberOfParameters();

  // We start not so far from  | 2 -2 |
  ParametersType initialPosition(spaceDimension);

  initialPosition[0] = 100;
  initialPosition[1] = -100;
  metric->SetParameters(initialPosition);

  itkOptimizer->SetLearningRate(0.1);
  itkOptimizer->SetNumberOfIterations(50);

  // test the optimization
  std::cout << "Test optimization 1:" << std::endl;
  if (ConjugateGradientLineSearchOptimizerv4RunTest(itkOptimizer) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  //
  // test with non-identity scales
  //
  std::cout << "Test optimization with non-identity scales:" << std::endl;
  metric->SetParameters(initialPosition);
  ScalesType scales(metric->GetNumberOfLocalParameters());
  scales.Fill(0.5);
  itkOptimizer->SetScales(scales);
  itkOptimizer->SetLowerLimit(0);
  itkOptimizer->SetUpperLimit(5);
  itkOptimizer->SetEpsilon(1.e-4);
  itkOptimizer->SetMaximumLineSearchIterations(5);
  metric->SetParameters(initialPosition);
  if (ConjugateGradientLineSearchOptimizerv4RunTest(itkOptimizer) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  std::cout << "Test optimization with few linear search iterations:" << std::endl;
  scales.Fill(0.5);
  itkOptimizer->SetMaximumLineSearchIterations(2);
  itkOptimizer->SetScales(scales);
  itkOptimizer->SetLearningRate(0.5);
  itkOptimizer->SetLowerLimit(0);
  itkOptimizer->SetUpperLimit(1);
  itkOptimizer->SetEpsilon(1.e-4);
  metric->SetParameters(initialPosition);
  if (ConjugateGradientLineSearchOptimizerv4RunTest(itkOptimizer) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  // Test repeat runs with the same parameters
  initialPosition[0] = 200;
  initialPosition[1] = -200;

  metric->SetParameters(initialPosition);

  // local variable to store initial learning rate
  double initialLearningRate = 0.5;

  itkOptimizer = OptimizerType::New(); // Reinitialize optimizer
  itkOptimizer->SetMetric(metric);     // Reattach the metric
  itkOptimizer->SetMaximumLineSearchIterations(5);
  itkOptimizer->SetLearningRate(initialLearningRate);
  itkOptimizer->SetLowerLimit(0.1);
  itkOptimizer->SetUpperLimit(5);
  itkOptimizer->SetEpsilon(1.e-4);

  itkOptimizer->SetNumberOfIterations(50);

  std::cout << "Test optimization retest 1 (initial):" << std::endl;
  if (ConjugateGradientLineSearchOptimizerv4RunTest(itkOptimizer) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  // Store the first result
  ParametersType firstFinalPosition = itkOptimizer->GetMetric()->GetParameters();

  metric->SetParameters(initialPosition);

  // Run the optimizer a second time
  std::cout << "Test optimization 2 (restart with same parameters):" << std::endl;
  if (ConjugateGradientLineSearchOptimizerv4RunTest(itkOptimizer) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

  // Store the second result
  ParametersType secondFinalPosition = itkOptimizer->GetMetric()->GetParameters();

  // Compare results
  bool identical = true;
  for (unsigned int j = 0; j < spaceDimension; ++j)
  {
    if (itk::Math::abs(firstFinalPosition[j] - secondFinalPosition[j]) > itk::NumericTraits<double>::epsilon())
    {
      identical = false;
    }
  }

  if (!identical)
  {
    std::cerr << "ERROR: Optimizer does not reset properly: results differ runs with same parameters." << std::endl;
    std::cerr << "First run result: " << firstFinalPosition << std::endl;
    std::cerr << "Second run result: " << secondFinalPosition << std::endl;
    return EXIT_FAILURE;
  }
  else
  {
    std::cout << "SUCCESS: Optimizer produces identical results on restart." << std::endl;
  }

  // Exercise various member functions
  itkOptimizer->SetInitialLearningRate(0.5);
  std::cout << "LearningRate: " << itkOptimizer->GetLearningRate();
  std::cout << std::endl;
  std::cout << "InitialLearningRate: " << itkOptimizer->GetInitialLearningRate();
  std::cout << std::endl;
  std::cout << "NumberOfIterations: " << itkOptimizer->GetNumberOfIterations();
  std::cout << std::endl;

  itkOptimizer->Print(std::cout);
  std::cout << "Stop description   = " << itkOptimizer->GetStopConditionDescription() << std::endl;

  auto badOptimizer = OptimizerType::New();
  bool caught = false;
  try
  {
    badOptimizer->GetCurrentPosition();
  }
  catch (const itk::ExceptionObject & e)
  {
    std::cout << "Caught expected exception!";
    std::cout << e << std::endl;
    caught = true;
  }

  if (!caught)
  {
    std::cout << "Failed to catch expected exception! " << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Printing self.. " << std::endl;
  std::cout << itkOptimizer << std::endl;

  std::cout << "Test passed." << std::endl;
  return EXIT_SUCCESS;
}
