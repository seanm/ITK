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

#include <iostream>
#include "itkRealTimeStamp.h"
#include "itkNumericTraits.h"
#include "itkMath.h"
#include "itkTestingMacros.h"

#define CHECK_FOR_VALUE(a, b)                                                            \
  {                                                                                      \
    double eps = 4.0 * itk::NumericTraits<double>::epsilon();                            \
    ITK_GCC_PRAGMA_PUSH                                                                  \
    ITK_GCC_SUPPRESS_Wfloat_equal                                                        \
    eps = (b == 0.0) ? eps : itk::Math::abs(b * eps);                                    \
    ITK_GCC_PRAGMA_POP                                                                   \
    if (itk::Math::abs(a - b) > eps)                                                     \
    {                                                                                    \
      std::cerr << "Error in " #a << " expected " << b << " but got " << a << std::endl; \
      return EXIT_FAILURE;                                                               \
    }                                                                                    \
  }                                                                                      \
  ITK_MACROEND_NOOP_STATEMENT

#define CHECK_FOR_BOOLEAN(x, expected)          \
  {                                             \
    if ((x) != expected)                        \
    {                                           \
      std::cerr << "Error in " #x << std::endl; \
      return EXIT_FAILURE;                      \
    }                                           \
  }                                             \
  ITK_MACROEND_NOOP_STATEMENT

int
itkRealTimeStampTest(int, char *[])
{
  const itk::RealTimeStamp stamp0;

  const double timeInMicroSeconds = stamp0.GetTimeInMicroSeconds();
  const double timeInMilliSeconds = stamp0.GetTimeInMilliSeconds();
  double       timeInSeconds = stamp0.GetTimeInSeconds();
  const double timeInHours = stamp0.GetTimeInHours();
  const double timeInDays = stamp0.GetTimeInDays();

  CHECK_FOR_VALUE(timeInMicroSeconds, 0.0);
  CHECK_FOR_VALUE(timeInMilliSeconds, 0.0);
  CHECK_FOR_VALUE(timeInSeconds, 0.0);
  CHECK_FOR_VALUE(timeInHours, 0.0);
  CHECK_FOR_VALUE(timeInDays, 0.0);

  const itk::RealTimeStamp stamp1;
  itk::RealTimeStamp       stamp2 = stamp0;

  const itk::RealTimeInterval minusOneSecond(-1, 0);
  ITK_TRY_EXPECT_EXCEPTION(stamp2 += minusOneSecond);

  const itk::RealTimeInterval oneSecond(1, 0);

  for (unsigned int i = 0; i < 1000000L; ++i)
  {
    stamp2 += oneSecond;
  }

  std::cout << "Stamp2 = " << stamp2 << std::endl;

  itk::RealTimeInterval manySeconds = stamp2 - stamp0;

  timeInSeconds = manySeconds.GetTimeInSeconds();

  CHECK_FOR_VALUE(timeInSeconds, 1000000.0);

  itk::RealTimeInterval fiveMicroseconds;
  fiveMicroseconds.Set(0, 5);

  itk::RealTimeStamp stamp3 = stamp0;

  for (unsigned int i = 0; i < 1000000L; ++i)
  {
    stamp3 += fiveMicroseconds;
  }

  manySeconds = stamp3 - stamp0;

  timeInSeconds = manySeconds.GetTimeInSeconds();

  CHECK_FOR_VALUE(timeInSeconds, 5.0);

  for (unsigned int i = 0; i < 1000000L; ++i)
  {
    stamp3 -= fiveMicroseconds;
  }

  manySeconds = stamp3 - stamp0;

  timeInSeconds = manySeconds.GetTimeInSeconds();

  CHECK_FOR_VALUE(timeInSeconds, 0.0);

  ITK_TRY_EXPECT_EXCEPTION(stamp3 += minusOneSecond);

  itk::RealTimeInterval timeSpan;

  timeSpan.Set(19, -5000000L);

  timeInSeconds = timeSpan.GetTimeInSeconds();

  CHECK_FOR_VALUE(timeInSeconds, 14.0);

  timeSpan.Set(-19, 5000000L);

  timeInSeconds = timeSpan.GetTimeInSeconds();

  CHECK_FOR_VALUE(timeInSeconds, -14.0);

  timeSpan.Set(-19, -5000000L);

  timeInSeconds = timeSpan.GetTimeInSeconds();

  CHECK_FOR_VALUE(timeInSeconds, -24.0);

  timeSpan.Set(19, 5000000L);

  timeInSeconds = timeSpan.GetTimeInSeconds();

  CHECK_FOR_VALUE(timeInSeconds, 24.0);


  const itk::RealTimeInterval timeSpan1(19, 300000L);
  const itk::RealTimeInterval timeSpan2(13, 500000L);

  const itk::RealTimeInterval timeSpan3 = timeSpan1 + timeSpan2;

  timeInSeconds = timeSpan3.GetTimeInSeconds();

  CHECK_FOR_VALUE(timeInSeconds, 32.8);

  // Test comparison operations
  const itk::RealTimeInterval dt1(15, 13);
  const itk::RealTimeInterval dt2(19, 11);
  const itk::RealTimeInterval dt3(15, 25);

  itk::RealTimeInterval t1;
  t1 += dt1;

  itk::RealTimeInterval t2;
  t2 += dt2;

  itk::RealTimeInterval t3;
  t3 += dt3;

  CHECK_FOR_BOOLEAN(t1 == t1, true);
  CHECK_FOR_BOOLEAN(t1 != t2, true);
  CHECK_FOR_BOOLEAN(t1 != t1, false);
  CHECK_FOR_BOOLEAN(t2 >= t1, true);
  CHECK_FOR_BOOLEAN(t1 >= t1, true);
  CHECK_FOR_BOOLEAN(t2 > t1, true);
  CHECK_FOR_BOOLEAN(t1 <= t2, true);
  CHECK_FOR_BOOLEAN(t1 <= t1, true);
  CHECK_FOR_BOOLEAN(t1 < t2, true);

  CHECK_FOR_BOOLEAN(t3 == t3, true);
  CHECK_FOR_BOOLEAN(t1 != t3, true);
  CHECK_FOR_BOOLEAN(t3 >= t1, true);
  CHECK_FOR_BOOLEAN(t3 > t1, true);
  CHECK_FOR_BOOLEAN(t3 <= t1, false);
  CHECK_FOR_BOOLEAN(t3 < t1, false);
  CHECK_FOR_BOOLEAN(t1 <= t3, true);
  CHECK_FOR_BOOLEAN(t1 < t3, true);
  CHECK_FOR_BOOLEAN(t1 >= t3, false);
  CHECK_FOR_BOOLEAN(t1 > t3, false);


  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}
