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



#ifndef _ExpeVector2Implementation_h_
#define _ExpeVector2Implementation_h_

#include "ExpeCore.h"
#include "ExpeMath.h"
#include <QtDebug>

namespace Expe
{

template <typename ScalarT, class Vector2T>
class Vector2Implementation
{

    typedef typename Expe::TypeInfo<ScalarT>::FloatingPointType FloatT;

public:

    /*__declspec(align(8)) */ ScalarT x;
    ScalarT y;

public:

    /** Has to be overloaded
    */
    inline Vector2Implementation() {}

    /** Has to be overloaded
    */
    inline Vector2Implementation(ScalarT _x, ScalarT _y)
        : x(_x), y(_y)
    {}
    
    inline Vector2Implementation(ScalarT _x)
        : x(_x), y(_x)
    {}

    /** Has to be overloaded
    */
    inline Vector2Implementation(const ScalarT* const coords)
        : x(coords[0]), y(coords[1])
    {}

    /** Has to be overloaded
    */
    inline Vector2Implementation(const Vector2T& v)
        : x(v.x), y(v.y)
    {}
    
    /** Common operator allowing both cast towards ScalarT* and [] operator.
    */
    inline operator ScalarT* ()
    {
        return &x;
    }

    /** Common operator allowing both cast towards ScalarT* and [] operator.
    */
    inline operator const ScalarT* () const
    {
        return &x;
    }
    
    /** Explicit cast towards ScalarT*
    */
    inline ScalarT* data(void)
    {
        return &x;
    }

    /** Explicit cast towards ScalarT*
    */
    inline const ScalarT* data(void) const
    {
        return &x;
    }
    
    /** Assigns the value of the other vtor.
        @param v The other vtor
    */
    inline Vector2T& operator = (const Vector2T& v)
    {
//         #ifdef VECTOR2_USE_SSE
//         vec = v.vec;
//         #else
        x = v.x;
        y = v.y;
//         #endif
        return *static_cast<Vector2T*>(this);
    }
    
    
    /** \name Comparison operators
    */
    //@{
    
    inline bool operator == (const Vector2T& v ) const
    {
        return (x == v.x && y == v.y);
    }

    inline bool operator != (const Vector2T& v ) const
    {
        return (x != v.x || y != v.y);
    }
    
    /** Returns true if the vtor's s components are all greater
        that the ones of the vtor it is compared against.
    */
    inline bool operator < ( const Vector2T& v ) const
    {
        return (x < v.x && y < v.y);
    }

    /** Returns true if the vtor's s components are all greater or equal
        that the ones of the vtor it is compared against.
    */
    inline bool operator <= ( const Vector2T& v ) const
    {
        return (x <= v.x && y <= v.y);
    }

    /** Returns true if the vtor's s components are all smaller
        that the ones of the vtor it is compared against.
    */
    inline bool operator > ( const Vector2T& v ) const
    {
        return (x > v.x && y > v.y);
    }

    /** Returns true if the vtor's s components are all smaller or equal
        that the ones of the vtor it is compared against.
    */
    inline bool operator >= ( const Vector2T& v ) const
    {
        return (x >= v.x && y >= v.y);
    }
    
    //@}
    
    
    /** \name Arithmetic operators
    */
    //@{
    
    inline Vector2T operator + (ScalarT s) const
    {
        Vector2T result;
        result.x = x + s;
        result.y = y + s;
        return result;
    }
    
    inline Vector2T operator - (ScalarT s) const
    {
        Vector2T result;
        result.x = x - s;
        result.y = y - s;
        return result;
    }
    
    inline Vector2T operator + (const Vector2T& v) const
    {
        Vector2T result;
//         #ifdef VECTOR2_USE_SSE
//         result.vec = vec + v.vec;
//         #else
        result.x = x + v.x;
        result.y = y + v.y;
//         #endif
        return result;
    }

