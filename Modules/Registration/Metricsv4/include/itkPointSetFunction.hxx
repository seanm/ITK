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
#ifndef itkPointSetFunction_hxx
#define itkPointSetFunction_hxx


namespace itk
{

template <typename TInputPointSet, typename TOutput, typename TCoordinate>
PointSetFunction<TInputPointSet, TOutput, TCoordinate>::PointSetFunction()
  : m_PointSet(nullptr)
{}

template <typename TInputPointSet, typename TOutput, typename TCoordinate>
void
PointSetFunction<TInputPointSet, TOutput, TCoordinate>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  itkPrintSelfObjectMacro(PointSet);
}

template <typename TInputPointSet, typename TOutput, typename TCoordinate>
void
PointSetFunction<TInputPointSet, TOutput, TCoordinate>::SetInputPointSet(const InputPointSetType * ptr)
{
  this->m_PointSet = ptr;
}

} // end namespace itk

#endif
