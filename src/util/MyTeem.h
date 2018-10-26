#ifndef MYTEEM
#define MYTEEM

#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <omp.h>
#include <teem/nrrd.h>
#include <teem/ten.h>
#include <teem/seek.h>
#include <teem/air.h>
#include <teem/limn.h>

using namespace std;

namespace 
{

	////////////////////////////////////////////////////////////////////////////////
	// Read and write Nrrd files
	////////////////////////////////////////////////////////////////////////////////
	Nrrd *readNrrd(const char* filename)
	{
		int ret;
		Nrrd *nin = nrrdNew();
		if ((ret = nrrdLoad(nin, filename, NULL))) {
			printf("Error %d reading %s\n", ret, filename);
			return NULL;
		}
		return nin;
	}

	void writeNrrd(void* data, const string& filename, int data_type, const vector< size_t >& dims, const vector< double >& spacing)
	{
		Nrrd *nout = nrrdNew();

		if (nrrdWrap_nva(nout, data, data_type, dims.size(), &dims[0])) {
			cerr << "readNrrd: " << biffGetDone(NRRD) << std::endl;
			exit(-1);
		}
		if (spacing.size() == dims.size()) {
			nrrdAxisInfoSet_nva(nout, nrrdAxisInfoSpacing, (const void*)&spacing[0]);
		}
		if (nrrdSave(filename.c_str(), nout, NULL)) {
			cerr << "readNrrd: " << biffGetDone(NRRD) << std::endl;
			exit(-1);
		}
	}

	void writeRawFile4D(float* data,const char *filename, int4 d, float4 s)
	{
		vector<size_t> dims;
		dims.push_back(d.x);
		dims.push_back(d.y);
		dims.push_back(d.z);
		dims.push_back(d.w);

		vector<double> spacing;
		spacing.push_back(s.x);
		spacing.push_back(s.y);
		spacing.push_back(s.z);
		spacing.push_back(s.w);

		string file_string(filename);
		writeNrrd(data, file_string, nrrdTypeFloat, dims, spacing);

		printf("Write '%s'\n", filename);
	}

	void writeRawFile4D(double* data,const char *filename, int4 d, float4 s)
	{
		float* fdata = (float*) malloc(d.x * d.y * d.z * d.w * sizeof(float));

		for (int i = 0; i < d.x * d.y * d.z * d.w; i++)
			fdata[i] = data[i];
		writeRawFile4D(fdata, filename, d, s);

		free(data);
	}

	void writeRawFile3D(float* data,const char *filename, int3 d, double3 s)
	{
		vector<size_t> dims;
		dims.push_back(d.x);
		dims.push_back(d.y);
		dims.push_back(d.z);

		vector<double> spacing;
		spacing.push_back(s.x);
		spacing.push_back(s.y);
		spacing.push_back(s.z);

		string file_string(filename);
		writeNrrd(data, file_string, nrrdTypeFloat, dims, spacing);

		printf("Write '%s'\n", filename);
	}

	void writeRawFile3D(double* data,const char *filename, int3 d, double3 s)
	{
		float* fdata = (float*) malloc(d.x * d.y * d.z * sizeof(float));

		for (int i = 0; i < d.x * d.y * d.z; i++)
			fdata[i] = data[i];
		writeRawFile3D(fdata, filename, d, s);

		free(data);
	}

	void writeRawFile2D(float* data,const char *filename, int2 d, double2 s)
	{
		vector<size_t> dims;
		dims.push_back(d.x);
		dims.push_back(d.y);

		vector<double> spacing;
		spacing.push_back(s.x);
		spacing.push_back(s.y);

		string file_string(filename);
		writeNrrd(data, file_string, nrrdTypeFloat, dims, spacing);

		printf("Write '%s'\n", filename);
	}

	void writeRawFile2D(double* data,const char *filename, int2 d, double2 s)
	{
		float* fdata = (float*) malloc(d.x * d.y * sizeof(float));

		for (int i = 0; i < d.x * d.y; i++)
			fdata[i] = data[i];
		writeRawFile2D(fdata, filename, d, s);

		free(data);
	}

	void writeRawFile1D(float* data,const char *filename, int d, double s)
	{
		vector<size_t> dims;
		dims.push_back(d);

		vector<double> spacing;
		spacing.push_back(s);

		string file_string(filename);
		writeNrrd(data, file_string, nrrdTypeFloat, dims, spacing);

		printf("Write '%s'\n", filename);
	}

