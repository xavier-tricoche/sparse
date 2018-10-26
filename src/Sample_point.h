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

#include <vector>
#include <vector_types.h>
#include <vector_functions.h>
#include <driver_types.h>
#include <cutil.h>
#include <cuda_runtime_api.h>
#include <helper_cuda.h>
#include <helper_math.h>

#include "MyMath.h"
#include "MyGeometry.h"
#include "MyTeem.h"


using namespace std;

class Sample_point
{
public:

	float3 coordinate;
	int id;
	double value;
	double gradient[3];

	Sample_point(void);
	~Sample_point(void);
};
