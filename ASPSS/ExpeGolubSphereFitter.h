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



#ifndef _ExpeGolubSphereFitter_h_
#define _ExpeGolubSphereFitter_h_

#include "ExpeAlgebraicSphere.h"
#include <gsl/gsl_matrix.h>

namespace Expe
{

class Neighborhood;

/** \brief Fit a sphere without normal

    Fit a sphere in a MLS sense using the Golub et al.'s matrix approximation technique
    as described by Nievergelt in Linear Algebra and its Applications 331 (2001) 43–59.
*/
class GolubSphereFitter : public AlgebraicSphere
{

public:

    GolubSphereFitter();
    
    ~GolubSphereFitter();
    
    /** Does the fitting.
        \return false if the fitting failed.
    */
    bool fit(const Neighborhood* pNeighborhood);
    
    /** Provided for compatibility with the SphericalMlsSurface framework but we currently
        don't know how to compute the gradient with the Golub's matrix approximation.
    */
    Vector3 mlsGradient(const Neighborhood* pNeighborhood, const Vector3& position) const { return Vector3::ZERO; }
    
    /** Returns the variance computed during the fit process.
        This value is useful to numerically compute the MLS gradient.
        \warning fit must be called before.
    */
    Real getVariance(void) { return mVariance; }

protected:

    static const uint N=5;

    Real mVariance;
    
    // variable allocation to save memory allocation/deallocation
    // as well to keep some results between the fit and the compmuatation of the gradient
    gsl_matrix *mMatV;
    gsl_vector *mVecS;
    gsl_vector *mVecWork;
    
    bool mNormalization;
};

}

#endif