	void writeRawFile1D(double* data,const char *filename, int d, double s)
	{
		float* fdata = (float*) malloc(d * sizeof(float));

		for (int i = 0; i < d; i++)
			fdata[i] = data[i];
		writeRawFile1D(fdata, filename, d, s);

		free(data);
	}

	Nrrd* create_nrrd(void* data, int data_type, const vector< size_t >& dims, const vector< double >& spacing)
	{
		Nrrd *nout = nrrdNew();

		if (nrrdWrap_nva(nout, data, data_type, dims.size(), &dims[0])) {
			cerr << "readNrrd: " << biffGetDone(NRRD) << std::endl;
			exit(-1);
		}
		if (spacing.size() == dims.size()) {
			nrrdAxisInfoSet_nva(nout, nrrdAxisInfoSpacing, (const void*)&spacing[0]);
		}

		// set the interpolation to node
		/*int* center = (int*) malloc(dims.size() * sizeof(int));
		for (int i = 0; i < dims.size(); i++)
			center[i] = nrrdCenterNode;
		nrrdAxisInfoSet_nva(nout, nrrdAxisInfoCenter, center);
		free(center);*/

		// set axis min and maximum
		for (int i = 0; i < dims.size(); i++)
		{
			nrrdAxisInfoMinMaxSet(nout, i, nout->axis[i].center);
		}

		return nout;
	}

	Nrrd* createNrrd3D(float* data, int3 d, double3 s)
	{
		vector<size_t> dims;
		dims.push_back(d.x);
		dims.push_back(d.y);
		dims.push_back(d.z);

		vector<double> spacing;
		spacing.push_back(s.x);
		spacing.push_back(s.y);
		spacing.push_back(s.z);

		return create_nrrd(data, nrrdTypeFloat, dims, spacing);
	}

	Nrrd* createNrrd2D(float* data, int2 d, double2 s)
	{
		vector<size_t> dims;
		dims.push_back(d.x);
		dims.push_back(d.y);

		vector<double> spacing;
		spacing.push_back(s.x);
		spacing.push_back(s.y);

		return create_nrrd(data, nrrdTypeFloat, dims, spacing);
	}

	Nrrd* teem_crop(Nrrd *nin, size_t *minr, size_t *maxr)
	{
		Nrrd *nout = nrrdNew();
		int res = nrrdCrop(nout, nin, minr, maxr);
		if (res == 1)
		{
			printf("My Teem: Crop operation failed!\n");
			return NULL;
		}

		return nout;
	}

	Nrrd* teem_read_crop(const char* filename, size_t *minr, size_t *maxr)
	{
		Nrrd *nin = readNrrd(filename);
		return teem_crop(nin, minr, maxr);
	}

	Nrrd* teem_slice(Nrrd *nin, int axis, size_t pos)
	{
		Nrrd *nout = nrrdNew();
		int res = nrrdSlice(nout, nin, axis, pos);
		if (res == 1)
		{
			printf("My Teem: Slice operation failed!\n");
			return NULL;
		}

		return nout;
	}

	Nrrd* teem_read_slice(const char* filename, int axis, size_t pos)
	{
		Nrrd *nin = readNrrd(filename);
		return teem_slice(nin, axis, pos);
	}

	Nrrd* teem_2D_to_3D(Nrrd *nin, double spc)
	{
		Nrrd *nins[2];
		nins[0] = nin;
		nins[1] = nin;
		Nrrd *nout = nrrdNew();
		int res = nrrdJoin(nout, nins, 2, 2, 1);
		nout->axis[2].spacing = spc;

		if (res == 1)
		{
			printf("My Teem: Join operation failed!\n");
			return NULL;
		}

		return nout;
	}

	Nrrd* teem_3D_to_4D(Nrrd *nin, double spc)
	{
		Nrrd *nins[2];
		nins[0] = nin;
		nins[1] = nin;
		Nrrd *nout = nrrdNew();
		int res = nrrdJoin(nout, nins, 2, 3, 1);
		nout->axis[3].spacing = spc;

		if (res == 1)
		{
			printf("My Teem: Join operation failed!\n");
			return NULL;
		}

		return nout;
	}

	Nrrd* teem_2D_to_3D(const char* filename, double spc)
	{
		Nrrd *nin = readNrrd(filename);
		return teem_2D_to_3D(nin, spc);
	}

