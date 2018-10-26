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

#include "ExpeQuaternion.h"

#include "ExpeMatrix3.h"

namespace Expe {

const Real Quaternion::ms_fEpsilon = 1e-03;
const Quaternion Quaternion::ZERO(0.0,0.0,0.0,0.0);
const Quaternion Quaternion::IDENTITY(1.0,0.0,0.0,0.0);

//-----------------------------------------------------------------------
Quaternion::Quaternion (Real fW, Real fX, Real fY, Real fZ)
{
    w = fW;
    x = fX;
    y = fY;
    z = fZ;
}
//-----------------------------------------------------------------------
Quaternion::Quaternion (const Quaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
}
//-----------------------------------------------------------------------
void Quaternion::fromRotationMatrix (const Matrix3& kRot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    Real fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
    Real fRoot;

    if ( fTrace > 0.0 )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.0);  // 2w
        w = 0.5*fRoot;
        fRoot = 0.5/fRoot;  // 1/(4w)
        x = (kRot[2][1]-kRot[1][2])*fRoot;
        y = (kRot[0][2]-kRot[2][0])*fRoot;
        z = (kRot[1][0]-kRot[0][1])*fRoot;
    }
    else
    {
        // |w| <= 1/2
        static int s_iNext[3] = { 1, 2, 0 };
        int i = 0;
        if ( kRot[1][1] > kRot[0][0] )
            i = 1;
        if ( kRot[2][2] > kRot[i][i] )
            i = 2;
        int j = s_iNext[i];
        int k = s_iNext[j];

        fRoot = sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0);
        Real* apkQuat[3] = { &x, &y, &z };
        *apkQuat[i] = 0.5*fRoot;
        fRoot = 0.5/fRoot;
        w = (kRot[k][j]-kRot[j][k])*fRoot;
        *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
        *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
    }
}
//-----------------------------------------------------------------------
void Quaternion::toRotationMatrix (Matrix3& kRot) const
{
    Real fTx  = 2.0*x;
    Real fTy  = 2.0*y;
    Real fTz  = 2.0*z;
    Real fTwx = fTx*w;
    Real fTwy = fTy*w;
    Real fTwz = fTz*w;
    Real fTxx = fTx*x;
    Real fTxy = fTy*x;
    Real fTxz = fTz*x;
    Real fTyy = fTy*y;
    Real fTyz = fTz*y;
    Real fTzz = fTz*z;

    kRot[0][0] = 1.0-(fTyy+fTzz);
    kRot[0][1] = fTxy-fTwz;
    kRot[0][2] = fTxz+fTwy;
    kRot[1][0] = fTxy+fTwz;
    kRot[1][1] = 1.0-(fTxx+fTzz);
    kRot[1][2] = fTyz-fTwx;
    kRot[2][0] = fTxz-fTwy;
    kRot[2][1] = fTyz+fTwx;
    kRot[2][2] = 1.0-(fTxx+fTyy);
}
//-----------------------------------------------------------------------
void Quaternion::fromAngleAxis (const Real& rfAngle,
    const Vector3& rkAxis)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Real fHalfAngle = 0.5*rfAngle;
    Real fSin = sin(fHalfAngle);
    w = cos(fHalfAngle);
    x = fSin*rkAxis.x;
    y = fSin*rkAxis.y;
    z = fSin*rkAxis.z;
}
//-----------------------------------------------------------------------
void Quaternion::toAngleAxis (Real& rfAngle, Vector3& rkAxis) const
{
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Real fSqrLength = x*x+y*y+z*z;
    if ( fSqrLength > 0.0 )
    {
        rfAngle = 2.0*acos(w);
        Real fInvLength = 1.0/sqrt(fSqrLength);
        rkAxis.x = x*fInvLength;
        rkAxis.y = y*fInvLength;
        rkAxis.z = z*fInvLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        rfAngle = 0.0;
        rkAxis.x = 1.0;
        rkAxis.y = 0.0;
        rkAxis.z = 0.0;
    }
}
//-----------------------------------------------------------------------
void Quaternion::fromAxes (const Vector3* akAxis)
{
    Matrix3 kRot;

    for (int iCol = 0; iCol < 3; iCol++)
    {
        kRot[0][iCol] = akAxis[iCol].x;
        kRot[1][iCol] = akAxis[iCol].y;
        kRot[2][iCol] = akAxis[iCol].z;
    }

    fromRotationMatrix(kRot);
}
//-----------------------------------------------------------------------
void Quaternion::fromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
{
    Matrix3 kRot;

    kRot[0][0] = xAxis.x;
    kRot[1][0] = xAxis.y;
    kRot[2][0] = xAxis.z;

    kRot[0][1] = yAxis.x;
    kRot[1][1] = yAxis.y;
    kRot[2][1] = yAxis.z;

    kRot[0][2] = zAxis.x;
    kRot[1][2] = zAxis.y;
    kRot[2][2] = zAxis.z;

    fromRotationMatrix(kRot);

}
//-----------------------------------------------------------------------
void Quaternion::toAxes (Vector3* akAxis) const
{
    Matrix3 kRot;

    toRotationMatrix(kRot);

    for (int iCol = 0; iCol < 3; iCol++)
    {
        akAxis[iCol].x = kRot[0][iCol];
        akAxis[iCol].y = kRot[1][iCol];
        akAxis[iCol].z = kRot[2][iCol];
    }
}
//-----------------------------------------------------------------------
void Quaternion::toAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const
{
    Matrix3 kRot;

    toRotationMatrix(kRot);

    xAxis.x = kRot[0][0];
    xAxis.y = kRot[1][0];
    xAxis.z = kRot[2][0];

    yAxis.x = kRot[0][1];
    yAxis.y = kRot[1][1];
    yAxis.z = kRot[2][1];

    zAxis.x = kRot[0][2];
    zAxis.y = kRot[1][2];
    zAxis.z = kRot[2][2];
}

