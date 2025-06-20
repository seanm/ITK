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

#include "itkFEMUtility.h"

namespace itk::fem
{

/**
 * Numerical integration (Gauss-Legendre formula).
 * Integrates function f(x) from x=a to x=b in n points.
 */
double
GaussIntegrate::Integrate(double (*f)(double), double a, double b, int n)
{
  /**
   * This subprogram produces the gauss-legendre numerical
   * integral for

   *   b
   * int   f(x)*dx
   *    a
   *
   * The number of nodes n must satisfy
   *    2 <= n <= 20
   * The following data statements contain the gauss-legendre
   * nodes for the interval [-1,1].
   */


  /*  Begin integration  */

  double scale = (b - a) / two;
  double sum;
  int    m;
  int    ibase;
  if ((n & 1) == 0)
  {
    m = n / 2;
    ibase = m * m;
    sum = zero;
  }
  else
  {
    m = (n - 1) / 2;
    ibase = (n * n - 1) / 4;
    sum = w[ibase + m] * (*f)((a + b) / two);
  }
  for (int i = 1; i <= m; ++i)
  {
    double t = z[ibase + i - 1];
    double tl = (a * (one + t) + (one - t) * b) / two;
    double tu = (a * (one - t) + (one + t) * b) / two;
    sum = sum + w[ibase + i - 1] * ((*f)(tl) + (*f)(tu));
  }

  return scale * sum;
}

} // namespace itk::fem