	void teem_quantize(Nrrd* nin, NrrdRange* range, const char* filename)
	{
		Nrrd *nout = nrrdNew();
		int res = nrrdQuantize(nout, nin, range, 8);
		if (nrrdSave(filename, nout, NULL)) {
			cerr << "readNrrd: " << biffGetDone(NRRD) << std::endl;
			exit(-1);
		}
	}

	Nrrd* teem_copy(Nrrd* nin)
	{
		Nrrd *nout = nrrdNew();
		int res = nrrdCopy(nout, nin);
		if (res == 1)
		{
			printf("My Teem: Slice operation failed!\n");
			return NULL;
		}

		return nout;
	}

	void teem_clear(Nrrd* nin)
	{
		int size = 1;
		for (int i = 0; i < nin->dim; i++)
			size *= nin->axis[i].size;
		memset(nin->data, 0, size * sizeof(float));
	}

	void teem_3D_set(Nrrd* nin, int x, int y, int z, double val)
	{
		((float*) nin->data)[x + nin->axis[0].size * (y + nin->axis[1].size * z)] = val;
	}

	void teem_4D_set(Nrrd* nin, int x, int y, int z, int w, double val)
	{
		((float*) nin->data)[x + nin->axis[0].size * (y + nin->axis[1].size * (z + nin->axis[2].size * w))] = val;
	}

	double teem_3D_get(Nrrd* nin, int x, int y, int z)
	{
		return ((float*) nin->data)[x + nin->axis[0].size * (y + nin->axis[1].size * z)];
	}

	double teem_4D_get(Nrrd* nin, int x, int y, int z, int w)
	{
		return ((float*) nin->data)[x + nin->axis[0].size * (y + nin->axis[1].size * (z + nin->axis[2].size * w))];
	}

	Nrrd* teem_read_as_3D(const char* filename)
	{
		Nrrd* nout = readNrrd(filename);
		if (nout->dim > 3)
		{
			printf("This file has dimensions for than 3!\n");
			return NULL;
		}
		else if (nout->dim == 2)
		{
			Nrrd* ref = nout;
			nout = teem_2D_to_3D(nout, 1.0);
			nrrdNuke(ref);
		}
		else if (nout->dim < 2)
		{
			printf("dimension too small!");
			return NULL;
		}
		return nout;
	}

	Nrrd* teem_read_as_4D(const char* filename)
	{
		Nrrd* nout = readNrrd(filename);
		if (nout->dim > 4)
		{
			printf("This file has dimensions for than 4!\n");
			return NULL;
		}
		else if (nout->dim == 3)
		{
			Nrrd* ref = nout;
			nout = teem_3D_to_4D(nout, 1.0);
			nrrdNuke(ref);
		}
		else if (nout->dim < 3)
		{
			printf("dimension too small!");
			return NULL;
		}
		return nout;
	}

	double teem_mse(Nrrd* n1, Nrrd* n2)
	{
		double sum = 0.0;
		if (n1->dim != n2->dim)
		{
			printf("Error: Different number of dimensions!\n");
			return 0.0;
		}
		int size = 1;
		for (int i = 0; i < n1->dim; i++)
			size *= n1->axis[i].size;

		for (int i = 0; i < size; i++)
		{
			sum += pow(((float*)n1->data)[i] - ((float*)n2->data)[i], 2.0f);
		}
		sum /= double(size);
		
		return sum;
	}

	class NrrdWrapper3D
	{
	public:
		Nrrd* ni;
		Nrrd* ni_grad;
		gagePerVolume *pvl;
		gageContext *gctx;
		const double* gctx_sclr;
		const double* gctx_grad;
		const double* gctx_hess;
		const double* gctx_eval;
		const double* gctx_evec;
		const double* gctx_gmag;
		const double* gctx_k1;
		const double* gctx_k1dir;
		const double* gctx_k2;
		const double* gctx_k2dir;
		double min_spc;

		// if to precompute gradient
		bool precompute_gradient;
		float3* precomputed_gradient;

		void PrecomputeGradient()
		{
			precompute_gradient = true;
			if (precomputed_gradient == NULL)
			{
				precomputed_gradient = (float3*) malloc(Size() * sizeof(float3));
			}

			// compute the gradient
			for (int x = 0; x < width(); x++)
			{
				for (int y = 0; y < height(); y++)
				{
					for (int z = 0; z < depth(); z++)
					{
						ProbeGrid(x, y, z);
						int idx = Coord2Addr(x, y, z);
						precomputed_gradient[idx].x = gctx_grad[0];
						precomputed_gradient[idx].y = gctx_grad[1];
						precomputed_gradient[idx].z = gctx_grad[2];
					}
				}
			}
		}

