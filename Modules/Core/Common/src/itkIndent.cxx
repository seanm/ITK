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
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#include "itkObjectFactory.h"

constexpr int ITK_STD_INDENT = 2;
constexpr int ITK_NUMBER_OF_BLANKS = 40;

namespace itk
{
static constexpr char blanks[ITK_NUMBER_OF_BLANKS + 1] = "                                        ";

Indent *
Indent::New()
{
  return new Self;
}

Indent
Indent::GetNextIndent() const
{
  int indent = m_Indent + ITK_STD_INDENT;

  if (indent > ITK_NUMBER_OF_BLANKS)
  {
    indent = ITK_NUMBER_OF_BLANKS;
  }
  return Indent{ indent };
}

std::ostream &
operator<<(std::ostream & os, const Indent & ind)
{
  os << blanks + (ITK_NUMBER_OF_BLANKS - ind.m_Indent);
  return os;
}
} // end namespace itk
