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

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <vector_types.h>
#include <vector_functions.h>
#include <cutil_inline.h>
#include <helper_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multifit.h>

#include "MyMath.h"

using namespace std;

double SmoothStepFitting1D(vector<float4>& xy, double lx, double hx, double ly, double hy, double qx, int& status, double ia, vector<double>& info);
double SphereFitting(vector<float3>& xy, double lx, double hx, double ly, double hy, double qx, int& status);
double SplineFitting(vector<float4>& xy, double lx, double hx, double ly, double hy, double qx, int& status);
double CubicSplineFitting(vector<float4>& xy, double lx, double hx, double ly, double hy, double qx, int& status);
double CircleFitting(vector<float4>& xy, double lx, double hx, double ly, double hy, double qx, int& status);