		NrrdWrapper3D(Nrrd* _ni, bool _precompute_gradient = false, bool grad = true, bool hess = true, bool eign = true, bool k1 = false, bool k2 = false)
		{
			if (!grad)
				hess = false;
			if (!hess)
				eign = false;

			ni = _ni;
			if (ni->dim != 3)
			{
				fprintf(stderr, "This is not a 3D nrrd!\n");
			}
			gctx = gageContextNew();
			double kparm[3]={1.0, 1.0, 0.0}; /* parameters of the cubic kernel */
			if (    gageKernelSet(gctx, gageKernel00, nrrdKernelBCCubic, kparm)
				|| (grad? gageKernelSet(gctx, gageKernel11, nrrdKernelBCCubicD, kparm) : 0)
				|| (hess? gageKernelSet(gctx, gageKernel22, nrrdKernelBCCubicDD, kparm) : 0)
				|| !(pvl = gagePerVolumeNew(gctx, ni, gageKindScl))
				|| gagePerVolumeAttach(gctx, pvl)
				|| gageQueryItemOn(gctx, pvl, gageSclValue)
				|| (grad? gageQueryItemOn(gctx, pvl, gageSclGradVec) : 0)
				|| (hess? gageQueryItemOn(gctx, pvl, gageSclHessian) : 0)
				|| (eign? gageQueryItemOn(gctx, pvl, gageSclHessEval) : 0)
				|| (eign? gageQueryItemOn(gctx, pvl, gageSclHessEvec) : 0)
				|| (grad? gageQueryItemOn(gctx, pvl, gageSclGradMag) : 0)
				|| (k1? gageQueryItemOn(gctx, pvl, gageSclK1) : 0)
				|| (k1? gageQueryItemOn(gctx, pvl, gageSclCurvDir1) : 0)
				|| (k2? gageQueryItemOn(gctx, pvl, gageSclK2) : 0)
				|| (k2? gageQueryItemOn(gctx, pvl, gageSclCurvDir2) : 0)
				|| gageUpdate(gctx)) 
			{
				char *err = " something stupuid in teem";
				fprintf(stderr, "ERROR while setting up Gage:\n%s\n", err);
			}
			gctx_sclr = gageAnswerPointer(gctx, pvl, gageSclValue);
			gctx_grad = gageAnswerPointer(gctx, pvl, gageSclGradVec);
			gctx_hess = gageAnswerPointer(gctx, pvl, gageSclHessian);
			gctx_eval = gageAnswerPointer(gctx, pvl, gageSclHessEval);
			gctx_evec = gageAnswerPointer(gctx, pvl, gageSclHessEvec);
			gctx_gmag = gageAnswerPointer(gctx, pvl, gageSclGradMag);
			gctx_k1 = gageAnswerPointer(gctx, pvl, gageSclK1);
			gctx_k1dir = gageAnswerPointer(gctx, pvl, gageSclCurvDir1);
			gctx_k2 = gageAnswerPointer(gctx, pvl, gageSclK2);
			gctx_k2dir = gageAnswerPointer(gctx, pvl, gageSclCurvDir2);

			min_spc = std::min(ni->axis[0].spacing, std::min(ni->axis[1].spacing, ni->axis[2].spacing));

			precomputed_gradient = NULL;
			precompute_gradient = _precompute_gradient;
			if (precompute_gradient)
			{
				PrecomputeGradient();
			}
		}

		~NrrdWrapper3D()
		{
			nrrdNuke(ni);
			//delete gctx;
			//delete pvl;
			//delete ni;
		}

		float3 Space2Grid(float3 spt)
		{
			return make_float3(spt.x / ni->axis[0].spacing, spt.y / ni->axis[1].spacing, spt.z / ni->axis[2].spacing);
		}
		float3 Space2Grid(double x, double y, double z)
		{
			return Space2Grid(make_float3(x,y,z));
		}
		float3 Grid2Space(float3 gpt)
		{
			return make_float3(gpt.x * ni->axis[0].spacing, gpt.y * ni->axis[1].spacing, gpt.z * ni->axis[2].spacing);
		}
		float3 Grid2Space(double x, double y, double z)
		{
			return Grid2Space(make_float3(x,y,z));
		}
		int Coord2Addr(int x, int y, int z)
		{
			return x + width() * (y + z * height());
		}
		int3 Addr2Coord(int idx)
		{
			int x = idx % width();
			int y = ((idx - x) / width()) % height();
			int z = (((idx - x) / width()) - y) / height();

			return make_int3(x, y, z);
		}

