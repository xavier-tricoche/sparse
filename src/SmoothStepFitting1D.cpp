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
#include "SmoothStepFitting1D.h"

struct data {
	vector<float4> xy;
	double lx;
	double hx;
	double ly;
	double hy;
	double qx;
};

struct cmp_data_points
{
    inline bool operator() (const float4& struct1, const float4& struct2)
    {
        return (struct1.x < struct2.x);
    }
};

int	smoothstep_f (const gsl_vector * x, void *data, gsl_vector * f)
{
	vector<float4> xy = ((struct data *)data)->xy;
	double lx = ((struct data *)data)->lx;
	double hx = ((struct data *)data)->hx;
	double ly = ((struct data *)data)->ly;
	double hy = ((struct data *)data)->hy;
	double qx = ((struct data *)data)->qx;
	int n = xy.size();
	double a = gsl_vector_get (x, 0);
	double b = gsl_vector_get (x, 1);

	double c = 0.0;

	size_t i;

	for (i = 0; i < n; i++)
	{
		double F = 0.5 * (ly + hy) + 0.5 * (hy - ly) * (tanh(8.0 * a * (xy[i].x - b) / (hx - lx)) + c * (xy[i].x - b));
		gsl_vector_set (f, i, (F - xy[i].y) 
								/ xy[i].z);
	}

	return GSL_SUCCESS;
}

int	smoothstep_df (const gsl_vector * x, void *data, gsl_matrix * J)
{
	vector<float4> xy = ((struct data *)data)->xy;
	double lx = ((struct data *)data)->lx;
	double hx = ((struct data *)data)->hx;
	double ly = ((struct data *)data)->ly;
	double hy = ((struct data *)data)->hy;
	double qx = ((struct data *)data)->qx;
	int n = xy.size();
	double a = gsl_vector_get (x, 0);
	double b = gsl_vector_get (x, 1);

	double c = 0.0;

	size_t i;

	for (i = 0; i < n; i++)
	{
		double qx = xy[i].x;
		gsl_matrix_set (J, i, 0, (4.0 * (hy - ly) * (qx - b) * sech2(8.0 * a * (qx - b) / (hx - lx)) / (hx - lx))
									/ xy[i].z);
		gsl_matrix_set (J, i, 1, (0.5 * (hy - ly) * (-8.0 * a * sech2(8.0 * a * (qx - b) / (hx - lx)) / (hx - lx) - c))
									/ xy[i].z);
	}

	return GSL_SUCCESS;
}

int	smoothstep_fdf (const gsl_vector * x, void *data,	gsl_vector * f, gsl_matrix * J)
{
	smoothstep_f (x, data, f);
	smoothstep_df (x, data, J);

	return GSL_SUCCESS;
}

void print_state1D (size_t iter, gsl_multifit_fdfsolver * s);

