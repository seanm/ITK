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

#include "itkMesh.h"
#include "itkTriangleCell.h"

#include <iostream>


int
itkTriangleCellTest(int, char *[])
{


  /**
   * Define a mesh type that stores a PixelType of "int".  Use the defaults for
   * the other template parameters.
   */
  using MeshType = itk::Mesh<int>;
  using CellTraits = MeshType::CellTraits;

  /**
   * Define a few cell types which uses a PixelType of "int".  Again,
   * use the defaults for the other parameters.  Note that a cell's template
   * parameters must match those of the mesh into which it is inserted.
   */
  using CellInterfaceType = itk::CellInterface<int, CellTraits>;
  using TriangleCellType = itk::TriangleCell<CellInterfaceType>;

  class TriangleHelper : public TriangleCellType
  {
    using Superclass = TriangleCellType;
    using CoordinateType = Superclass::CoordinateType;
    using PointsContainer = Superclass::PointsContainer;
    using InterpolationWeightType = Superclass::InterpolationWeightType;

  public:
    bool
    EvaluatePosition(CoordinateType *          inputPoint,
                     PointsContainer *         points,
                     CoordinateType *          closestPoint,
                     CoordinateType            pcoord[],
                     double *                  distance,
                     InterpolationWeightType * weights) override
    {
      return this->Superclass::EvaluatePosition(inputPoint, points, closestPoint, pcoord, distance, weights);
    }
  };


  /**
   * Typedef the generic cell type for the mesh.  It is an abstract class,
   * so we can only use information from it, like get its pointer type.
   */
  using CellType = MeshType::CellType;
  using CellAutoPointer = CellType::CellAutoPointer;

  /**
   * The type of point stored in the mesh. Because mesh was instantiated
   * with defaults (itkDefaultStaticMeshTraits), the point dimension is 3 and
   * the coordinate representation is float.
   */
  using PointType = MeshType::PointType;


  /**
   * Create the mesh through its object factory.
   */
  auto mesh = MeshType::New();
  mesh->DebugOn();

  constexpr unsigned int numberOfPoints = 4;
  /**
   * Define the 3d geometric positions for 4 points in a square.
   */
  constexpr MeshType::CoordinateType testPointCoords[numberOfPoints][3] = {
    { 0, 0, 0 }, { 10, 0, 0 }, { 10, 10, 0 }, { 0, 10, 0 }
  };

  /**
   * Add our test points to the mesh.
   * mesh->SetPoint(pointId, point)
   * Note that the constructor for Point is public, and takes an array
   * of coordinates for the point.
   */
  for (unsigned int i = 0; i < numberOfPoints; ++i)
  {
    mesh->SetPoint(i, PointType(testPointCoords[i]));
  }

  /**
   * Specify the method used for allocating cells
   */
  mesh->SetCellsAllocationMethod(itk::MeshEnums::MeshClassCellsAllocationMethod::CellsAllocatedDynamicallyCellByCell);

  /**
   * Create the test cell. Note that testCell is a generic auto
   * pointer to a cell; in this example it ends up pointing to
   * different types of cells.
   */
  CellAutoPointer testCell;
  auto *          newcell = new TriangleHelper;
  testCell.TakeOwnership(newcell); // polymorphism

  /**
   * List the points that the polygon will use from the mesh.
   */
  MeshType::PointIdentifier polygon1Points1[3] = { 2, 0, 1 };

  /**
   * Assign the points to the tetrahedron through their identifiers.
   */
  testCell->SetPointIds(polygon1Points1);

  /**
   * Add the test cell to the mesh.
   * mesh->SetCell(cellId, cell)
   */
  mesh->SetCell(0, testCell); // Transfer ownership to the mesh
  std::cout << "TriangleCell pointer = " << (const void *)testCell.GetPointer() << std::endl;
  std::cout << "TriangleCell Owner   = " << testCell.IsOwner() << std::endl;

  {
    std::cout << "Test MakeCopy" << std::endl;

    CellAutoPointer anotherCell;

    testCell->MakeCopy(anotherCell);

    if (anotherCell->GetNumberOfPoints() != testCell->GetNumberOfPoints())
    {
      std::cerr << "Make Copy failed !" << std::endl;
      return EXIT_FAILURE;
    }
  }

  //
  // Exercise the EvaluatePosition() method of the TriangleCell
  //
  TriangleCellType::CoordinateType          inputPoint[3];
  TriangleCellType::PointsContainer *       points = mesh->GetPoints();
  TriangleCellType::CoordinateType          closestPoint[3];
  TriangleCellType::CoordinateType          pcoords[3];
  TriangleCellType::InterpolationWeightType weights[3];

  constexpr double tolerance = 1e-5;

  // Test 1:
  inputPoint[0] = 5.0;
  inputPoint[1] = 3.0;
  inputPoint[2] = 0.0;

  std::cout << "Calling EvaluatePosition for ";
  std::cout << inputPoint[0] << ", ";
  std::cout << inputPoint[1] << ", ";
  std::cout << inputPoint[2] << std::endl;
  double distance = NAN;
  bool   isInside = testCell->EvaluatePosition(inputPoint, points, closestPoint, pcoords, &distance, weights);

  if (!isInside)
  {
    std::cerr << "Error: point should be reported as being inside" << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(pcoords[0] - 0.3) > tolerance) || (itk::Math::abs(pcoords[1] - 0.5) > tolerance) ||
      (itk::Math::abs(pcoords[2] - 0.2) > tolerance))
  {
    std::cerr << "Error: pcoords computed incorrectly" << std::endl;
    std::cerr << "pcoords[0] = " << pcoords[0] << std::endl;
    std::cerr << "pcoords[1] = " << pcoords[1] << std::endl;
    std::cerr << "pcoords[2] = " << pcoords[2] << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(weights[0] - 0.3) > tolerance) || (itk::Math::abs(weights[1] - 0.5) > tolerance) ||
      (itk::Math::abs(weights[2] - 0.2) > tolerance))
  {
    std::cerr << "Error: weights computed incorrectly" << std::endl;
    std::cerr << "weights[0] = " << weights[0] << std::endl;
    std::cerr << "weights[1] = " << weights[1] << std::endl;
    std::cerr << "weights[2] = " << weights[2] << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(closestPoint[0] - 5.0) > tolerance) || (itk::Math::abs(closestPoint[1] - 3.0) > tolerance) ||
      (itk::Math::abs(closestPoint[2] - 0.0) > tolerance))
  {
    std::cerr << "Error: closestPoint computed incorrectly" << std::endl;
    std::cerr << "closestPoint[0] = " << closestPoint[0] << std::endl;
    std::cerr << "closestPoint[1] = " << closestPoint[1] << std::endl;
    std::cerr << "closestPoint[2] = " << closestPoint[2] << std::endl;
    return EXIT_FAILURE;
  }


  // Test 2:
  inputPoint[0] = 15.0;
  inputPoint[1] = 5.0;
  inputPoint[2] = 0.0;

  std::cout << "Calling EvaluatePosition for ";
  std::cout << inputPoint[0] << ", ";
  std::cout << inputPoint[1] << ", ";
  std::cout << inputPoint[2] << std::endl;

  isInside = testCell->EvaluatePosition(inputPoint, points, closestPoint, pcoords, &distance, weights);

  if (isInside)
  {
    std::cerr << "Error: point should be reported as being outside" << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(pcoords[0] - 0.5) > tolerance) || (itk::Math::abs(pcoords[1] + 0.5) > tolerance) ||
      (itk::Math::abs(pcoords[2] - 1.0) > tolerance))
  {
    std::cerr << "Error: pcoords computed incorrectly" << std::endl;
    std::cerr << "pcoords[0] = " << pcoords[0] << std::endl;
    std::cerr << "pcoords[1] = " << pcoords[1] << std::endl;
    std::cerr << "pcoords[2] = " << pcoords[2] << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(closestPoint[0] - 10.0) > tolerance) || (itk::Math::abs(closestPoint[1] - 5.0) > tolerance) ||
      (itk::Math::abs(closestPoint[2] - 0.0) > tolerance))
  {
    std::cerr << "Error: closestPoint computed incorrectly" << std::endl;
    std::cerr << "closestPoint[0] = " << closestPoint[0] << std::endl;
    std::cerr << "closestPoint[1] = " << closestPoint[1] << std::endl;
    std::cerr << "closestPoint[2] = " << closestPoint[2] << std::endl;
    return EXIT_FAILURE;
  }

  //
  // NOTE: Outside points don't get their weights computed.
  //


  // Test 3:
  inputPoint[0] = 0.0;
  inputPoint[1] = 10.0;
  inputPoint[2] = 0.0;

  std::cout << "Calling EvaluatePosition for ";
  std::cout << inputPoint[0] << ", ";
  std::cout << inputPoint[1] << ", ";
  std::cout << inputPoint[2] << std::endl;

  isInside = testCell->EvaluatePosition(inputPoint, points, closestPoint, pcoords, &distance, weights);

  if (isInside)
  {
    std::cerr << "Error: point should be reported as being outside" << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(pcoords[0] - 1.0) > tolerance) || (itk::Math::abs(pcoords[1] - 1.0) > tolerance) ||
      (itk::Math::abs(pcoords[2] + 1.0) > tolerance))
  {
    std::cerr << "Error: pcoords computed incorrectly" << std::endl;
    std::cerr << "pcoords[0] = " << pcoords[0] << std::endl;
    std::cerr << "pcoords[1] = " << pcoords[1] << std::endl;
    std::cerr << "pcoords[2] = " << pcoords[2] << std::endl;
    return EXIT_FAILURE;
  }

  // note should be half way up the line (0,0,0)  -> (10,10,0)
  if ((itk::Math::abs(closestPoint[0] - 5.0) > tolerance) || (itk::Math::abs(closestPoint[1] - 5.0) > tolerance) ||
      (itk::Math::abs(closestPoint[2] - 0.0) > tolerance))
  {
    std::cerr << "Error: closestPoint computed incorrectly" << std::endl;
    std::cerr << "closestPoint[0] = " << closestPoint[0] << std::endl;
    std::cerr << "closestPoint[1] = " << closestPoint[1] << std::endl;
    std::cerr << "closestPoint[2] = " << closestPoint[2] << std::endl;
    return EXIT_FAILURE;
  }

  //
  // NOTE: Outside points don't get their weights computed.
  //


  // Test 4:
  inputPoint[0] = 5.0;
  inputPoint[1] = -5.0;
  inputPoint[2] = 0.0;

  std::cout << "Calling EvaluatePosition for ";
  std::cout << inputPoint[0] << ", ";
  std::cout << inputPoint[1] << ", ";
  std::cout << inputPoint[2] << std::endl;

  isInside = testCell->EvaluatePosition(inputPoint, points, closestPoint, pcoords, &distance, weights);

  if (isInside)
  {
    std::cerr << "Error: point should be reported as being outside" << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(pcoords[0] + 0.5) > tolerance) || (itk::Math::abs(pcoords[1] - 0.5) > tolerance) ||
      (itk::Math::abs(pcoords[2] - 1.0) > tolerance))
  {
    std::cerr << "Error: pcoords computed incorrectly" << std::endl;
    std::cerr << "pcoords[0] = " << pcoords[0] << std::endl;
    std::cerr << "pcoords[1] = " << pcoords[1] << std::endl;
    std::cerr << "pcoords[2] = " << pcoords[2] << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(closestPoint[0] - 5.0) > tolerance) || (itk::Math::abs(closestPoint[1] - 0.0) > tolerance) ||
      (itk::Math::abs(closestPoint[2] - 0.0) > tolerance))
  {
    std::cerr << "Error: closestPoint computed incorrectly" << std::endl;
    std::cerr << "closestPoint[0] = " << closestPoint[0] << std::endl;
    std::cerr << "closestPoint[1] = " << closestPoint[1] << std::endl;
    std::cerr << "closestPoint[2] = " << closestPoint[2] << std::endl;
    return EXIT_FAILURE;
  }

  //
  // NOTE: Outside points don't get their weights computed.
  //


  // Test 5:
  inputPoint[0] = -5.0;
  inputPoint[1] = -3.0;
  inputPoint[2] = 0.0;

  std::cout << "Calling EvaluatePosition for ";
  std::cout << inputPoint[0] << ", ";
  std::cout << inputPoint[1] << ", ";
  std::cout << inputPoint[2] << std::endl;

  isInside = testCell->EvaluatePosition(inputPoint, points, closestPoint, pcoords, &distance, weights);

  if (isInside)
  {
    std::cerr << "Error: point should be reported as being outside" << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(pcoords[0] + 0.3) > tolerance) || (itk::Math::abs(pcoords[1] - 1.5) > tolerance) ||
      (itk::Math::abs(pcoords[2] + 0.2) > tolerance))
  {
    std::cerr << "Error: pcoords computed incorrectly" << std::endl;
    std::cerr << "pcoords[0] = " << pcoords[0] << std::endl;
    std::cerr << "pcoords[1] = " << pcoords[1] << std::endl;
    std::cerr << "pcoords[2] = " << pcoords[2] << std::endl;
    return EXIT_FAILURE;
  }

  if ((itk::Math::abs(closestPoint[0] - 0.0) > tolerance) || (itk::Math::abs(closestPoint[1] - 0.0) > tolerance) ||
      (itk::Math::abs(closestPoint[2] - 0.0) > tolerance))
  {
    std::cerr << "Error: closestPoint computed incorrectly" << std::endl;
    std::cerr << "closestPoint[0] = " << closestPoint[0] << std::endl;
    std::cerr << "closestPoint[1] = " << closestPoint[1] << std::endl;
    std::cerr << "closestPoint[2] = " << closestPoint[2] << std::endl;
    return EXIT_FAILURE;
  }

  //
  // NOTE: Outside points don't get their weights computed.
  //

  return EXIT_SUCCESS;
}