		float3 Addr2Grid(int idx)
		{
			int x = idx % width();
			int y = ((idx - x) / width()) % height();
			int z = (((idx - x) / width()) - y) / height();

			return make_float3(x, y, z);
		}

		float3 Addr2Space(int idx)
		{
			int x = idx % width();
			int y = ((idx - x) / width()) % height();
			int z = (((idx - x) / width()) - y) / height();

			return Grid2Space(double(x), double(y), double(z));
		}


		/* Dimensions info */

		int width()
		{
			return ni->axis[0].size;
		}

		int height()
		{
			return ni->axis[1].size;
		}

		int depth()
		{
			return ni->axis[2].size;
		}

		int Size()
		{
			return width() * height() * depth();
		}

		double MaxX()
		{
			return (width() - 1) * ni->axis[0].spacing;
		}

		double MaxY()
		{
			return (height() - 1) * ni->axis[1].spacing;
		}

		double MaxZ()
		{
			return (depth() - 1) * ni->axis[2].spacing;
		}

		/* Probe using teem functions */

		bool ProbeSpace(float3 spt)
		{
			float3 gpt = Space2Grid(spt);
			return ProbeGrid(gpt);
		}

		bool ProbeSpace(double x, double y, double z)
		{
			return ProbeSpace(make_float3(x,y,z));
		}

		bool ProbeGrid(float3 gpt)
		{
			if (gageProbe(gctx, gpt.x, gpt.y, gpt.z)) 
			{
				//fprintf(stderr, "probe: trouble:\n(%d) %s\n", gctx->errNum, gctx->errStr);
				return true;
			}
			return false;
		}

		bool ProbeGrid(double x, double y, double z)
		{
			return ProbeGrid(make_float3(x,y,z));
		}

		/* Check coordinates */

		bool ValidGridPoint(float3 gpt)
		{
			return !((gpt.x < 0.0) || (gpt.x > width() - 1) ||
					 (gpt.y < 0.0) || (gpt.y > height() - 1) ||
					 (gpt.z < 0.0) || (gpt.z > depth() - 1));
		}

		bool ValidGridPoint(int x, int y, int z)
		{
			return !((x < 0) || (x > width() - 1) ||
					 (y < 0) || (y > height() - 1) ||
					 (z < 0) || (z > depth() - 1));
		}

		bool BoundaryGridPoint(float3 gpt)
		{
			return ((gpt.x < 1.0) || (gpt.x > width() - 2) ||
					(gpt.y < 1.0) || (gpt.y > height() - 2) ||
					(gpt.z < 1.0) || (gpt.z > depth() - 2));
		}

		bool BoundaryGridPoint(int x, int y, int z)
		{
			return ((x < 1) || (x > width() - 2) ||
					(y < 1) || (y > height() - 2) ||
					(z < 1) || (z > depth() - 2));
		}

		/* Probe without teem */

		double ProbeValueGrid(float3 gpt, bool& failed)
		{
			if (!ValidGridPoint(gpt))
			{
				failed = true;
				return 0.0;
			}

			int3 l = make_int3(floor(gpt.x), floor(gpt.y), floor(gpt.z));
			int3 u = make_int3(ceil(gpt.x), ceil(gpt.y), ceil(gpt.z));
			double x = gpt.x - l.x;
			double y = gpt.y - l.y;
			double z = gpt.z - l.z;
	
			float* data = (float*) ni->data;

			float V000 = (data[Coord2Addr(l.x, l.y, l.z)]);
			float V001 = (data[Coord2Addr(l.x, l.y, u.z)]);
			float V010 = (data[Coord2Addr(l.x, u.y, l.z)]);
			float V011 = (data[Coord2Addr(l.x, u.y, u.z)]);
			float V100 = (data[Coord2Addr(u.x, l.y, l.z)]);
			float V101 = (data[Coord2Addr(u.x, l.y, u.z)]);
			float V110 = (data[Coord2Addr(u.x, u.y, l.z)]);
			float V111 = (data[Coord2Addr(u.x, u.y, u.z)]);
	
			float V =   V000 * (1 - x) * (1 - y) * (1 - z) +
						V100 * x * (1 - y) * (1 - z) +
						V010 * (1 - x) * y * (1 - z) +
						V001 * (1 - x) * (1 - y) * z +
						V101 * x * (1 - y) * z +
						V011 * (1 - x) * y * z +
						V110 * x * y * (1 - z) +
						V111 * x * y * z; 
	
			return V;
		}