double SmoothStepFitting1D(vector<float4>& xy, double lx, double hx, double ly, double hy, double qx, int& status, double ia, vector<double>& info)
{
	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;
	unsigned int i, iter = 0;
	const size_t n = xy.size();
	const size_t p = 2;

	gsl_matrix *covar = gsl_matrix_alloc (p, p);
        gsl_matrix *J = gsl_matrix_alloc(n, p);

	struct data d_ = { xy, lx, hx, ly, hy, qx};
	gsl_multifit_function_fdf f;
	double x_init[2] = { ia, 0.0};
	gsl_vector_view x = gsl_vector_view_array (x_init, p);
	const gsl_rng_type * type;
	gsl_rng * r;

	gsl_rng_env_setup();

	type = gsl_rng_default;
	r = gsl_rng_alloc (type);

	f.f = &smoothstep_f;
	f.df = &smoothstep_df;
	f.fdf = &smoothstep_fdf;
	f.n = n;
	f.p = p;
	f.params = &d_;

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc (T, n, p);
	gsl_multifit_fdfsolver_set (s, &f, &x.vector);

	//print_state1D (iter, s);

	do
	{
		iter++;
		status = gsl_multifit_fdfsolver_iterate (s);

		//printf ("status = %s\n", gsl_strerror (status));
		//print_state1D (iter, s);

		if (status)
			break;

		status = gsl_multifit_test_delta (s->dx, s->x, 1e-8, 1e-8);
	}
	while (status == GSL_CONTINUE && iter < 500);

        gsl_multifit_fdfsolver_jac(s, J);
	gsl_multifit_covar (J, 0.0, covar);

#define FIT(i) gsl_vector_get(s->x, i)
#define ERR(i) sqrt(gsl_matrix_get(covar,i,i))

	{ 
		double chi = gsl_blas_dnrm2(s->f);
		double dof = n - p;
		double c = GSL_MAX_DBL(1, chi / sqrt(dof)); 

		//printf("chisq/dof = %g\n",  pow(chi, 2.0) / dof);
		//printf ("a      = %.5f +/- %.5f\n", FIT(0), c*ERR(0));
		//printf ("b      = %.5f +/- %.5f\n", FIT(1), c*ERR(1));
	}

	status = gsl_multifit_test_delta (s->dx, s->x, (hy - ly) * 1e-4, (hy - ly) * 1e-4);
	//printf ("status = %s\n", gsl_strerror (status));
	//printf("%e %e %e %e\n", gsl_vector_get (s->x, 0), gsl_vector_get (s->x, 1), gsl_vector_get (s->dx, 0), gsl_vector_get (s->dx, 1));

	// get all the parameters
	double p_a = gsl_vector_get (s->x, 0);
	double p_b = gsl_vector_get (s->x, 1);
	for (int i = 0; i < n; i++)
	{
		//printf("%lf\t%lf\t%lf\t%lf \n", xy[i].x, xy[i].y, xy[i].z, ly + 0.5 * (hy - ly) * (1.0 + tanh(8.0 * p_a * (xy[i].x - p_b) / (hx - lx))));
	}

	// return value based on parameters
	info.push_back(tanh(8.0 * p_a * (qx - p_b) / (hx - lx)));
	info.push_back(gsl_blas_dnrm2 (s->f));
	info.push_back(lx);
	info.push_back(hx);
	info.push_back(ly);
	info.push_back(hy);
	info.push_back(p_a);
	info.push_back(p_b);

	// free memory
	gsl_multifit_fdfsolver_free (s);
	gsl_matrix_free (covar);
	gsl_rng_free (r);

	// return value
	return ly + 0.5 * (hy - ly) * (1.0 + tanh(8.0 * p_a * (qx - p_b) / (hx - lx)));
}

void print_state1D (size_t iter, gsl_multifit_fdfsolver * s)
{
	printf ("iter: %3u a = % 15.8f b = % 15.8f "
		"|f(x)| = %g\n",
		iter,
		gsl_vector_get (s->x, 0),
		gsl_vector_get (s->x, 1),
		gsl_blas_dnrm2 (s->f));
}


