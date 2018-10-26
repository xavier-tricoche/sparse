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



#ifndef _ExpeSphere_h_ 
#define _ExpeSphere_h_

#include "ExpeCore.h"
#include "ExpeVector3.h"

namespace Expe
{

template <typename ScalarT>
class SphereT
{
typedef typename GetVector<3,ScalarT>::Type Vector3T;
public:

    

    SphereT()
    {}
    
    /** Constructs a sphere from its center and radius.
    */
    SphereT(const Vector3T& center, ScalarT r)
        : mCenter(center), mRadius(r)
    {}
    
    ~SphereT()
    {}
    
    /** Computes the signed distance between the sphere and a point.
        Note that the distance is positive outside and negative inside.
    */
    inline ScalarT distanceTo(const Vector3T& p) const
    {
        return (p-mCenter).length() - mRadius;
    }
    
    /** Computes the projection of a point onto the sphere.
    */
    inline Vector3T project(const Vector3T& p) const
    {
        return (p-mCenter).normalized() * mRadius + mCenter;
    }
    
    inline const Vector3T& center(void) const {return mCenter;}
    
    inline ScalarT radius(void) const {return mRadius;}
    
    inline void setCenter(const Vector3T& center) {mCenter = center;}
    
    inline void setRadius(ScalarT r) {mRadius = r;}

public:

    Vector3T mCenter;
    ScalarT mRadius;
};

typedef SphereT<Real> Sphere;
typedef SphereT<float> Spheref;
typedef SphereT<double> Sphered;

}

#endif