		double ProbeValueSpace(float3 spt, bool& failed)
		{
			float3 gpt = Space2Grid(spt);
			return ProbeValueGrid(gpt, failed);
		}

		float3 ProbeGradGrid(float3 gpt, bool& failed)
		{
			if (!ValidGridPoint(gpt))
			{
				failed = true;
				return make_float3(0.0);
			}

			int3 l = make_int3(floor(gpt.x), floor(gpt.y), floor(gpt.z));
			int3 u = make_int3(ceil(gpt.x), ceil(gpt.y), ceil(gpt.z));
			double x = gpt.x - l.x;
			double y = gpt.y - l.y;
			double z = gpt.z - l.z;
	
			float3* data = precomputed_gradient;

			float3 V000 = (data[Coord2Addr(l.x, l.y, l.z)]);
			float3 V001 = (data[Coord2Addr(l.x, l.y, u.z)]);
			float3 V010 = (data[Coord2Addr(l.x, u.y, l.z)]);
			float3 V011 = (data[Coord2Addr(l.x, u.y, u.z)]);
			float3 V100 = (data[Coord2Addr(u.x, l.y, l.z)]);
			float3 V101 = (data[Coord2Addr(u.x, l.y, u.z)]);
			float3 V110 = (data[Coord2Addr(u.x, u.y, l.z)]);
			float3 V111 = (data[Coord2Addr(u.x, u.y, u.z)]);
	
			float3 V =  V000 * (1 - x) * (1 - y) * (1 - z) +
						V100 * x * (1 - y) * (1 - z) +
						V010 * (1 - x) * y * (1 - z) +
						V001 * (1 - x) * (1 - y) * z +
						V101 * x * (1 - y) * z +
						V011 * (1 - x) * y * z +
						V110 * x * y * (1 - z) +
						V111 * x * y * z; 
	
			return V;
		}

		float3 ProbeGradSpace(float3 spt, bool& failed)
		{
			float3 gpt = Space2Grid(spt);
			return ProbeGradGrid(gpt, failed);
		}

		double ProbeValueAt(int x, int y, int z)
		{
			float* data = (float*) ni->data;
			return data[Coord2Addr(x, y, z)];
		}

		float3 ProbeGradAt(int x, int y, int z)
		{
			if (precompute_gradient)
			{
				float3* data = precomputed_gradient;
				return data[Coord2Addr(x, y, z)];
			}
			else
			{
				if ((!ValidGridPoint(x-1,y-1,z-1)) || (!ValidGridPoint(x+1,y+1,z+1)))
					return make_float3(0.0);
				float3 g;
				g.x = 0.5 * (ProbeValueAt(x+1,y,z) - ProbeValueAt(x-1,y,z)) / ni->axis[0].spacing;
				g.y = 0.5 * (ProbeValueAt(x,y+1,z) - ProbeValueAt(x,y-1,z)) / ni->axis[1].spacing;
				g.z = 0.5 * (ProbeValueAt(x,y,z+1) - ProbeValueAt(x,y,z-1)) / ni->axis[2].spacing;
				return g;
			}
		}

		/* Write info */

		void Set(int x, int y, int z, double val)
		{
			((float*)ni->data)[x + width() * (y + z * height())] = val;
		}

		void Clear()
		{
			for (int i = 0; i < width() * height() * depth(); i++)
				((float*)ni->data)[i] = 0.0;
		}

		void Write(const char* filename)
		{
			if (nrrdSave(filename, ni, NULL)) {
				cerr << "readNrrd: " << biffGetDone(NRRD) << std::endl;
				exit(-1);
			}

			printf("Write '%s'\n", filename); fflush(stdout);
		}
	};


	double teem_mse(NrrdWrapper3D* n1, NrrdWrapper3D* n2)
	{
		double sum = 0.0;
		if (n1->Size() != n2->Size())
		{
			printf("Error: Different sizes!\n");
			return 0.0;
		}

		for (int i = 0; i < n1->Size(); i++)
		{
			sum += pow(((float*)n1->ni->data)[i] - ((float*)n2->ni->data)[i], 2.0f);
		}
		sum /= double(n1->Size());
		
		return sum;
	}
}

#endif