    inline Vector2T operator - (const Vector2T& v) const
    {
        Vector2T result;
//         #ifdef VECTOR2_USE_SSE
//         result.vec = vec - v.vec;
//         #else
        result.x = x - v.x;
        result.y = y - v.y;
//         #endif
        return result;
    }

    inline Vector2T operator * (ScalarT s) const
    {
        Vector2T result;
        result.x = s*x;
        result.y = s*y;
        return result;
    }

    /** Per components multiplication.
    */
    inline Vector2T operator * (const Vector2T& v) const
    {
        Vector2T result;
//         #ifdef VECTOR2_USE_SSE
//         result.vec = vec * v.vec;
//         #else
        result.x = v.x * x;
        result.y = v.y * y;
//         #endif
        return result;
    }

    /** Division by a scalar.
        \warning should be overloaded for floating point data.
    */
    inline Vector2T operator / (ScalarT s) const
    {
        assert( s != ScalarT(0) );
        Vector2T result;
        result.x = x / s;
        result.y = y / s;
        return result;
    }
    
    /** Per component division.
    */
    inline Vector2T operator / (const Vector2T& v) const
    {
        assert( v.x != ScalarT(0) );
        assert( v.y != ScalarT(0) );
        Vector2T result;
        result.x = x / v.x;
        result.y = y / v.y;
        return result;
    }

    inline Vector2T operator - () const
    {
        return Vector2T(-x, -y);
    }

    inline friend Vector2T operator * (ScalarT s, const Vector2T& v)
    {
        Vector2T result;
        result.x = s * v.x;
        result.y = s * v.y;
        return result;
    }
    
    inline friend Vector2T operator + (ScalarT s, const Vector2T& v)
    {
        Vector2T result;
        result.x = s + v.x;
        result.y = s + v.y;
        return result;
    }
    
    inline friend Vector2T operator - (ScalarT s, const Vector2T& v)
    {
        Vector2T result;
        result.x = s - v.x;
        result.y = s - v.y;
        return result;
    }
    
    //@}

    /** \name Arithmetic update operators
    */
    //@{
    inline Vector2T& operator += (ScalarT s)
    {
        x += s;
        y += s;
        return *static_cast<Vector2T*>(this);
    }
    
    inline Vector2T& operator += (const Vector2T& v)
    {
//         #ifdef VECTOR2_USE_SSE
//         vec += v.vec;
//         #else
        x += v.x;
        y += v.y;
//         #endif
        return *static_cast<Vector2T*>(this);
    }

    inline Vector2T& operator -= (ScalarT s)
    {
        x -= s;
        y -= s;
        return *static_cast<Vector2T*>(this);
    }
    
    inline Vector2T& operator -= (const Vector2T& v)
    {
//         #ifdef VECTOR2_USE_SSE
//         vec -= v.vec;
//         #else
        x -= v.x;
        y -= v.y;
//         #endif
        return *static_cast<Vector2T*>(this);
    }

    inline Vector2T& operator *= (ScalarT s)
    {
        x *= s;
        y *= s;
        return *static_cast<Vector2T*>(this);
    }
    
    /** Per components multiplication.
    */
    inline Vector2T& operator *= (const Vector2T& v)
    {
//         #ifdef VECTOR2_USE_SSE
//         vec *= v.vec;
//         #else
        x *= v.x;
        y *= v.y;
//         #endif
        return *static_cast<Vector2T*>(this);
    }

    /** Division by a scalar.
        \warning should be overloaded for floating point data.
    */
    inline Vector2T& operator /= (ScalarT s)
    {
        assert( s != ScalarT(0) );
        x /= s;
        y /= s;
        return *static_cast<Vector2T*>(this);
    }
    
    inline Vector2T& operator /= (const Vector2T& v)
    {
        assert( v.x != ScalarT(0) );
        assert( v.y != ScalarT(0) );
        x /= v.x;
        y /= v.y;
        return *static_cast<Vector2T*>(this);
    }
    
    //@}
    