#include "ASPSS/ExpeLinearAlgebra.h"
double SphereFitting(vector<float3>& xy, double lx, double hx, double ly, double hy, double qx, int& status)
{
	const uint N=4;

    // variable allocation to save memory allocation/deallocation
    // as well to keep some results between the fit and the compmuatation of the gradient
    double mVecU[N];
    double mCovMat[N][N];
    double mLambda;
    
    // used by for the gradient
    double mMatdA[N][N];
    gsl_matrix* mSvdMatV;
    gsl_vector* mSvdVecS;
    gsl_vector* mSvdVecWork;
    
    bool mNormalization;

	double u[5];


	// the constraint matrix
    static double invC[] = {
        0, 0, 0, -0.5,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -0.5, 0, 0, 0};
    
	uint nofSamples = xy.size();
    if (nofSamples<5)
    {
		status = -2;
        return 0.0;
    }
    
    gsl_matrix_view invC_view = gsl_matrix_view_array (invC, N, N);
    gsl_matrix_view matrixA_view = gsl_matrix_view_array ((double*)mCovMat, N, N);
    
    // directly fill the covariance matrix A'A
    for(uint i=0 ; i<N ; ++i)
        for(uint j=0 ; j<N ; ++j)
            mCovMat[i][j] = 0.;
    
    double vec[N];
    for (uint k=0; k<nofSamples; k++)
    {
        double w = xy[k].z;
        
        vec[0] = 1.;
		vec[1] = xy[k].x;
        vec[2] = xy[k].y;
        vec[3] = xy[k].x * xy[k].x + xy[k].y * xy[k].y;
        
        for(uint i=0 ; i<N ; ++i)
            for(uint j=0 ; j<N ; ++j)
                mCovMat[i][j] += vec[i]*vec[j]*w;
    }

    // Compute the determinant of the covariance matrix
    #ifndef EXPE_HAVE_OCTAVE
    #pragma message ( "#warning The octave library is not enabled => the result of the sphere fitting may be unstable for ill-conditionned covariance matrix." )
    #endif
	double det = Expe::ela::det(&matrixA_view.matrix);
	if (det < 1e-15)
		det = 0.0;
    if (det==0)
    {
        // this means either an exact fit is possible or that the problem is underconstrained
        // then algebraic coeffs == cofactors of A'A
        double cofactors[N];
        double norm = 0.;
        double sign = (N%2) ? -1. : 1.;
        gsl_matrix* subMat = gsl_matrix_alloc(N-1,N-1);
        for(uint k=0 ; k<N ; ++k)
        {
            // fill the sub matrix
            uint j1 = 0;
            for(uint j=0 ; j<N ; ++j)
            {
                if(j!=k)
                {
                    for(uint i=0 ; i<N-1 ; ++i)
                    {
                        gsl_matrix_set(subMat, i, j1, gsl_matrix_get(&matrixA_view.matrix, i, j));
                    }
                    j1++;
                }
            }
            cofactors[k] = sign*Expe::ela::det(subMat);
            norm += cofactors[k]*cofactors[k];
//             det += cofactors[k] * gsl_matrix_get(&matrixA_view.matrix, N-1, k);
            sign = -sign;
        }
        gsl_matrix_free(subMat);
        
        norm = sqrt(norm);
        if (norm<1e-4)
		{
			status = -2;
            return 0.0;
		}
        
        // copy the result
        for(uint k=0 ; k<N ; ++k)
        {
            u[k] = cofactors[k];
        }
        
		u[4] = u[3];
		u[3] = 0.0;
    }
    else
    {
        // use the cholesky decomposition to solve the generalized eigen problem
        // as a simple eigen problem
        gsl_matrix *eigen_vectors = gsl_matrix_alloc(N,N);
        gsl_vector *eigen_values = gsl_vector_alloc(N);
        if(!Expe::ela::eigen_generalized_sym(&invC_view.matrix, &matrixA_view.matrix, eigen_values, eigen_vectors, Expe::ela::GEF_ABx_lx))
        {
            std::cerr << "ela::eigen_generalized_sym failed:\n"; 
            Expe::ela::print_matrix(&matrixA_view.matrix);
            std::cerr << "\tnofSamples = " << nofSamples << "\n";
            std::cerr << "\tdet(AA) = " << det << "\n\n";
			status = -2;
            return 0.0;
        }

        // search the lowest positive eigen value
        int lowestId = -1;
        for (uint i=0 ; i<N ; ++i)
        {
            double ev = gsl_vector_get(eigen_values, i);
            if(fabs(ev)<1e-9)
                ev = 0;
            if ((ev>0) && (lowestId==-1 || ev<gsl_vector_get(eigen_values, lowestId)))
            {
                lowestId = i;
            }
        }
        
        // keep the value of the eigen value for the gradient calculation
        mLambda = gsl_vector_get(eigen_values, lowestId);
        
        for (uint i=0 ; i<N ; ++i)
            mVecU[i] = gsl_matrix_get(eigen_vectors, i, lowestId);
        
        double norm = mVecU[1]*mVecU[1]+mVecU[2]*mVecU[2] - 4.*mVecU[0]*mVecU[N-1];
//         std::cout << "norm = " << norm << "\n";
        norm = 1./sqrtf(norm);
        for (uint i=0 ; i<N ; ++i)
            mVecU[i] *= norm;

        gsl_matrix_free(eigen_vectors);
        gsl_vector_free(eigen_values);
        
        // copy
        for (uint i=0 ; i<N ; ++i)
            u[i] = mVecU[i];

		u[4] = u[3];
		u[3] = 0.0;
    }
    
	// resulting sphere or plane
	enum AlgebraicSphereState {ASS_UNDETERMINED=0,ASS_PLANE=1,ASS_SPHERE=2};
	AlgebraicSphereState mState;
	float3 s_center; double s_radius;
	float3 p_normal; double p_offset;
	float3 u13 = make_float3(u[1], u[2], u[3]);
	double qy;
    if (abs(u[4])>1e-9)
    {
        mState = ASS_SPHERE;
        double b = 1./u[4];
        s_center = -0.5*b*u13;
        s_radius = sqrt(dot(s_center,s_center) - b*u[0]);

		qy = sqrt(s_radius * s_radius - (qx - s_center.x) * (qx - s_center.x)) + s_center.y;
    }
    else if (u[4]==0.0)
    {
        mState = ASS_PLANE;
        double s = 1.0 / length(u13);
        p_normal = normalize(u13*s);
        p_offset = u[0]*s;

		double slope = -sin(p_normal.y) / cos(p_normal.x);
		qy = p_offset + slope * qx;
    }
    else
    {
        mState = ASS_UNDETERMINED;

		status = -2;
        return 0.0;
    }

	// now do the projection
	double value = 0.0;

	status = GSL_SUCCESS;
    return qy;
}

