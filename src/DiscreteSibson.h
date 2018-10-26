/*************************************************************************
sparse: Efficient Computation of the Flow Map from Sparse Samples

Author: Samer S. Barakat

Copyright (c) 2010-2013, Purdue University

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/
#pragma once

#ifndef __DISCRETESIBSON_H__
#define __DISCRETESIBSON_H__

#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <utility>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <time.h>
#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector_types.h>
#include <vector_functions.h>
#include <cutil_inline.h>
#include <helper_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv.h>
#include <omp.h>

#include <vtkStructuredGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkContourFilter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkPolyDataReader.h>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkLODActor.h>
#include <vtkConeSource.h>
#include <vtkGlyph3D.h>
#include <vtkCellPicker.h>
#include <vtkTextMapper.h>
#include <vtkActor2D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkTubeFilter.h>
#include <vtkPNGReader.h>
#include <vtkImageActor.h>
#include <vtkPointPicker.h>
#include <vtkImageFlip.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkPolyDataReader.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkPolyDataWriter.h>
#include <vtkIdList.h>
#include <vtkColorTransferFunction.h>
#include <vtkClipPolyData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkPlane.h>
#include <vtkRegularPolygonSource.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkArrowSource.h>
#include <vtkMath.h>
#include <vtkX3DExporter.h>
#include <vtkStructuredPointsReader.h>
#include <vtkMarchingCubes.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkSobelEdgeDetectionImageFilter.h>
#include <itkMetaDataObject.h>
#include <itkNrrdImageIO.h>
#include <itkVTKImageIO.h>
#include <itkConnectedComponentImageFilter.h>

#include "MyTeem.h"
#include "MyMath.h"
#include "MyGeometry.h"
#include "Sample_point.h"
#include "SmoothStepFitting1D.h"

#include "ASPSS/ExpePointSet.h"
#include "ASPSS/ExpeNormalConstrainedSphericalMlsSurface.h"
#include "ASPSS/ExpeBallNeighborhood.h"
#include "ASPSS/ExpeEigenSphericalMlsSurface.h"
#include "ASPSS/ExpeKdTree.h"
#include "ASPSS/ExpeGolubSphereFitter.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/basic.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <boost/iterator/zip_iterator.hpp>

#include <dataanalysis.h>

#include <omp.h>

#include "Timer.h"

using namespace std;
using namespace Expe;
using namespace alglib;

// ITK types
const unsigned int Dimension = 3;
typedef unsigned char CharPixelType;  //  IO
typedef float RealPixelType;  //  Operations
typedef itk::Image<CharPixelType, Dimension>    CharImageType;
typedef itk::Image<RealPixelType, Dimension>    RealImageType;
typedef itk::ImageFileReader< RealImageType >  RealReaderType;
typedef itk::ImageFileReader< CharImageType >  CharReaderType;
typedef itk::ImageFileWriter< CharImageType >  CharWriterType;
typedef itk::CannyEdgeDetectionImageFilter<RealImageType, RealImageType> CannyFilter;
typedef itk::RescaleIntensityImageFilter<RealImageType, CharImageType> RescaleFilter;
typedef itk::ConnectedComponentImageFilter<CharImageType, CharImageType> ConnectedComponentImageFilterType;

// CGAL inital types
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point_3;
typedef boost::tuple<Point_3,int> Point_and_int;

//definition of the property map
struct My_point_property_map{
  typedef Point_3 value_type;
  typedef const value_type& reference;
  typedef const Point_and_int& key_type;
  typedef boost::lvalue_property_map_tag category;
};

// types used for the tree search
typedef CGAL::Search_traits_3<Kernel> Traits_base;
typedef CGAL::Search_traits_adapter<Point_and_int,My_point_property_map,Traits_base> Traits;
typedef CGAL::Orthogonal_k_neighbor_search<Traits> K_neighbor_search;
typedef K_neighbor_search::Tree Tree;
typedef K_neighbor_search::Distance Distance;

// structure to hold info per grid point about closest site
struct closest_site {
  int id; //+ve mean site and -ve means curve
  float dist;
};


extern map<string, string> parameters;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NaturalNeighbors
{
public:
	vector<int> nv;
	vector<float> nw;
	int vsize;

	NaturalNeighbors()
	{
		nv.reserve(40);
		nw.reserve(40);
		nv.resize(40);
		nw.resize(40);
		nv.clear();
		nw.clear();

		vsize = 0;
	}

	NaturalNeighbors(const NaturalNeighbors &copy)
    {
		nv.reserve(40);
		nw.reserve(40);
		nv.resize(40);
		nw.resize(40);
		nv.clear();
		nw.clear();

		for (int i = 0; i < copy.nv.size(); i++)
		{
			nv.push_back(copy.nv[i]);
			nw.push_back(copy.nw[i]);
		}

		vsize = nv.size();
    }

	~NaturalNeighbors()
	{
	}

	int insert(int key, float w)
	{
		if (nv.size() == nv.capacity())
		{
			#pragma omp critical
			{
				printf("Error: exceeded natural neighbors max size!\n");
				printf("Current size is %d and current capacity is %d\n", nv.size(), nv.capacity());
				print();
				char c; cin >> c;
			}
			return -1;
		}
		nv.push_back(key);
		nw.push_back(w);
		vsize++;

		return (vsize - 1);


		//int i, j, tmp;
		//float ftmp;
		//j = vsize - 1;
		//while (j > 0 && nv[j - 1] > nv[j])
		//{
		//	// switch ids
		//	tmp = nv[j];
		//	nv[j] = nv[j - 1];
		//	nv[j - 1] = tmp;

		//	// switch weights
		//	ftmp = nw[j];
		//	nw[j] = nw[j - 1];
		//	nw[j - 1] = ftmp;

		//	j--;
		//}
		//return j;
	}

	int find(int key)
	{
		for (int i = 0; i < vsize; i++)
		{
			if (nv[i] == key)
				return i;
		}
		return -1;


		//int imin = 0;
		//int imax = vsize - 1;

		//// continually narrow search until just one element remains
		//while (imin < imax)
		//{
		//	int imid = (imin+imax)/2;

		//	// code must guarantee the interval is reduced at each iteration
		//	assert(imid < imax);
		//	// note: 0 <= imin < imax implies imid will always be less than imax

		//	// reduce the search
		//	if (nv[imid] < key)
		//		imin = imid + 1;
		//	else
		//		imax = imid;
		//}
		//// At exit of while:
		////   if A[] is empty, then imax < imin
		////   otherwise imax == imin

		//// deferred test for equality
		//if ((imax == imin) && (nv[imin] == key))
		//	return imin;
		//else
		//	return -1;
	}

	void reset()
	{
		for (int i = 0; i < vsize; i++)
			nw[i] = 0.0;
	}

	void clear()
	{
		nv.clear();
		nw.clear();
		vsize = 0;
	}

	int size()
	{
		return vsize;
	}

	void removezero()
	{
		int cl = 0;
		for (int i = 0; i < nv.size(); i++)
		{
			if (nw[i] != 0.0)
			{
				nv[cl] = nv[i];
				nw[cl] = nw[i];
				cl++;
			}
		}
		nv.resize(cl);
		nw.resize(cl);
		vsize = nv.size();
	}

	void print()
	{
		for (int i = 0; i < vsize; i++)
		{
			printf("%d: %d %f\n", i, nv[i], nw[i]);
		}
		printf("\n");
	}

	/*float& operator[](unsigned int id)
	{
		int i = find(id);
		if (i < 0)
		{
			i = insert(id, 0.0);
		}

		return (nw[i]);
	}*/
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void dumpMallinfo(void);