//-----------------------------------------------------------------------
Quaternion& Quaternion::operator= (const Quaternion& rkQ)
{
    w = rkQ.w;
    x = rkQ.x;
    y = rkQ.y;
    z = rkQ.z;
    return *this;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
{
    return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator- (const Quaternion& rkQ) const
{
    return Quaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator* (const Quaternion& rkQ) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

    return Quaternion
    (
        w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
        w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
        w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
        w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
    );
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator* (Real fScalar) const
{
    return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
}
//-----------------------------------------------------------------------
Quaternion operator* (Real fScalar, const Quaternion& rkQ)
{
    return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,
        fScalar*rkQ.z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator- () const
{
    return Quaternion(-w,-x,-y,-z);
}
//-----------------------------------------------------------------------
Real Quaternion::dot (const Quaternion& rkQ) const
{
    return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}
//-----------------------------------------------------------------------
Real Quaternion::squaredLength() const
{
    return w*w+x*x+y*y+z*z;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::inverse () const
{
    Real fNorm = w*w+x*x+y*y+z*z;
    if ( fNorm > 0.0 )
    {
        Real fInvNorm = 1.0/fNorm;
        return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
    }
    else
    {
        // return an invalid result to flag the error
        return ZERO;
    }
}
//-----------------------------------------------------------------------
Quaternion Quaternion::unitInverse () const
{
    // assert:  'this' is unit length
    return Quaternion(w,-x,-y,-z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::exp () const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    Real fAngle = sqrt(x*x+y*y+z*z);
    Real fSin = sin(fAngle);

    Quaternion kResult;
    kResult.w = cos(fAngle);

    if ( fabs(fSin) >= ms_fEpsilon )
    {
        Real fCoeff = fSin/fAngle;
        kResult.x = fCoeff*x;
        kResult.y = fCoeff*y;
        kResult.z = fCoeff*z;
    }
    else
    {
        kResult.x = x;
        kResult.y = y;
        kResult.z = z;
    }

    return kResult;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::log () const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    Quaternion kResult;
    kResult.w = 0.0;

    if ( fabs(w) < 1.0 )
    {
        Real fAngle = acos(w);
        Real fSin = sin(fAngle);
        if ( fabs(fSin) >= ms_fEpsilon )
        {
            Real fCoeff = fAngle/fSin;
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
            return kResult;
        }
    }

    kResult.x = x;
    kResult.y = y;
    kResult.z = z;

    return kResult;
}
//-----------------------------------------------------------------------
Vector3 Quaternion::operator* (const Vector3& v) const
{
    // nVidia SDK implementation
    Vector3 uv, uuv; 
    Vector3 qvec(x, y, z);
    uv = qvec.cross(v); 
    uuv = qvec.cross(uv); 
    uv *= (2.0f * w); 
    uuv *= 2.0f; 
    
    return v + uv + uuv;

}
//-----------------------------------------------------------------------
Quaternion Quaternion::Slerp (Real t, const Quaternion& q0,
    const Quaternion& q1)
{
    Real fCos = q0.dot(q1);        

    if ( fCos < -1.0 )
        fCos = -1.0;
    else if ( fCos > 1.0 )
        fCos = 1.0;
        
    Real fAngle = acos(fCos);
    //std::cout << "fCos = " << fCos << "  Angle = " << 180.*fAngle/M_PI << std::endl;
    /*if(fAngle>0.5*M_PI)
        fAngle = M_PI - fAngle;*/
    Real fSin = sin(fAngle);
    
    //if ( fabs(fAngle) < ms_fEpsilon )
    //    return rkP;

    if ( fSin < ms_fEpsilon )
    {
        return q0;
    }
    else
    {            
        Real fInvSin = 1.0/fSin;
        Real fCoeff0 = sin((1.0-t)*fAngle)*fInvSin;
        Real fCoeff1 = sin(t*fAngle)*fInvSin;
        return fCoeff0*q0 + fCoeff1*q1;
    }
}
//-----------------------------------------------------------------------
Quaternion Quaternion::SlerpExtraSpins (Real fT,
    const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
{
    Real fCos = rkP.dot(rkQ);
    Real fAngle = acos(fCos);

    if ( fabs(fAngle) < ms_fEpsilon )
        return rkP;

    Real fSin = sin(fAngle);
    Real fPhase = M_PI*iExtraSpins*fT;
    Real fInvSin = 1.0/fSin;
    Real fCoeff0 = sin((1.0-fT)*fAngle - fPhase)*fInvSin;
    Real fCoeff1 = sin(fT*fAngle + fPhase)*fInvSin;
    return fCoeff0*rkP + fCoeff1*rkQ;
}
//-----------------------------------------------------------------------
void Quaternion::Intermediate (const Quaternion& rkQ0,
    const Quaternion& rkQ1, const Quaternion& rkQ2,
    Quaternion& rkA, Quaternion& rkB)
{
    // assert:  q0, q1, q2 are unit quaternions

    Quaternion kQ0inv = rkQ0.unitInverse();
    Quaternion kQ1inv = rkQ1.unitInverse();
    Quaternion rkP0 = kQ0inv*rkQ1;
    Quaternion rkP1 = kQ1inv*rkQ2;
    Quaternion kArg = 0.25*(rkP0.log()-rkP1.log());
    Quaternion kMinusArg = -kArg;

    rkA = rkQ1*kArg.exp();
    rkB = rkQ1*kMinusArg.exp();
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Squad (Real fT,
    const Quaternion& rkP, const Quaternion& rkA,
    const Quaternion& rkB, const Quaternion& rkQ)
{
    Real fSlerpT = 2.0*fT*(1.0-fT);
    Quaternion kSlerpP = Slerp(fT,rkP,rkQ);
    Quaternion kSlerpQ = Slerp(fT,rkA,rkB);
    return Slerp(fSlerpT,kSlerpP,kSlerpQ);
}
//-----------------------------------------------------------------------
bool Quaternion::operator== (const Quaternion& rhs) const
{
    return (rhs.x == x) && (rhs.y == y) &&
        (rhs.z == z) && (rhs.w == w);
}

}