#include <interpolation.h>
//#include <samer_interpolation.h>
using namespace alglib;
using namespace alglib_impl;
double SplineFitting(vector<float4>& xy, double lx, double hx, double ly, double hy, double qx, int& status)
{
	alglib::ae_int_t m = max(4, xy.size());

	// compute second derivative
	sort(xy.begin(), xy.end(), cmp_data_points());
	for (int i = 0; i < xy.size() - 1; i++)
	{
		double rg = xy[i].w;
		double eg = (xy[i + 1].y - xy[i].y) / (xy[i + 1].x - xy[i].x);
		double err = abs(eg - rg) / abs(rg);
		double xd = abs(xy[i + 1].x - xy[i].x) / (hx - lx);
		if ((xd < 0.025) && (err > 2.0) && (eg * rg < 0.0))
		{
			if (xy[i].z < xy[i + 1].z)
			{
				xy.erase(xy.begin() + i + 1);
			}
			else
			{
				xy.erase(xy.begin() + i);
			}
			i--;
		}
	}

	// fill data arrays
	real_1d_array x;
	x.setlength(xy.size());
	real_1d_array y;
	y.setlength(xy.size());
	real_1d_array g;
	g.setlength(xy.size());
	real_1d_array w;
	w.setlength(xy.size());
	for (int i = 0; i < xy.size(); i++)
	{
		x[i] = xy[i].x;
		y[i] = xy[i].y;
		g[i] = xy[i].w;
		w[i] = 1.0 / xy[i].z;
	}

	// set parameters and call fitting
	alglib::ae_int_t info;
	alglib::spline1dinterpolant s;
	alglib::spline1dfitreport rep;
	double rho = 0.0;
	alglib::ae_int_t n = xy.size() / 3;
	alglib::spline1dfitpenalizedw(x, y, /*g, <-- ??*/ w, n, m, rho, info, s, rep);

	if (info <= 0)
	{
		// error occured: change status to something other than zero
		status = -2;
		return 0.0;
	}

	// print debug info
	//for (int i = 0; i < xy.size(); i++)
	//{
	//	printf("%lf %lf %lf %lf %lf\n", x[i], y[i], alglib::spline1dcalc(s, x[i]), g[i], w[i]);
	//}
	//printf("\n");
	//printf("minx %lf maxx %lf miny %lf maxy %lf\n", lx, hx, ly, hy);
	//lx = min(lx, qx);
	//hx = max(hx, qx);
	//for ( double px = lx; px < hx; px+=0.1)
	//{
	//	printf("%lf %lf\n", px, alglib::spline1dcalc(s, px));
	//}
	//printf("\n");

	// return the value
	double value = alglib::spline1dcalc(s, qx);
	return value;
}

