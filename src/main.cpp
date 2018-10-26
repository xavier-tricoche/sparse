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
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <string>
#include <math.h>
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
#include <math/fixed_vector.hpp>
#include <math/bezier_spline.hpp>
#include <math/dopri5.hpp>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_multifit.h>
#include <teem/nrrd.h>
#include <teem/ten.h>
#include <teem/seek.h>
#include <teem/air.h>
#include <teem/limn.h>
#include <boost/algorithm/string.hpp>

#include "MyMath.h"
#include "MyTeem.h"
#include "MyGeometry.h"
#include "Sample_point.h"

#include "DiscreteSibson.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Delaunay triangulation of sample points
////////////////////////////////////////////////////////////////////////////////


map<string, string> parameters;
vector<Sample_point>* points = NULL;
vector<Sample_point>* pts = NULL;
NrrdWrapper3D* recons[3];
NrrdWrapper3D* fm[3];
NrrdWrapper3D* fmJ[3][3];
int selected_field = 0;
double min_spc;

////////////////////////////////////////////////////////////////////////////////
// Main entry point
////////////////////////////////////////////////////////////////////////////////

void WriteOutput(int oid, int option)
{
	min_spc = fm[0]->min_spc;
	int dim = 3;
	int dimJ = 9;

	// add samples and create the triangulation
	NrrdWrapper3D* output[3];
	for (int cdim = 0; cdim < dim; cdim++)
	{
		output[cdim] = new NrrdWrapper3D(teem_copy(fm[cdim]->ni));

		double mse = 0.0;
		double psnr = 0.0;
		for (int x = 0; x < fm[cdim]->width(); x+=1)
		{
			for (int y = 0; y < fm[cdim]->height(); y+=1)
			{
				for (int z = 0; z < fm[cdim]->depth(); z+=1)
				{
					int k = fm[cdim]->Coord2Addr(x, y, z);


					// data fields
					double init_value = fm[cdim]->ProbeValueAt(x, y, z);
					double value = recons[cdim]->ProbeValueAt(x, y, z);

					// value to save
					double quan = 0.0;
					quan = value;

					if (myiswn(quan))
						quan = 0.0;

					output[cdim]->Set(x, y, z, quan);
				}
			}
		}
	}

	// join the different fields into a single nrrd
	Nrrd** tmp = new Nrrd*[dim];
	for (int cdim = 0; cdim < dim; cdim++)
	{
		tmp[cdim] = output[cdim]->ni;
	}
	Nrrd* nout = nrrdNew();
	nrrdJoin(nout, tmp, dim, 0, 1);
	nout->axis[0].spacing = 1.0;

	// ready to save the file
	char str[12];
	sprintf(str, "%d", oid);
	char str2[12];
	sprintf(str2, "%d", option);
	string filename(parameters["OUTPUT_SIGNAL"] + string("_") + string(str) + string(str2) + string(".nrrd"));
	if (nrrdSave(filename.c_str(), nout, NULL)) {
		cerr << "readNrrd: " << biffGetDone(NRRD) << std::endl;
		exit(-1);
	}
	for (int cdim = 0; cdim < dim; cdim++)
	{
		delete output[cdim];
	}
	delete[] tmp;
	nrrdNuke(nout);
}

void AddSampleAt(Sample_point& qp, int cdim, int x, int y, int z, double grad_limit)
{
	qp.coordinate = fm[cdim]->Grid2Space(x, y, z);
	qp.value = fm[cdim]->ProbeValueAt(x, y, z);
	qp.gradient[0] = fmJ[cdim][0]->ProbeValueAt(x, y, z);
	qp.gradient[1] = fmJ[cdim][1]->ProbeValueAt(x, y, z);
	qp.gradient[2] = fmJ[cdim][2]->ProbeValueAt(x, y, z);// / 3.0; // only tdelta divide by 3

	// scale gradient (very large gradient is likely error or noise)
	float3 g = make_float3(qp.gradient[0], qp.gradient[1], qp.gradient[2]);
	if (length(g) > grad_limit)
	{
		g = grad_limit * normalize(g);
		qp.gradient[0] = g.x;
		qp.gradient[1] = g.y;
		qp.gradient[2] = g.z;
	}

	// add the point
	//fm[cdim]->ProbeGrid(x, y, z);
	//fmJ[cdim][0]->ProbeGrid(x, y, z);
	//fmJ[cdim][1]->ProbeGrid(x, y, z);
	//Sample_point qp;
	//qp.coordinate = fm[cdim]->Grid2Space(x, y, z);
	//qp.value = fm[cdim]->gctx_sclr[0];
	//qp.gradient[0] = fmJ[cdim][0]->gctx_sclr[0];
	//qp.gradient[1] = fmJ[cdim][1]->gctx_sclr[0];

	//qp.gradient[0] = fm[cdim]->gctx_grad[0];
	//qp.gradient[1] = fm[cdim]->gctx_grad[1];
	//qp.gradient[2] = fm[cdim]->gctx_grad[2];
}

