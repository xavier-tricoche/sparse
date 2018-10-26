all:
	gcc -I/home/sbarakat/Chaos2Rost/Projects/SamerLibraries \
		-IASPSS \
		-I/usr/include/qt4/ \
		-I/usr/include/qt4/Qt/ \
		-I/usr/include/qt4/QtCore/ \
		-I/usr/include/atlas \
		-I/media/SDA2/scratch/sbarakat/Libraries/cuda/include \
		-I/media/SDA2/scratch/sbarakat/Libraries/NVIDIA_GPU_Computing_SDK/C/common/inc \
		-I/media/SDA2/scratch/sbarakat/Libraries/nvis/nvis/ \
		-I/media/SDA2/scratch/sbarakat/Libraries/vtk-5.8.0/vtk-binaries/include/vtk-5.8/ \
		-I/media/SDA2/scratch/sbarakat/Libraries/celltree/benchmark \
		-I/media/SDA2/scratch/sbarakat/Libraries/celltree/src \
		-I/media/SDA2/scratch/sbarakat/Libraries/celltree/fantom \
		-I/media/SDA2/scratch/sbarakat/Libraries/InsightToolkit-4.3.1/installation/include/ITK-4.3 \
		-I/media/SDA2/scratch/sbarakat/Libraries/CGAL-4.1/include \
		-I/media/SDA2/scratch/sbarakat/Libraries/alglib-3.6.0.cpp/cpp/src \
		-I/usr/include -I/usr/local/include -I/opt/local/include \
		-L/media/SDA2/scratch/sbarakat/Libraries/vtk-5.8.0/vtk-binaries/lib/vtk-5.8/ \
		-L/media/SDA2/scratch/sbarakat/Libraries/InsightToolkit-4.3.1/installation/lib \
		-L/media/SDA2/scratch/sbarakat/Libraries/CGAL-4.1/lib \
		-L/media/SDA2/scratch/sbarakat/Libraries/alglib-3.6.0.cpp/cpp/lib \
		-L/usr/lib -L/usr/local/lib -L/opt/local/lib \
		-lMapReduceMPI -lmpistubs -lvtkalglib -lvtkCharts -lvtkCommon -lvtkDICOMParser -lvtkmetaio -lvtkNetCDF -lvtkNetCDF_cxx -lvtkpng -lvtkproj4 -lvtkRendering -lvtksqlite -lvtkexpat -lvtkFiltering -lvtkfreetype -lvtkftgl -lvtkGenericFiltering -lvtksys -lvtktiff -lvtkverdict -lvtkViews -lvtkVolumeRendering -lvtkWidgets -lvtkexoIIc -lvtklibxml2 -lvtkzlib -lvtkGeovis -lvtkGraphics -lvtkhdf5 -lvtkHybrid -lvtkImaging -lvtkInfovis -lvtkIO -lvtkjpeg \
		-lgsl -lgslcblas -lteem -lCGAL -latlas -llapack -lcblas -lalglib -lQtCore -lQtGui -lQtScript -lQtScriptTools -lQtDeclarative -litkpng-4.3 -lITKIONRRD-4.3 -lITKIOPNG-4.3 -lITKVTK-4.3 -lITKCommon-4.3 \
		-litksys-4.3 -lITKWatersheds-4.3 -lITKNrrdIO-4.3 -lITKIOImageBase-4.3 -lITKIOStimulate-4.3 -lITKMesh-4.3 -lITKMetaIO-4.3 -litkvnl-4.3 -lITKVNLInstantiation-4.3 -litkvnl_algo-4.3 -litkzlib-4.3 -litkvcl-4.3 \
		-litkv3p_lsqr-4.3 -litkv3p_netlib-4.3 -lITKIOTransformBase-4.3 -litkNetlibSlatec-4.3 -lITKPath-4.3 -lITKPolynomials-4.3 -lITKKLMRegionGrowing-4.3 -lITKLabelMap-4.3 -lITKIOVTK-4.3 -lITKIOMesh-4.3 -lITKStatistics-4.3 \
		-lITKIOSpatialObjects-4.3  -lITKOptimizers-4.3 -lITKOptimizersv4-4.3 -lITKQuadEdgeMesh-4.3 -lITKReview-4.3 -lITKSpatialObjects-4.3 -lITKVideoCore-4.3 -lITKVideoIO-4.3 -lITKIOMeta-4.3 -lITKBiasCorrection-4.3 \
		-lITKBioCell-4.3 -lITKDICOMParser-4.3 -lITKEXPAT-4.3 -lITKFEM-4.3 -litkgdcmCommon-4.3 -litkgdcmDICT-4.3 -litkgdcmDSED-4.3 -litkgdcmIOD-4.3 -litkgdcmjpeg12-4.3 -litktiff-4.3 -lITKznz-4.3 -litkgdcmjpeg8-4.3 \
		-litkgdcmMSFF-4.3 -litkgdcmuuid-4.3 -lITKgiftiio-4.3 -litkhdf5-4.3 -litkhdf5_cpp-4.3 -lITKIOBioRad-4.3 -lITKIOBMP-4.3 -lITKIOCSV-4.3 -lITKIOGDCM-4.3 -lITKIOGE-4.3 -lITKIOGIPL-4.3 -lITKIOHDF5-4.3 -lITKIOIPL-4.3 \
		-lITKIOJPEG-4.3 -lITKIOLSM-4.3 -lITKIONIFTI-4.3 -lITKIOSiemens-4.3 -litkgdcmjpeg16-4.3 -lITKIOTIFF-4.3 -lITKIOTransformHDF5-4.3 -lITKIOTransformInsightLegacy-4.3 -lITKIOTransformMatlab-4.3 -lITKIOXML-4.3 -litkjpeg-4.3 \
		-lITKniftiio-4.3 -litkopenjpeg-4.3 -lnetcdf \
		-fopenmp -frounding-math -lm -w -Wfatal-errors -O2 -m64 \
		-o AdaptiveSampling3DParticle main.cpp ASPSS/ExpeAlgebraicSphere.cpp ASPSS/ExpeAxisAlignedBox.cpp ASPSS/ExpeBallNeighborhood.cpp ASPSS/ExpeBasicMesh2PointSet.cpp ASPSS/ExpeColor.cpp ASPSS/ExpeEigenPlaneFitter.cpp ASPSS/ExpeEigenSphereFitter.cpp ASPSS/ExpeEigenSphericalMlsSurface.cpp ASPSS/ExpeEuclideanNeighborhood.cpp ASPSS/ExpeGeometryAutoReshape.cpp ASPSS/ExpeGeometryObject.cpp ASPSS/ExpeGeometryOperator.cpp ASPSS/ExpeGolubSphereFitter.cpp ASPSS/ExpeHalfedgeConnectivity.cpp ASPSS/ExpeImplicitSurface.cpp ASPSS/ExpeKdTree.cpp ASPSS/ExpeLinearAlgebra.cpp ASPSS/ExpeLocalMlsApproximationSurface.cpp ASPSS/ExpeLogManager.cpp ASPSS/ExpeMath.cpp ASPSS/ExpeMatrix3.cpp ASPSS/ExpeMesh.cpp ASPSS/ExpeMeshNormalEvaluator.cpp ASPSS/ExpeMlsSurface.cpp ASPSS/ExpeNeighborhood.cpp ASPSS/ExpeNormalConstrainedSphereFitter.cpp ASPSS/ExpeNormalConstrainedSphericalMlsSurface.cpp ASPSS/ExpePointSet.cpp ASPSS/ExpePolynomialFitter.cpp ASPSS/ExpeQuaternion.cpp ASPSS/ExpeQueryDataStructure.cpp ASPSS/ExpeQueryGrid.cpp ASPSS/ExpeRgba.cpp ASPSS/ExpeSerializableObject.cpp ASPSS/ExpeSimplePSS.cpp ASPSS/ExpeSphericalMlsSurface.cpp ASPSS/ExpeStaticInitializer.cpp ASPSS/ExpeTypedObject.cpp ASPSS/ExpeVector2.cpp ASPSS/ExpeVector3.cpp ASPSS/ExpeVector4.cpp ASPSS/ExpeWeightingFunction.cpp Sample_point.cpp Timer.cpp SmoothStepFitting1D.cpp DiscreteSibson.cpp 
clean: 
	rm AdaptiveSampling3DParticle;