double QuinticSplineFitting(vector<float4>& xy, double lx, double hx, double ly, double hy, double qx, int& status)
{
	sort(xy.begin(), xy.end(), cmp_data_points());

	int i, n, m;
    double xi, yi, ei, chisq;
    gsl_matrix *X, *cov;
    gsl_vector *y, *w, *c;
     
	n = 2 * xy.size() + 4; 
    m = 12;

    X = gsl_matrix_alloc (n, m);
    y = gsl_vector_alloc (n);
    w = gsl_vector_alloc (n);
     
    c = gsl_vector_alloc (m);
    cov = gsl_matrix_alloc (m, m);
     
	vector<double> r;
	r.push_back(lx);
	int k = 0;
	for (i = 0; i < xy.size(); i++)
	{
		int cnt = 0;
		
		if ((xy[i].x < 0.0) || ((i > 0) && (xy[i - 1].x < 0.0)))
		{
			gsl_matrix_set (X, k, 0, xy[i].x * xy[i].x * xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 1, xy[i].x * xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 2, xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 3, xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 4, xy[i].x );
			gsl_matrix_set (X, k, 5, 1.0 );
			gsl_matrix_set (X, k, 6, 0.0);
			gsl_matrix_set (X, k, 7, 0.0);
			gsl_matrix_set (X, k, 8, 0.0);
			gsl_matrix_set (X, k, 9, 0.0);
			gsl_matrix_set (X, k, 10, 0.0);
			gsl_matrix_set (X, k, 11, 0.0);
			gsl_vector_set (y, k, xy[i].y);
			gsl_vector_set (w, k, 1.0 / xy[i].z);
			k++;

			gsl_matrix_set (X, k, 0, 5.0 * xy[i].x * xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 1, 4.0 * xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 2, 3.0 * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 3, 2.0 * xy[i].x );
			gsl_matrix_set (X, k, 4, 1.0 );
			gsl_matrix_set (X, k, 5, 0.0);
			gsl_matrix_set (X, k, 6, 0.0);
			gsl_matrix_set (X, k, 7, 0.0);
			gsl_matrix_set (X, k, 8, 0.0);
			gsl_matrix_set (X, k, 9, 0.0);
			gsl_matrix_set (X, k, 10, 0.0);
			gsl_matrix_set (X, k, 11, 0.0);
			gsl_vector_set (y, k, xy[i].w);
			gsl_vector_set (w, k, 0.1 / xy[i].z);
			k++;

			cnt++;
		}

		if ((xy[i].x >= 0.0) || ((i < xy.size() - 1) && (xy[i + 1].x >= 0.0)))
		{
			gsl_matrix_set (X, k, 6, xy[i].x * xy[i].x * xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 7, xy[i].x * xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 8, xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 9, xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 10, xy[i].x );
			gsl_matrix_set (X, k, 11, 1.0 );
			gsl_matrix_set (X, k, 0, 0.0);
			gsl_matrix_set (X, k, 1, 0.0);
			gsl_matrix_set (X, k, 2, 0.0);
			gsl_matrix_set (X, k, 3, 0.0);
			gsl_matrix_set (X, k, 4, 0.0);
			gsl_matrix_set (X, k, 5, 0.0);
			gsl_vector_set (y, k, xy[i].y);
			gsl_vector_set (w, k, 1.0 / xy[i].z);
			k++;

			gsl_matrix_set (X, k, 6, 5.0 * xy[i].x * xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 7, 4.0 * xy[i].x * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 8, 3.0 * xy[i].x * xy[i].x );
			gsl_matrix_set (X, k, 9, 2.0 * xy[i].x );
			gsl_matrix_set (X, k, 10, 1.0 );
			gsl_matrix_set (X, k, 11, 0.0);
			gsl_matrix_set (X, k, 0, 0.0);
			gsl_matrix_set (X, k, 1, 0.0);
			gsl_matrix_set (X, k, 2, 0.0);
			gsl_matrix_set (X, k, 3, 0.0);
			gsl_matrix_set (X, k, 4, 0.0);
			gsl_matrix_set (X, k, 5, 0.0);
			gsl_vector_set (y, k, xy[i].w);
			gsl_vector_set (w, k, 0.1 / xy[i].z);
			k++;

			cnt++;
		}

		if (cnt == 2)
		{
			r.push_back(xy[i].x);
		}
	}
	r.push_back(hx);
     
	{
		gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (n, m);
		gsl_multifit_wlinear (X, w, y, c, cov, &chisq, work);
		gsl_multifit_linear_free (work);
	}
     


	double value, v1, v2;
	double f = 0.0;

	// print debug info
	for (int i = 0; i < xy.size(); i++)
	{
		double px = xy[i].x;

		if (px <= r[1])
			f = 1.0;
		else if ((px > r[1]) && (px < r[2]))
			f = (r[2] - px) / (r[2] - r[1]);
		else
			f = 0.0;
		v1 = gsl_vector_get(c, 0) * pow(px, 5.0) + gsl_vector_get(c, 1) * pow(px, 4.0) + gsl_vector_get(c, 2) * pow(px, 3.0) + gsl_vector_get(c, 3) * pow(px, 2.0) + gsl_vector_get(c, 4) * px + gsl_vector_get(c, 5);
		v2 = gsl_vector_get(c, 6) * pow(px, 5.0) + gsl_vector_get(c, 7) * pow(px, 4.0) + gsl_vector_get(c, 8) * pow(px, 3.0) + gsl_vector_get(c, 9) * pow(px, 2.0) + gsl_vector_get(c, 10) * px + gsl_vector_get(c, 11);
		double F = f * v1 + (1.0 - f) * v2;

		printf("%lf %lf %lf\n", xy[i].x, xy[i].y, F);
	}
	printf("\n");
	for ( double px = lx; px < hx; px+=0.1)
	{
		if (px <= r[1])
			f = 1.0;
		else if ((px > r[1]) && (px < r[2]))
			f = (r[2] - px) / (r[2] - r[1]);
		else
			f = 0.0;
		v1 = gsl_vector_get(c, 0) * pow(px, 5.0) + gsl_vector_get(c, 1) * pow(px, 4.0) + gsl_vector_get(c, 2) * pow(px, 3.0) + gsl_vector_get(c, 3) * pow(px, 2.0) + gsl_vector_get(c, 4) * px + gsl_vector_get(c, 5);
		v2 = gsl_vector_get(c, 6) * pow(px, 5.0) + gsl_vector_get(c, 7) * pow(px, 4.0) + gsl_vector_get(c, 8) * pow(px, 3.0) + gsl_vector_get(c, 9) * pow(px, 2.0) + gsl_vector_get(c, 10) * px + gsl_vector_get(c, 11);
		double F = f * v1 + (1.0 - f) * v2;

		printf("%lf %lf\n", px, F);
	}
	printf("\n");





	// compute the value
	if (qx <= r[1])
		f = 1.0;
	else if ((qx > r[1]) && (qx < r[2]))
		f = (r[2] - qx) / (r[2] - r[1]);
	else
		f = 0.0;
	v1 = gsl_vector_get(c, 0) * pow(qx, 5.0) + gsl_vector_get(c, 1) * pow(qx, 4.0) + gsl_vector_get(c, 2) * pow(qx, 3.0) + gsl_vector_get(c, 3) * pow(qx, 2.0) + gsl_vector_get(c, 4) * qx + gsl_vector_get(c, 5);
	v2 = gsl_vector_get(c, 6) * pow(qx, 5.0) + gsl_vector_get(c, 7) * pow(qx, 4.0) + gsl_vector_get(c, 8) * pow(qx, 3.0) + gsl_vector_get(c, 9) * pow(qx, 2.0) + gsl_vector_get(c, 10) * qx + gsl_vector_get(c, 11);
	value = f * v1 + (1.0 - f) * v2;
     
	// free resources
	gsl_matrix_free (X);
	gsl_vector_free (y);
	gsl_vector_free (w);
	gsl_vector_free (c);
	gsl_matrix_free (cov);

	return value;
}


