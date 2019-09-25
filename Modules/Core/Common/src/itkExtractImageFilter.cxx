/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkExtractImageFilter.h"

namespace itk
{
/** Define how to print enumeration */
std::ostream &
operator<<(std::ostream & out, const ExtractImageFilterCollapseStrategy value)
{
  return out << [value] {
    switch (value)
    {
      case ExtractImageFilterCollapseStrategy::DIRECTIONCOLLAPSETOUNKOWN:
        return "ExtractImageFilterCollapseStrategy::DIRECTIONCOLLAPSETOUNKOWN";
      case ExtractImageFilterCollapseStrategy::DIRECTIONCOLLAPSETOIDENTITY:
        return "ExtractImageFilterCollapseStrategy::DIRECTIONCOLLAPSETOIDENTITY";
      case ExtractImageFilterCollapseStrategy::DIRECTIONCOLLAPSETOSUBMATRIX:
        return "ExtractImageFilterCollapseStrategy::DIRECTIONCOLLAPSETOSUBMATRIX";
      case ExtractImageFilterCollapseStrategy::DIRECTIONCOLLAPSETOGUESS:
        return "ExtractImageFilterCollapseStrategy::DIRECTIONCOLLAPSETOGUESS";
      default:
        return "INVALID VALUE FOR ExtractImageFilterCollapseStrategy";
    }
  }();
}
} // end namespace itk