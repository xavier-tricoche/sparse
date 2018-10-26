/*
----------------------------------------------------------------------

This source file is part of Expé
(EXperimental Point Engine)

Copyright (c) 2004-2007 by
 - Computer Graphics Laboratory, ETH Zurich
 - IRIT, University of Toulouse
 - Gael Guennebaud.

----------------------------------------------------------------------

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.

http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
----------------------------------------------------------------------
*/



#ifndef _ExpeLinearAlgebra_h_ 
#define _ExpeLinearAlgebra_h_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;


#include "ExpePrerequisites.h"

#ifndef EXPE_HAVE_GSL
#error It is forbiden to use ExpeLinearAlgebra.h while GSL is not available.
#endif

#include <gsl/gsl_matrix.h>

namespace Expe
{

// ela stands for Expe Linear Algebra
namespace ela
{

enum GenEigenForm {GEF_Ax_lBx, GEF_BAx_lx, GEF_ABx_lx};

/** Solve a generalized eigensystem of the form form where both A and B are symmetrics and B is definite positive.
*/
extern int eigen_generalized_sym(gsl_matrix* matA, gsl_matrix* matB, gsl_vector* evals, gsl_matrix* evecs, GenEigenForm form);

/// print a matrix
extern void print_matrix(gsl_matrix* m);

/// Computes the determinant of a matrix.
extern double det(gsl_matrix* m);


#ifdef EXPE_HAVE_OCTAVE
/// Solve an eigen system using octave (more robust to handle ill-conditionned cases)
extern int eigen_by_octave(gsl_matrix* matA, gsl_vector* evals, gsl_matrix* evecs);
#endif

}


}

#endif