#include "ASPSS/ExpeLinearAlgebra.h"
#include "ASPSS/ExpeMath.h"
double CircleFitting(vector<float4>& xy, double lx, double hx, double ly, double hy, double qx, int& status)
{
	// do scaling to avoid weird matrix approximation issues
	for (int i = 0; i < xy.size(); i++)
	{
		xy[i].x = 10 * (xy[i].x - lx) / (hx - lx);
		xy[i].y = 10 * (xy[i].y - ly) / (hy - ly);
	}
	qx = 10 * (qx - lx) / (hx - lx);
	double oly = ly;
	double ohy = hy;
	lx = 0.0;
	hx = 10.0;
	ly = 0.0;
	hy = 10.0;


	// now do the circle fitting
	static const uint N=4;
	double mVecU[N];
    double mCovMat[N][N];
    double mLambda;
	double u[5];

	// the constraint matrix
    static double invC[] = {
        0, 0, 0, -0.5,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -0.5, 0, 0, 0};
    
	uint nofSamples = xy.size();
    if (nofSamples<5)
    {
		// error
		status = 1;
        return 0.0;
    }
    
    gsl_matrix_view invC_view = gsl_matrix_view_array (invC, N, N);
    gsl_matrix_view matrixA_view = gsl_matrix_view_array ((double*)mCovMat, N, N);
    
    // directly fill the covariance matrix A'A
    for(uint i=0 ; i<N ; ++i)
        for(uint j=0 ; j<N ; ++j)
            mCovMat[i][j] = 0.;
    
    double vec[N];
    for (uint k=0; k<nofSamples; k++)
    {
        float3 pt = make_float3(xy[k].x, xy[k].y, 0.0);
		double w = 1.0 / (xy[k].z);
        
        vec[0] = 1.;
        vec[1] = pt.x;
        vec[2] = pt.y;
        //vec[3] = pt.z;
        vec[3] = dot(pt,pt);
        
        for(uint i=0 ; i<N ; ++i)
            for(uint j=0 ; j<N ; ++j)
                mCovMat[i][j] += vec[i]*vec[j]*w;
    }

    // Compute the determinant of the covariance matrix
    #ifndef EXPE_HAVE_OCTAVE
    #pragma message ( "#warning The octave library is not enabled => the result of the sphere fitting may be unstable for ill-conditionned covariance matrix." )
    #endif
	double det = Expe::ela::det(&matrixA_view.matrix);
	if (det < 1e-15)
		det = 0.0;
    if (det==0)
    {
        // this means either an exact fit is possible or that the problem is underconstrained
        // then algebraic coeffs == cofactors of A'A
        double cofactors[N];
        double norm = 0.;
        double sign = (N%2) ? -1. : 1.;
        gsl_matrix* subMat = gsl_matrix_alloc(N-1,N-1);
        for(uint k=0 ; k<N ; ++k)
        {
            // fill the sub matrix
            uint j1 = 0;
            for(uint j=0 ; j<N ; ++j)
            {
                if(j!=k)
                {
                    for(uint i=0 ; i<N-1 ; ++i)
                    {
                        gsl_matrix_set(subMat, i, j1, gsl_matrix_get(&matrixA_view.matrix, i, j));
                    }
                    j1++;
                }
            }
            cofactors[k] = sign*Expe::ela::det(subMat);
            norm += cofactors[k]*cofactors[k];
//             det += cofactors[k] * gsl_matrix_get(&matrixA_view.matrix, N-1, k);
            sign = -sign;
        }
        gsl_matrix_free(subMat);
        
		norm = Expe::Math::Sqrt(norm);
        if (norm<1e-4)
		{
            // error
			status = 1;
			return 0.0;
		}
        
        // copy the result
        for(uint k=0 ; k<N ; ++k)
        {
            u[k] = cofactors[k];
        }
        
		u[4] = u[3];
		u[3] = 0.0;
    }
    else
    {
        // use the cholesky decomposition to solve the generalized eigen problem
        // as a simple eigen problem
        gsl_matrix *eigen_vectors = gsl_matrix_alloc(N,N);
        gsl_vector *eigen_values = gsl_vector_alloc(N);
		if(!Expe::ela::eigen_generalized_sym(&invC_view.matrix, &matrixA_view.matrix, eigen_values, eigen_vectors, Expe::ela::GEF_ABx_lx))
        {
            std::cerr << "ela::eigen_generalized_sym failed:\n"; 
            Expe::ela::print_matrix(&matrixA_view.matrix);
            std::cerr << "\tnofSamples = " << nofSamples << "\n";
            std::cerr << "\tdet(AA) = " << det << "\n\n";
            
			// error
			status = 1;
			return 0.0;
        }

        // search the lowest positive eigen value
        int lowestId = -1;
        for (uint i=0 ; i<N ; ++i)
        {
            double ev = gsl_vector_get(eigen_values, i);
            if(fabs(ev)<1e-9)
                ev = 0;
            if ((ev>0) && (lowestId==-1 || ev<gsl_vector_get(eigen_values, lowestId)))
            {
                lowestId = i;
            }
        }
        
        // keep the value of the eigen value for the gradient calculation
		if ((lowestId < 0) || (lowestId > N-1))
		{
			printf("Error at %d\n", __LINE__);
			//for (int i = 0; i < N; i++)
			//	printf("%lf, ", gsl_vector_get(eigen_values, i));
			//fflush(stdout);
			// error
			status = 1;
			return 0.0;
		}
        mLambda = gsl_vector_get(eigen_values, lowestId);
        
        for (uint i=0 ; i<N ; ++i)
            mVecU[i] = gsl_matrix_get(eigen_vectors, i, lowestId);
        
        double norm = mVecU[1]*mVecU[1]+mVecU[2]*mVecU[2] - 4.*mVecU[0]*mVecU[N-1];
//         std::cout << "norm = " << norm << "\n";
        norm = 1./sqrtf(norm);
        for (uint i=0 ; i<N ; ++i)
            mVecU[i] *= norm;

        gsl_matrix_free(eigen_vectors);
        gsl_vector_free(eigen_values);
        
        // copy
        for (uint i=0 ; i<N ; ++i)
            u[i] = mVecU[i];

		u[4] = u[3];
		u[3] = 0.0;
    }

	if (Expe::Math::Abs(u[4])>1e-9)
    {
        // Sphere
        double b = 1./u[4];
        float3 center = -0.5 * b * make_float3(u[1], u[2], u[3]);
        double radius = Expe::Math::Sqrt( dot(center,center) - b*u[0] );

		// find the direction
		bool pos = true;
		double err1 = 0.0;
		double err2 = 0.0;
		for (int i = 0; i < xy.size(); i++)
		{
			double cas1 = sqrt(radius * radius - (xy[i].x - center.x) * (xy[i].x - center.x)) + center.y;
			double cas2 = -sqrt(radius * radius - (xy[i].x - center.x) * (xy[i].x - center.x)) + center.y;
			if (myiswn(cas1) || myiswn(cas2))
				continue;
			
			err1 += abs(xy[i].y - cas1) / xy[i].z;
			err2 += abs(xy[i].y - cas2) / xy[i].z;
		}
		if (err2 < err1)
			pos = false;

		// find the value now
		double ret;
		if (pos)
			ret = sqrt(radius * radius - (qx - center.x) * (qx - center.x)) + center.y;
		else
			ret = -sqrt(radius * radius - (qx - center.x) * (qx - center.x)) + center.y;

		if ((ret > 15) || (ret < -5) || myiswn(ret))
		{
			// print debug info
			//for (int i = 0; i < xy.size(); i++)
			//{
			//	printf("%lf %lf %lf\n", xy[i].x, xy[i].y, 1.0 / xy[i].z);
			//}
			//printf("\n");
			//printf("minx %lf maxx %lf miny %lf maxy %lf\n", lx, hx, ly, hy);
			//lx = min(lx, qx);
			//hx = max(hx, qx);
			//for ( double px = lx; px < hx; px+=0.1)
			//{
			//	double v;
			//	if (pos)
			//		v = sqrt(radius * radius - (px - center.x) * (px - center.x)) + center.y;
			//	else
			//		v = -sqrt(radius * radius - (px - center.x) * (px - center.x)) + center.y;
			//	printf("%lf %lf\n", px, v);
			//}
			//printf("\n");

			//printf(",");

			// error
			status = 1;
			return 0.0;
		}

		return oly + 0.1 * ret * (ohy - oly);
    }
    else if (u[4]==0.)
    {
        // Plane
		float3 u13 = make_float3(u[1], u[2], u[3]);
        double s = 1.0 / length(u13);
        float3 normal = u13 * s;
        double offset = u[0] * s;
		return -(normal.x * qx + offset) / normal.y;
    }

	  
}
