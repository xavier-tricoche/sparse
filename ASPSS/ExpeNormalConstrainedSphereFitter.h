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



#ifndef _ExpeNormalConstrainedSphereFitter_h_
#define _ExpeNormalConstrainedSphereFitter_h_

#include "ExpeAlgebraicSphere.h"

namespace Expe
{

class Neighborhood;

/** Fit a sphere in a MLS sense using the normals and the method described in [Guennebaud and Gross 2007].
*/

class NormalConstrainedSphereFitter : public AlgebraicSphere
{

public:

    NormalConstrainedSphereFitter();
    
    ~NormalConstrainedSphereFitter();
    
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
    typedef double LocalFloat;
    typedef GetVector<3,LocalFloat>::Type LocalVector3;
    LocalFloat mCovMat[N][N];
    LocalFloat mMatU[N][N];
    LocalFloat mVecB[N];
    
    Real mNormalParameter;
    bool mNormalization;
    
};

}

#endif

 
