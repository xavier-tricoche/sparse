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



#ifndef _ExpePlane_h_ 
#define _ExpePlane_h_

#include "ExpeCore.h"
#include "ExpeVector3.h"

namespace Expe
{

template <typename ScalarT>
class PlaneT
{
    typedef typename GetVector<3,ScalarT>::Type Vector3T;
public:

    PlaneT()
    {}
    
    /** Construct a plane from its normal and a point onto the plane.
        \warning the vector normal is assumed to be normalized.
    */
    PlaneT(const Vector3T& normal, const Vector3T e)
        : mNormal(normal), mOffset(-normal.dotProduct(e))
    {}
    
    /** Constructs a plane from its normal and distance to the origin.
        \warning the vector normal is assumed to be normalized.
    */
    PlaneT(const Vector3T& normal, ScalarT d)
        : mNormal(normal), mOffset(d)
    {}
    
    ~PlaneT()
    {}
    
    /** Computes the signed distance between the plane and a point.
    */
    inline ScalarT distanceTo(const Vector3T& p) const
    {
        return mNormal.dot(p) + mOffset;
    }
    
    /** Computes the projection of a point onto the plane.
    */
    inline Vector3T project(const Vector3T& p) const
    {
        return p - distanceTo(p)*mNormal;
    }
    
    /** Returns a 4 component vector storing the parameter of the plane equation
        a*x + b*y + c*z + d = 0
    */
    inline const ScalarT* getEquation(void) const {return mNormal;}
    
    inline const Vector3T& normal(void) const {return mNormal;}

    inline ScalarT offset(void) const {return mOffset;}
    
    /** Set the normal of the plane.
        \warning the vector normal is assumed to be normalized.
    */
    inline void setNormal(const Vector3T& normal) {mNormal = normal;}
    
    inline void setOffset(ScalarT d) {mOffset = d;}

protected:

    Vector3T mNormal;
    ScalarT mOffset;
};

typedef PlaneT<Real> Plane;
typedef PlaneT<float> Planef;
typedef PlaneT<double> Planed;

}

#endif