    /** Returns the length (magnitude) of the vtor.
    */
    inline FloatT length () const
    {
        return Math::Sqrt(FloatT(x*x + y*y));
    }

    /** Returns the square of the length of the vtor.
    */
    inline ScalarT squaredLength (void) const
    {
        return x*x + y*y;
    }
    
    inline ScalarT squaredDistanceTo(const Vector2T& v) const
    {
        ScalarT dx,dy;
        dx = x-v.x;
        dy = y-v.y;
        return dx*dx + dy*dy;
    }

    inline FloatT distanceTo(const Vector2T& v) const
    {
        ScalarT dx,dy;
        dx = x-v.x;
        dy = y-v.y;
        return Math::Sqrt(FloatT(dx*dx + dy*dy));
    }
    
    /** Calculates the dot (s) resultuct of this vtor with another.
        @remarks
            The dot product can be used to calculate the angle between 2
            vtors. If both are unit vtors, the dot product is the
            cosine of the angle; otherwise the dot product must be
            divided by the product of the lengths of both vtors to get
            the cosine of the angle. This result can further be used to
            calculate the distance of a point from a plane.
        @param v Vector with which to calculate the dot resultuct (together with this one).
        @returns A scalar representing the dot product value.
    */
    inline ScalarT dot(const Vector2T& v) const
    {
        return x*v.x + y*v.y;
    }
    
    /** Calculates the 2D cross-product of 2 vtors.
    */
    inline Vector2T cross( const Vector2T& v ) const
    {
        return x*v.y - y*v.x;
    }
    
    
    /** Sets this vtor's components to the minimum of its own and the
        ones of the passed in vtor.
        @remarks
            'Minimum' in this case means the combination of the lowest
            value of x, y and z from both vtors. Lowest is taken just
            numerically, not magnitude, so -1 < 0.
    */
    inline void makeFloor(const Vector2T& cmp)
    {
        if( cmp.x < x ) x = cmp.x;
        if( cmp.y < y ) y = cmp.y;
    }

    /** Sets this vtor's components to the maximum of its own and the
        ones of the passed in vtor.
        @remarks
            'Maximum' in this case means the combination of the highest
            value of x, y and z from both vtors. Highest is taken just
            numerically, not magnitude, so 1 > -3.
    */
    inline void makeCeil(const Vector2T& cmp)
    {
        if( cmp.x > x ) x = cmp.x;
        if( cmp.y > y ) y = cmp.y;
    }

    /** Return the maximal component value.
    */
    inline ScalarT maxComponent(void) const
    {
        return Math::Max(x,y);
    }

    /** Return the minimal component value.
    */
    inline ScalarT minComponent(void) const
    {
        return Math::Min(x,y);
    }
    
    /** Generates a vector perpendicular to this vector.
    */
    inline Vector2T perpendicular(void) const
    {
        return Vector2T(-y, x);
    }
    
