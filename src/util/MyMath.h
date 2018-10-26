#pragma once

#ifndef MYMATH
#define MYMATH

#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <string.h>
#include <math.h>
#include <cmath>
#include <time.h>
#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <omp.h>

using namespace std;

#define MYPI 3.141592653589793

#define ERRP true
#define CUERRP(x) {if (ERRP) {printf("%s -> Function \"%s\" in file %s at line %d\n", x, __FUNCDNAME__, __FILE__, __LINE__); fflush(stdout);}}

namespace 
{
	double length(double2& x)
	{
		return sqrt(x.x * x.x + x.y * x.y);
	}

	double2 normalize(double2& x)
	{
		double a = length(x);
		if (a == 0.0)
			return make_double2(x.x, x.y);
		return make_double2(x.x / a, x.y / a);
	}

	inline double2 operator-(double2 a, double2 b)
	{
		return make_double2(a.x - b.x, a.y - b.y);
	}

	inline double2 operator-(double2 &a)
	{
		return make_double2(-a.x, -a.y);
	}

	inline double2 operator*(double2 a, float b)
	{
		return make_double2(a.x * b, a.y * b);
	}

	inline double2 operator*(float b, double2 a)
	{
		return make_double2(b * a.x, b * a.y);
	}

	inline double2 operator+(double2 a, double2 b)
	{
		return make_double2(a.x + b.x, a.y + b.y);
	}

	bool myisnan(double var)
	{
		volatile double temp = var;
		return temp != temp;
	}

	bool myisinf(double var)
	{
		return (numeric_limits<double>::infinity() == var) || (numeric_limits<double>::infinity() == -var);
	}

	bool myiswn(double var)
	{
		return (myisnan(var) || myisinf(var));
	}

	bool myisnan(float var)
	{
		volatile float temp = var;
		return temp != temp;
	}

	bool myisinf(float var)
	{
		return (numeric_limits<float>::infinity() == var) || (numeric_limits<float>::infinity() == -var);
	}

	bool myiswn(float var)
	{
		return (myisnan(var) || myisinf(var));
	}

	int myround(double number)
	{
		return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
	}

	int cint(double x){

		double fractpart, intpart;
		fractpart = modf (x , &intpart);

		if (fractpart>=.5)
			return x>=0?ceil(x):floor(x);
		else
			return x<0?ceil(x):floor(x);
	}

	float CopySign(float x, float y)
	{
		if (y >= 0)
			return fabs(x);
		else
			return fabs(x) * -1;
	}

	double phi_c(double r, double w, double d)
	{
		// single spatial radial energy profile, a tunable C2
		// piecewise cubic function φc(r) with a potential well of depth d < 0 at
		// radius r = w, shown in Fig. 3
		// see http://lmi.bwh.harvard.edu/papers/pdfs/2009/kindlmannTVGC09.pdf
		// d is chosen to be a small negative number -0.1

		double ar = (r > 0.0) ? r : -r;

		if ((ar >= 0) && (ar < w))
		{
			return  1 
					+ (3 * (d - 1) * ar / w) 
					- (3 * (d - 1) * ar * ar / (w * w)) 
					+ ((d - 1) * ar * ar * ar / (w * w * w));
		}
		if ((ar >= w) && (ar < 1))
		{
			return  d
					- (2 * d * pow(ar - w, 3) / pow(w - 1, 3))
					- (3 * d * pow(ar - w, 2) / pow(w - 1, 2));
		}

		return 0.0;
	}

	class gsl_akima_interpolator
	{
	public:
		double* x;
		double* y;
		int n;
		gsl_interp_accel *acc;
		gsl_spline *spline;

		gsl_akima_interpolator(int _n, double _x[], double _y[])
		{
			n = _n;
			x = (double*) malloc(n * sizeof(double));
			y = (double*) malloc(n * sizeof(double));
			memcpy(x, _x, n * sizeof(double));
			memcpy(y, _y, n * sizeof(double));

			acc = gsl_interp_accel_alloc ();
			spline = gsl_spline_alloc (gsl_interp_akima, n);
			gsl_spline_init (spline, x, y, n);
		}

		~gsl_akima_interpolator()
		{
			free(x);
			free(y);
			gsl_spline_free (spline);
			gsl_interp_accel_free (acc);
		}

		double GetValue(double xi)
		{
			return gsl_spline_eval (spline, xi, acc);
		}
	};

	float2 cross(const float2& u, const float2& v)
	{
		return make_float2(u.x * v.y - u.y * v.x);
	}

	float3 barycentric(float2 A, float2 B, float2 C, float2 P)
	{
		// Compute the normal of the triangle
		float2 N = normalize(cross(B-A,C-A));

		// Compute twice area of triangle ABC
		double AreaABC = dot(N,cross(B-A,C-A));

		// Compute a, b, c
		double AreaPBC = dot(N,cross(B-P,C-P));
		double a = AreaPBC / AreaABC;
		double AreaPCA = dot(N,cross(C-P,A-P));
		double b = AreaPCA / AreaABC;
		double c = 1.0f - a - b;

		return make_float3(a, b, c);
	}

	double squaredlength(const float2& vec)
	{
		return dot(vec, vec);
	}

	double Gaussian(double a, double c2, double r2)
	{
		// a: is the height of the curve's peak
		// c: controls the width of the "bell"
		return a * exp(- r2 / (2.0 * c2));
	}

	float2 float3_to_float2(const float3& f)
	{
		return make_float2(f.x, f.y);
	}

	double sech(double x) 
	{
		double temp = exp(x);
		return 2.0 / (temp + 1.0 / temp);
	}

	double tanh2(double x)
	{
		double r = tanh(x);
		return r*r;
	}

	double sech2(double x)
	{
		double r = sech(x);
		return r*r;
	}

	double log(double n, double b)  
	{  
		// log(n)/log(2) is log2.  
		return std::log(n) / std::log(b);  
	}

	void randperm(int n, int perm[])
	{
		int i, j, t;
		srand (time(NULL));
		for(i=0; i<n; i++)
			perm[i] = i;
		for(i=0; i<n; i++) {
			j = rand()%(n-i)+i;
			t = perm[j];
			perm[j] = perm[i];
			perm[i] = t;
		}
	}
}

#endif