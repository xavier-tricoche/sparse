set( ASPSS_LIB_SRC
    ExpeAlgebraicSphere.cpp
    ExpeAxisAlignedBox.cpp
    ExpeBallNeighborhood.cpp
    ExpeBasicMesh2PointSet.cpp
    ExpeColor.cpp
    ExpeEigenPlaneFitter.cpp
    ExpeEigenSphereFitter.cpp
    ExpeEigenSphericalMlsSurface.cpp
    ExpeEuclideanNeighborhood.cpp
    ExpeGeometryAutoReshape.cpp
    ExpeGeometryObject.cpp
    ExpeGeometryOperator.cpp
    ExpeGolubSphereFitter.cpp
    ExpeHalfedgeConnectivity.cpp
    ExpeImplicitSurface.cpp
    ExpeKdTree.cpp
    ExpeLinearAlgebra.cpp
    ExpeLocalMlsApproximationSurface.cpp
    ExpeLogManager.cpp
    ExpeMath.cpp
    ExpeMatrix3.cpp
    ExpeMesh.cpp
    ExpeMeshNormalEvaluator.cpp
    ExpeMlsSurface.cpp
    ExpeNeighborhood.cpp
    ExpeNormalConstrainedSphereFitter.cpp
    ExpeNormalConstrainedSphericalMlsSurface.cpp
    ExpePointSet.cpp ExpePolynomialFitter.cpp
    ExpeQuaternion.cpp
    ExpeQueryDataStructure.cpp
    ExpeQueryGrid.cpp
    ExpeRgba.cpp
    ExpeSerializableObject.cpp
    ExpeSimplePSS.cpp
    ExpeSphericalMlsSurface.cpp
    ExpeStaticInitializer.cpp
    ExpeTypedObject.cpp
    ExpeVector2.cpp
    ExpeVector3.cpp
    ExpeVector4.cpp
    ExpeWeightingFunction.cpp
)

add_library( aspss ${LIBMODE} ${ASPSS_LIB_SRC} )
target_link_libraries( aspss ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${GSL_LIBRARY} )
install( TARGETS aspss
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