void FindClosest(
	void* reconsc,
	vector<closest_site>& query_cls,
	vector<NaturalNeighbors>& query_nc,
	vector<Sample_point>& pts,
	vector<bool>& site_is_disc,
	Tree*& tree,
	int nosurf,
	vector<NormalConstrainedSphericalMlsSurface*>& surfaces,
	vector<set<int> >& site2discs);

void FindNaturalCoordinates(
	void* reconsc,
	vector<closest_site>& query_cls,
	vector<NaturalNeighbors>& query_nc,
	vector<Sample_point>& pts,
	int nosurf,
	vector<NormalConstrainedSphericalMlsSurface*>& surfaces);

void DiscreteSisbon(
	void* originc,
	void* reconsc,
	vector<float>& errm,
	vector<Sample_point>& pts,
	Tree*& tree,
	vector<closest_site>& query_cls,
	vector<NaturalNeighbors>& query_nc);

void DiscreteSisbonWithSurfaces(
	int iter,
	int field,
	void* originc,
	void* reconsc,
	vector<float> errm,
	vector<Sample_point>& pts,
	vector<closest_site> query_cls,
	vector<NaturalNeighbors> query_nc);

void Refine(
	int iter,
	void* originc,
	vector<int>& nids,
	vector<float>& errm,
	vector<Sample_point>& pts,
	Tree*& tree,
	vector<closest_site>& query_cls);

void GenerateSurfaceMesh(
	void* reconsc,
	vector<NormalConstrainedSphericalMlsSurface*>& surfaces,
	int nosurf,
	vector<int> items);

#endif
