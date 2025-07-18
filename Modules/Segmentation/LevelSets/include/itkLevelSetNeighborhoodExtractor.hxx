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
#ifndef itkLevelSetNeighborhoodExtractor_hxx
#define itkLevelSetNeighborhoodExtractor_hxx

#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"
#include "itkMath.h"

#include <algorithm>

namespace itk
{

template <typename TLevelSet>
LevelSetNeighborhoodExtractor<TLevelSet>::LevelSetNeighborhoodExtractor()
  : m_InsidePoints(nullptr)
  , m_OutsidePoints(nullptr)
  , m_InputLevelSet(nullptr)
  , m_InputNarrowBand(nullptr)
  , m_LargeValue(NumericTraits<PixelType>::max())

{
  m_NodesUsed.resize(SetDimension);
}

template <typename TLevelSet>
void
LevelSetNeighborhoodExtractor<TLevelSet>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "LevelSetValue: " << m_LevelSetValue << std::endl;

  os << indent << "InsidePoints: " << m_InsidePoints << std::endl;
  os << indent << "OutsidePoints: " << m_OutsidePoints << std::endl;
  os << indent << "InputLevelSet: " << m_InputLevelSet << std::endl;
  itkPrintSelfBooleanMacro(NarrowBanding);
  os << indent << "NarrowBandwidth: " << m_NarrowBandwidth << std::endl;
  os << indent << "InputNarrowBand: " << m_InputNarrowBand << std::endl;
  os << indent << "ImageRegion: " << m_ImageRegion << std::endl;
  os << indent << "LargeValue: " << static_cast<typename NumericTraits<PixelType>::PrintType>(m_LargeValue)
     << std::endl;
  // ToDo
  // os << indent << "NodesUsed: " << m_NodesUsed << std::endl;
  itkPrintSelfBooleanMacro(LastPointIsInside);
}

template <typename TLevelSet>
void
LevelSetNeighborhoodExtractor<TLevelSet>::SetInputNarrowBand(NodeContainer * ptr)
{
  if (m_InputNarrowBand != ptr)
  {
    m_InputNarrowBand = ptr;
    this->Modified();
  }
}

template <typename TLevelSet>
void
LevelSetNeighborhoodExtractor<TLevelSet>::Locate()
{
  this->GenerateData();
}

template <typename TLevelSet>
void
LevelSetNeighborhoodExtractor<TLevelSet>::Initialize()
{
  // create new empty points containers
  m_InsidePoints = NodeContainer::New();
  m_OutsidePoints = NodeContainer::New();

  m_ImageRegion = this->m_InputLevelSet->GetBufferedRegion();
}

template <typename TLevelSet>
void
LevelSetNeighborhoodExtractor<TLevelSet>::GenerateData()
{
  if (!m_InputLevelSet)
  {
    itkExceptionMacro("Input level set is nullptr");
  }

  this->Initialize();

  if (m_NarrowBanding)
  {
    this->GenerateDataNarrowBand();
  }
  else
  {
    this->GenerateDataFull();
  }

  itkDebugMacro("No. inside points: " << m_InsidePoints->Size());
  itkDebugMacro("No. outside points: " << m_OutsidePoints->Size());
}

template <typename TLevelSet>
void
LevelSetNeighborhoodExtractor<TLevelSet>::GenerateDataFull()
{
  using InputConstIterator = ImageRegionConstIterator<LevelSetImageType>;

  InputConstIterator inIt(m_InputLevelSet, m_InputLevelSet->GetBufferedRegion());

  IndexType inputIndex;

  const SizeValueType totalPixels = m_InputLevelSet->GetBufferedRegion().GetNumberOfPixels();
  SizeValueType       updateVisits = totalPixels / 10;
  if (updateVisits < 1)
  {
    updateVisits = 1;
  }

  for (SizeValueType i = 0; !inIt.IsAtEnd(); ++inIt, ++i)
  {
    // update progress
    if (!(i % updateVisits))
    {
      this->UpdateProgress(static_cast<float>(i) / static_cast<float>(totalPixels));
    }

    inputIndex = inIt.GetIndex();
    this->CalculateDistance(inputIndex);
  }
}

