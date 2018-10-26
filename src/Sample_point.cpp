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
#include "Sample_point.h"


Sample_point::Sample_point(void)
{
	value = 0.0;
	gradient[0] = gradient[1] = gradient[2] = 0.0;
}

Sample_point::~Sample_point(void)
{
}
