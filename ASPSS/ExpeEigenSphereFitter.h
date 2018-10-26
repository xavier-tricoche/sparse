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



#ifndef _ExpeEigenSphereFitter_h_
#define _ExpeEigenSphereFitter_h_

#include "ExpeAlgebraicSphere.h"
#include <gsl/gsl_matrix.h>

namespace Expe
{

class Neighborhood;

/** \brief Fit a sphere without normal

    Fit a sphere in a MLS sense using the Pratt's constraint described in [Eigen 1987] and [Guennebaud and Gross 2007].
*/
class EigenSphereFitter : public AlgebraicSphere
{

public:

    EigenSphereFitter();
    
    ~EigenSphereFitter();
    
    /** Does the fitting.
        \return false if the fitting failed.
    */
    bool fit(const Neighborhood* pNeighborhood);
    
    /** Compute the gradient of the MLS surface.
        \warning the function fit must be called before !
    */
    Vector3 mlsGradient(const Neighborhood* pNeighborhood, const Vector3& position) const;

protected:

    static const uint N=5;

    // variable allocation to save memory allocation/deallocation
    // as well to keep some results between the fit and the compmuatation of the gradient
    double mVecU[N];
    double mCovMat[N][N];
    double mLambda;
    
    // used by for the gradient
    mutable double mMatdA[N][N];
    mutable gsl_matrix* mSvdMatV;
    mutable gsl_vector* mSvdVecS;
    mutable gsl_vector* mSvdVecWork;
    
    bool mNormalization;
};

}

#endif