int main( int argc, char *argv[] )
{
	printf("Start the adaptive sampling application.\n");

	// read parameter file
	string pfile;
	if (argc < 2)
	{
		pfile = string("E:\\Chaos2Rost\\VIS13Data\\ABC2\\params_w.txt");
	}
	else
	{
		pfile = string(argv[1]);
	}

	// reading the parameters from a file
	string line;
	ifstream myfile (pfile.c_str());
	if (myfile.is_open())
	{
		while ( myfile.good() )
		{
			getline(myfile,line);
			cout << line << endl;
			vector<string> results;
			boost::split(results, line, boost::is_any_of("="));
			parameters[results[0]] = results[1];
		}
		myfile.close();
	}
	else
	{
		cout << "Unable to open parameters file!\n";
		return 0;
	}

	// read nrrd scalar and setup the gage object
	Nrrd* flowmap = readNrrd(parameters["INPUT_SIGNAL"].c_str());
	Nrrd* flowmapJ = readNrrd(parameters["INPUT_SIGNAL_JACOBIAN"].c_str());

	// Seven
	//size_t minr[2] = {785, 819};
	//size_t maxr[2] = {minr[0] + 200, minr[1] + 200};

	// LINEARSYS
	//size_t minr[2] = {0, 0};
	//size_t maxr[2] = {minr[0] + 255, minr[1] + 255};


	int dim = 3;
	int dimJ = 9;
	int factor = atoi(parameters["START_FACTOR"].c_str());
	pts = new vector<Sample_point>[dim];
	Nrrd* ref;
	for (int i = 0; i < dim; i++)
	{
		// for the flow map
		Nrrd* flowmap_c = teem_slice(flowmap, 0, i);

		//ref = flowmap_c;
		//flowmap_c = teem_crop(flowmap_c, minr, maxr);
		//nrrdNuke(ref);

		fm[i] = new NrrdWrapper3D(flowmap_c);
	}
	for (int i = 0; i < dimJ; i++)
	{
		// for the flow map
		Nrrd* flowmap_c = teem_slice(flowmapJ, 0, i);

		//ref = flowmap_c;
		//flowmap_c = teem_crop(flowmap_c, minr, maxr);
		//nrrdNuke(ref);

		fmJ[i/dim][i%dim] = new NrrdWrapper3D(flowmap_c);
	}

	nrrdNuke(flowmap);
	nrrdNuke(flowmapJ);
	printf("Loading data complete.\n");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// copy from flow map
	for (int cdim = 0; cdim < dim; cdim++)
	{
		recons[cdim] = new NrrdWrapper3D(teem_copy(fm[cdim]->ni));
		memset(recons[cdim]->ni->data, 0, recons[cdim]->Size() * sizeof(float));
	}

	// sample points
	printf("Adding initial samples.\n");
	double grad_limit = atof(parameters["GRAD_LIMIT"].c_str());
	for (int cdim = 0; cdim < dim; cdim++)
	{
		for (int x = 0; x < fm[cdim]->width(); x+=factor)
		{
			for (int y = 0; y < fm[cdim]->height(); y+=factor)
			{
				for (int z = 0; z < fm[cdim]->depth(); z+=factor)
				{
					// add the point
					Sample_point qp;
					AddSampleAt(qp, cdim, x, y, z, grad_limit);

					// add the point
					pts[cdim].push_back(qp);
				}
			}
		}
	}

	printf("Number of samples is %d\n", pts[0].size());

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// data structures
	int size = recons[0]->Size();
	Tree* tree = NULL;
	vector<closest_site> query_cls(size);
	vector<NaturalNeighbors> query_nc(size);
	vector<set<int> > site2discs;
	int nosurf = 0;
	vector<NormalConstrainedSphericalMlsSurface*> surfaces;
	vector<float> errm[3];
	for (int i = 0; i < dim; i++)
		errm[i].resize(size);
	vector<float> errmt(size);

	// loop on user commands
	int seq[5] = {1, 4, 3, 4, 2};
	int seq_idx = 0;
	int miter = atoi(parameters["MAX_ITER"].c_str());
	int iter = -1;
	while(true)
	{
		printf("\n======================================\n");
		printf("Enter operation you want to perform: ");
		printf("(0) Exit. ");
		printf("(1) Regular Discrete Sibson. ");
		printf("(2) Refine by adding new samples. ");
		printf("(3) Modified Discrete Sibson. ");
		printf("(4) Save output.\n");
		int option;
		//cin >> option;


		// automatic number of iterations for a chart
		if (seq_idx == 0)
			iter++;
		if (iter >= miter)
			break;
		option = seq[seq_idx];
		seq_idx = (seq_idx + 1) % 5;
		printf("selected is %d for iteration %d\n", option, iter);


		if (option == 1)
		{
			Timer timer;
			timer.start();

			// compute the natural neighbors
			vector<bool> site_is_disc(pts[0].size());
			FindClosest(recons[0], query_cls, query_nc, pts[0], site_is_disc, tree, nosurf, surfaces, site2discs);
			FindNaturalCoordinates(recons[0], query_cls, query_nc, pts[0], nosurf, surfaces);

			// now run regular sibson
			for (int cdim = 0; cdim < dim; cdim++)
			{
				DiscreteSisbon(fm[cdim], recons[cdim], errm[cdim], pts[cdim], tree, query_cls, query_nc);
			}

			timer.stop();
			cout << "\nTime for regular Sibson's step is " << (0.001 * timer.getElapsedTimeInMilliSec()) << " sec.\n";
		}
		else if (option == 2)
		{
			Timer timer;
			timer.start();

			// max error
			for (int i = 0; i < errmt.size(); i++)
			{
				double maxe = numeric_limits<double>::min();
				double sume = 0.0;
				for (int cdim = 0; cdim < dim; cdim++)
				{
					maxe = max(maxe, double(errm[cdim][i]));
					sume += double(errm[cdim][i]);
				}
				//errmt[i] = maxe;
				errmt[i] = sume;
			}

			// do the refinement
			vector<int> nids;
			Refine(iter, fm[0], nids, errmt, pts[0], tree, query_cls);

			// insert the points
			std::vector<Point_3> points;
			std::vector<int> indices;
			for (int cdim = 0; cdim < dim; cdim++)
			{
				for (int i = 0; i < nids.size(); i++)
				{
					int3 c = fm[cdim]->Addr2Coord(nids[i]);
					int x = c.x;
					int y = c.y;
					int z = c.z;

					// add the point
					Sample_point qp;
					AddSampleAt(qp, cdim, x, y, z, grad_limit);

					// add the point
					pts[cdim].push_back(qp);

					// items to add in tree
					if (cdim == 0)
					{
						// insert in the tree
						points.push_back(Point_3(qp.coordinate.x, qp.coordinate.y, qp.coordinate.z));
						indices.push_back(pts[cdim].size() - 1);
					}
				}
			}
			tree->insert(
				boost::make_zip_iterator(boost::make_tuple( points.begin(),indices.begin() )),
				boost::make_zip_iterator(boost::make_tuple( points.end(),indices.end() ) )
			);
			printf("Total number of samples is %d, a percentage of %2.2lf%%\n", pts[0].size(), (100.0 * pts[0].size()) / recons[0]->Size());

			timer.stop();
			cout << "\nTime for refinement step is " << (0.001 * timer.getElapsedTimeInMilliSec()) << " sec.\n";
		}
		else if (option == 3)
		{
			if (iter == 0)
				continue;

			for (int cdim = 0; cdim < dim; cdim++)
			{
				DiscreteSisbonWithSurfaces(iter, cdim, fm[cdim], recons[cdim], errm[cdim], pts[cdim], query_cls, query_nc);
			}
		}
		else if (option == 4)
		{
			WriteOutput(iter, seq[seq_idx - 2]);
		}
		else if (option == 0)
			break;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	return 0;
}
