/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBlobSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkBlobSpatialObject_h
#define __itkBlobSpatialObject_h

#include <list>

#include "itkSpatialObject.h"
#include "itkSpatialObjectPoint.h"

namespace itk 
{

/** 
* \class BlobSpatialObject
* \brief Spatial object representing a potentially amorphous object.
*
* The BlobSpatialObject is a discretized representation of a ``blob'',
* which can be taken to be an arbitrary, possibly amorphous shape.
* The representation is a list of the points (voxel centers) contained
* in the object.  This can be thought of as an alternate way to
* represent a binary image.
*
* \sa SpatialObjectPoint
*/

template < unsigned int TDimension = 3 >
class BlobSpatialObject 
:public SpatialObject<  TDimension >
{

public:

  typedef BlobSpatialObject                           Self;
  typedef SpatialObject< TDimension >                 Superclass;
  typedef SmartPointer < Self >                       Pointer;
  typedef SmartPointer < const Self >                 ConstPointer;
  typedef double                                      ScalarType;
  typedef SpatialObjectPoint< TDimension >            BlobPointType; 
  typedef std::list < BlobPointType >                 PointListType;
  typedef typename Superclass::PointType              PointType;
  typedef typename Superclass::TransformType          TransformType;
  typedef VectorContainer<unsigned long,PointType>    PointContainerType;
  typedef SmartPointer<PointContainerType>            PointContainerPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Method for creation through the object factory. */
  itkTypeMacro( BlobSpatialObject, SpatialObject );
  
  /** Returns a reference to the list of the Blob points. */
  PointListType & GetPoints( void );

 /** Returns a reference to the list of the Blob points. */
  const PointListType & GetPoints( void ) const;

  /** Set the list of Blob points.*/
  void SetPoints( PointListType & newPoints );

  /** Returns true if the Blob is evaluable at the requested point, 
   *  false otherwise. */
  bool IsEvaluableAt( const PointType & point, 
                      unsigned int depth=0, char *name = NULL ) const;

  /** Returns the value of the Blob at that point.
   *  Currently this function returns a binary value,
   *  but it might want to return a degree of membership
   *  in case of fuzzy Blobs. */
  bool ValueAt( const PointType & point, double & value,
                unsigned int depth=0, char *name = NULL ) const;

  /** Returns true if the point is inside the Blob, false otherwise. */
  bool IsInside( const PointType & point,
                 unsigned int depth=0, char *name = NULL ) const;

  /** Compute the boundaries of the Blob. */
  bool ComputeBoundingBox( unsigned int depth=0, char *name = NULL );

protected:

  PointListType   m_Points;

  BlobSpatialObject();
  virtual ~BlobSpatialObject();

  /** Method to print the object. */
  virtual void PrintSelf( std::ostream& os, Indent indent ) const;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION 
  #include "itkBlobSpatialObject.txx" 
#endif 

#endif // __itkBlobSpatialObject_h
