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



// NOTE THAT THIS FILE IS BASED ON MATERIAL FROM:

// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000, All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#ifndef _ExpeQuaternion_h_
#define _ExpeQuaternion_h_

#include "ExpeCore.h"
#include "ExpeMath.h"
#include <iostream>

namespace Expe {

/** Implementation of a Quaternion, i.e. a rotation around an axis.
*/
class Quaternion
{
public:

    Real w, x, y, z;
    
    Quaternion (Real fW = 1.0, Real fX = 0.0, Real fY = 0.0, Real fZ = 0.0);
    Quaternion (const Quaternion& rkQ);
    
    inline Real* data(void) {return &w;}
    inline const Real* data(void) const {return &w;}

    void fromRotationMatrix (const Matrix3& kRot);
    void toRotationMatrix (Matrix3& kRot) const;
    void fromAngleAxis (const Real& rfAngle, const Vector3& rkAxis);
    void toAngleAxis (Real& rfAngle, Vector3& rkAxis) const;
    void fromAxes (const Vector3* akAxis);
    void fromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
    void toAxes (Vector3* akAxis) const;
    void toAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

    Quaternion& operator= (const Quaternion& rkQ);
    Quaternion operator+ (const Quaternion& rkQ) const;
    Quaternion operator- (const Quaternion& rkQ) const;
    Quaternion operator* (const Quaternion& rkQ) const;
    Quaternion operator* (Real fScalar) const;
    friend Quaternion operator* (Real fScalar, const Quaternion& rkQ);
    Quaternion operator- () const;
    bool operator== (const Quaternion& rhs) const;

    // functions of a quaternion
    Real dot (const Quaternion& rkQ) const;  // dot product
    Real squaredLength () const;  // squared-length
    Quaternion inverse () const;  // apply to non-zero quaternion
    Quaternion unitInverse () const;  // apply to unit-length quaternion
    Quaternion exp () const;
    Quaternion log () const;

    // rotation of a vector by a quaternion
    Vector3 operator* (const Vector3& rkVector) const;
    
    inline void slerp(Real t, const Quaternion& q1, const Quaternion& q2)
    {
        Real fCos = q1.dot(q2);
        Real fAngle = acosf(fCos);

        if ( fabs(fAngle) < ms_fEpsilon )
        {
            *this = q1;
            return;
        }

        Real fSin = sinf(fAngle);
        Real fInvSin = 1.0/fSin;
        Real fCoeff0 = sinf((1.0-t)*fAngle)*fInvSin;
        Real fCoeff1 = sinf(t*fAngle)*fInvSin;
        *this = fCoeff0*q1 + fCoeff1*q2;
    }

    // spherical linear interpolation
    static Quaternion Slerp (Real fT, const Quaternion& rkP, const Quaternion& rkQ);

    static Quaternion SlerpExtraSpins (Real fT, const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins);

    // setup for spherical quadratic interpolation
    static void Intermediate (const Quaternion& rkQ0, const Quaternion& rkQ1, const Quaternion& rkQ2, Quaternion& rka, Quaternion& rkB);

    // spherical quadratic interpolation
    static Quaternion Squad (Real fT, const Quaternion& rkP, const Quaternion& rkA, const Quaternion& rkB, const Quaternion& rkQ);

    // cutoff for sine near zero
    static const Real ms_fEpsilon;

    // special values
    static const Quaternion ZERO;
    static const Quaternion IDENTITY;

    
    inline friend std::ostream& operator <<
        ( std::ostream& o, const Quaternion& q )
    {
        o << "Quaternion(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
        return o;
    }

};

}

#include <qmetatype.h>
Q_DECLARE_METATYPE(Expe::Quaternion);

#endif 