    /** Calculates a reflection vtor to the plane with the given normal .
    @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
    */
    inline Vector2T reflect(const Vector2& normal)
    {
        return Vector2T( ( ScalarT(2) * this->dot(normal) * normal ) - *this );
    }
};









template <typename ScalarT,class Vector2T> class Vector2FloatImplementation : public Vector2Implementation<ScalarT,Vector2T>
{

typedef Vector2Implementation<ScalarT,Vector2T> Super;

public:
    
    inline Vector2FloatImplementation() {}

    inline Vector2FloatImplementation(ScalarT _x, ScalarT _y) : Super(_x,_y) {}
    inline Vector2FloatImplementation(ScalarT _x) : Super(_x) {}
    
    inline Vector2FloatImplementation(const ScalarT* const coords) : Super(coords) {}
    
    inline Vector2FloatImplementation(const Vector2T& v) : Super(v) {}
    
    inline bool isFinite(void)
    {
        return Math::isFinite(Super::x) && Math::isFinite(Super::y);
    }

    /** Overloaded for efficiency.
    */
    inline Vector2T operator / ( Real s ) const
    {
        assert( s != 0.0 );
        Vector2T result;
        ScalarT inv = 1.0 / s;
        result.x = Super::x * inv;
        result.y = Super::y * inv;
        return result;
    }

    /** Overloaded for efficiency.
    */
    inline Vector2& operator /= ( ScalarT s )
    {
        assert( s != 0.0 );
        ScalarT inv = 1.0 / s;
        Super::x *= inv;
        Super::y *= inv;
        return *this;
    }

    /** Normalizes the vector
        @remarks
            This method normalises the vtor such that it's
            length / magnitude is 1. The result is called a unit vtor.
        @note
            This function will not crash for zero-sized vtors, but there
            will be no changes made to their components.
        @returns The previous length of the vtor.
    */
    inline void normalize(void)
    {
        ScalarT sqLength = Super::x*Super::x + Super::y*Super::y;

        if (sqLength > 1e-12)
        {
            ScalarT invLength = Math::InvSqrt(sqLength);
            Super::x *= invLength;
            Super::y *= invLength;
        }
    }
    
    /** Returns the normalized version of the vector.
        \remarks The vector is left unchanged.
        \see normalize
    */
    inline Vector2T normalized(void) const
    {
        ScalarT sqlength = Super::x*Super::x + Super::y*Super::y;
        if(sqlength > 1e-12)
        {
            ScalarT invLength = Math::InvSqrt(sqlength);
            return Vector2T(Super::x*invLength,Super::y*invLength);
        }
        return *this;
    }


    /** Scales the vector such that its length is l.
        \param l target length
        \remarks the initial vector length must not be null
    */
    inline void scaleTo(ScalarT l)
    {
        assert(this->squaredLength()>1e-12);
        
        ScalarT invLength = Math::InvSqrt( Super::x*Super::x + Super::y*Super::y ) * l;
        Super::x *= invLength;
        Super::y *= invLength;
    }

    /** Returns a vector at a point half way between this and the passed
        in vector.
    */
    inline Vector2T midPoint(const Vector2T& vec) const
    {
        return Vector2T( (Super::x+vec.x)*0.5, (Super::y+vec.y)*0.5 );
    }

    /** Performs a linear interpolation between the two vectors.
        \code
        v0.lerp(t,v1) == (1-t)*v0 + t*v1
        \endcode
    */
    inline Vector2T lerp(ScalarT t, const Vector2T v1) const
    {
        Vector2T vAux;
        ScalarT t1 = 1.-t;
        vAux.x = t1*Super::x + t*v1.x;
        vAux.y = t1*Super::y + t*v1.y;
        return vAux;
    }

    /** Returns true if this vtor is zero length. */
    inline bool isZero(void) const
    {
        ScalarT sqlen = (Super::x * Super::x) + (Super::y * Super::y);
        return (sqlen < (1e-06 * 1e-06));

    }

};


//--------------------------------------------------------------------------------
// Begining of 32 bist integer Vector2i
//--------------------------------------------------------------------------------

/** Standard 2-dimensional vector of 32 bits int
*/
template <typename ScalarT,class Vector2T> class Vector2IntegerImplementation : public Vector2Implementation<ScalarT,Vector2T>
{
typedef Vector2Implementation<ScalarT,Vector2T> Super;

public:
    inline Vector2IntegerImplementation()
    {}

    inline Vector2IntegerImplementation(ScalarT _x, ScalarT _y) : Super(_x,_y) {}
    inline Vector2IntegerImplementation(ScalarT _x) : Super(_x) {}
    
    inline Vector2IntegerImplementation(const ScalarT* const coords)
        : Super(coords)
    {}
    
    inline Vector2IntegerImplementation(const Vector2T& vec)
        : Super(vec)
    {}

    /** Returns true if this vector is zero */
    inline bool isZero(void) const
    {
        return Super::x==0 && Super::y==0;
    }
};



}


#endif