template <typename TLevelSet>
void
LevelSetNeighborhoodExtractor<TLevelSet>::GenerateDataNarrowBand()
{
  if (!m_InputNarrowBand)
  {
    itkExceptionMacro("InputNarrowBand has not been set");
  }

  typename NodeContainer::ConstIterator pointsIter;
  typename NodeContainer::ConstIterator pointsEnd;

  pointsIter = m_InputNarrowBand->Begin();
  pointsEnd = m_InputNarrowBand->End();
  NodeType     node;
  const double maxValue = m_NarrowBandwidth / 2.0;

  const SizeValueType totalPixels = m_InputNarrowBand->Size();
  SizeValueType       updateVisits = totalPixels / 10;
  if (updateVisits < 1)
  {
    updateVisits = 1;
  }

  for (unsigned int i = 0; pointsIter != pointsEnd; ++pointsIter, ++i)
  {
    // update progress
    if (!(i % updateVisits))
    {
      this->UpdateProgress(static_cast<float>(i) / static_cast<float>(totalPixels));
    }

    node = pointsIter.Value();
    if (itk::Math::abs(node.GetValue()) <= maxValue)
    {
      this->CalculateDistance(node.GetIndex());
    }
  }
}

template <typename TLevelSet>
double
LevelSetNeighborhoodExtractor<TLevelSet>::CalculateDistance(IndexType & index)
{
  m_LastPointIsInside = false;

  typename LevelSetImageType::PixelType centerValue;
  PixelType                             inputPixel = m_InputLevelSet->GetPixel(index);
  centerValue = static_cast<double>(inputPixel);
  centerValue -= m_LevelSetValue;

  NodeType centerNode;
  centerNode.SetIndex(index);

  if (centerValue == 0.0)
  {
    centerNode.SetValue(0.0);
    m_InsidePoints->InsertElement(m_InsidePoints->Size(), centerNode);
    m_LastPointIsInside = true;
    return 0.0;
  }

  const bool inside = (centerValue <= 0.0);

  IndexType neighIndex = index;
  NodeType  neighNode;

  // In each dimension, find the distance to the zero set
  // by linear interpolating along the grid line.
  for (unsigned int j = 0; j < SetDimension; ++j)
  {
    neighNode.SetValue(m_LargeValue);
    SpacePrecisionType spacing = m_InputLevelSet->GetSpacing()[j];

    for (int s = -1; s < 2; s = s + 2)
    {
      neighIndex[j] = index[j] + s;

      if (!this->m_ImageRegion.IsInside(neighIndex))
      {
        continue;
      }

      inputPixel = m_InputLevelSet->GetPixel(neighIndex);
      typename LevelSetImageType::PixelType neighValue = inputPixel;
      neighValue -= m_LevelSetValue;

      if ((neighValue > 0 && inside) || (neighValue < 0 && !inside))
      {
        SpacePrecisionType distance = centerValue / (centerValue - neighValue) * spacing;

        if (neighNode.GetValue() > distance)
        {
          neighNode.SetValue(distance);
          neighNode.SetIndex(neighIndex);
        }
      }
    } // end one dim loop

    // put the minimum distance neighbor onto the heap
    m_NodesUsed[j] = neighNode;

    // reset neighIndex
    neighIndex[j] = index[j];
  } // end dimension loop

  // sort the neighbors according to distance
  std::sort(m_NodesUsed.begin(), m_NodesUsed.end());

  // The final distance is given by the minimum distance to the plane
  // crossing formed by the zero set crossing points.
  SpacePrecisionType distance = 0.0;
  for (unsigned int j = 0; j < SetDimension; ++j)
  {
    neighNode = m_NodesUsed[j];

    if (neighNode.GetValue() >= m_LargeValue)
    {
      break;
    }

    distance += 1.0 / itk::Math::sqr(static_cast<double>(neighNode.GetValue()));
  }

  if (distance == 0.0)
  {
    return m_LargeValue;
  }

  distance = std::sqrt(1.0 / distance);
  centerNode.SetValue(distance);

  if (inside)
  {
    m_InsidePoints->InsertElement(m_InsidePoints->Size(), centerNode);
    m_LastPointIsInside = true;
  }
  else
  {
    m_OutsidePoints->InsertElement(m_OutsidePoints->Size(), centerNode);
    m_LastPointIsInside = false;
  }

  return distance;
}
} // namespace itk

#endif
