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



#ifndef _ExpeVector4Implementation_h_
#define _ExpeVector4Implementation_h_

#include "ExpeCore.h"
#include <QtDebug>

namespace Expe
{

/*  TODO implement a homogeneous Vector4 class.
    Note that the current implementation is hybrid between
    a pure 4 component class and a homogeneous Vector4 class...
*/

template <typename ScalarT,class Vector4T> class Vector4Implementation
{
    typedef typename Expe::TypeInfo<ScalarT>::FloatingPointType FloatT;

public:

    ScalarT x, y, z, w;
    
public:

    inline Vector4Implementation() {}

    inline Vector4Implementation(ScalarT _x, ScalarT _y, ScalarT _z = 0, ScalarT _w = 1)
        : x(_x), y(_y), z(_z), w(_w)
    {}
    
    inline Vector4Implementation(ScalarT _x)
        : x(_x), y(_x), z(_x), w(1)
    {}

    inline Vector4Implementation(const ScalarT* const coords)
        : x(coords[0]), y(coords[1]), z(coords[2]), w(coords[3])
    {}

    inline Vector4Implementation(const Vector4T& vec)
        : x(vec.x), y(vec.y), z(vec.z), w(vec.w)
    {}
    
    /** Common operator allowing both cast towards ScalarT* and [] operator.
    */
    inline operator ScalarT* ();

    /** Common operator allowing both cast towards ScalarT* and [] operator.
    */
    inline operator const ScalarT* () const;
    
    /** Explicit cast towards ScalarT*
    */
    inline ScalarT* data(void);
    
    /** Explicit cast towards ScalarT*
    */
    inline const ScalarT* data(void) const;
    
    /** Assigns the value of the other vector.
        @param vec The other vector
    */
    inline Vector4T& operator = (const Vector4T& vec);
    
    template <int a,int b,int c,int d> inline Vector4T sw (void) const;
    
    template <int a,int b,int c,int d> inline Vector4T operator() (void) const;
    
    
    /** \name Comparison operators
    */
    //@{
    
    inline bool operator == (const Vector4T& vec ) const;
    inline bool operator != (const Vector4T& vec ) const;
    
    /** Returns true if the vector's s components are all greater
        that the ones of the vector it is compared against.
    */
    inline bool operator < ( const Vector4T& vec ) const;

    /** Returns true if the vector's s components are all greater or equal
        that the ones of the vector it is compared against.
    */
    inline bool operator <= ( const Vector4T& vec ) const;

    /** Returns true if the vector's s components are all smaller
        that the ones of the vector it is compared against.
    */
    inline bool operator > ( const Vector4T& vec ) const;

    /** Returns true if the vector's s components are all smaller or equal
        that the ones of the vector it is compared against.
    */
    inline bool operator >= ( const Vector4T& vec ) const;
    
    //@}
    
    
    /** \name Arithmetic vector operations
    */
    //@{
    
    inline Vector4T operator + (const Vector4T& vec) const;
    inline Vector4T& operator += (const Vector4T& vec);
    
    inline Vector4T operator - (const Vector4T& vec) const;
    inline Vector4T& operator -= (const Vector4T& vec);
    
    inline Vector4T operator - () const;
    
    inline Vector4T operator * (const Vector4T& vec) const;
    inline Vector4T& operator *= (const Vector4T& vec);

    inline Vector4T operator / (const Vector4T& v) const;
    inline Vector4T& operator /= (const Vector4T& v);
    
    //@}

    /** \name Arithmetic vector/scalar operations
    */
    //@{
    
    inline Vector4T operator + (ScalarT s) const;
    inline Vector4T& operator += (ScalarT s);
    
    inline Vector4T operator - (ScalarT s) const;
    inline Vector4T& operator -= (ScalarT s);
    
//     inline friend Vector4T operator * (ScalarT s, const Vector4T& v);
    inline friend Vector4T operator * (ScalarT s, const Vector4T& v)
    {
        Vector4T result;
        result.x = s * v.x;
        result.y = s * v.y;
        result.z = s * v.z;
        return result;
    }
    
    inline Vector4T operator * (ScalarT s) const;
    inline Vector4T& operator *= (ScalarT s);
    
    /** Division by a scalar.
        \warning should be overloaded for floating point data.
    */
    inline Vector4T operator / (ScalarT s) const;
    
    /** Division by a scalar.
        \warning should be overloaded for floating point data.
    */
    inline Vector4T& operator /= (ScalarT s);
    
    //@}
    
    /** Returns the length of the vector.
    */
    inline FloatT length () const;

    /** Returns the square of the vector length.
    */
    inline ScalarT squaredLength (void) const;
    
    inline ScalarT squaredDistanceTo(const Vector4T& vec) const;

    inline FloatT distanceTo(const Vector4T& vec) const;
    
    /** Calculates the dot product of this vector with another.
    */
    inline ScalarT dot(const Vector4T& vec) const;
    
    /** Calculates the 3 component dot product of this vector with another.
    */
    inline ScalarT dot3(const Vector4T& vec) const;
    
    /** Calculates the 3 component cross-product of 2 vectors.
    */
    inline Vector4T cross3( const Vector4T& vec ) const;
    
    /** Per components sets to minimal value operation.
    */
    inline void makeFloor(const Vector4T& cmp);

    /** Per components sets to maximal value operation.
    */
    inline void makeCeil(const Vector4T& cmp);

    /** Return the maximal component value.
    */
    inline ScalarT maxComponent(void) const;

    /** Return the minimal component value.
    */
    inline ScalarT minComponent(void) const;
    
    //--------------------------------------------------------------------------------
    
    friend QDataStream& operator << (QDataStream& s, const Vector4T& vec)
    {
        s << vec.x << vec.y << vec.z << vec.w;
        return s;
    }
    
    friend QDataStream& operator >> (QDataStream& s, Vector4T& vec)
    {
        s >> vec.x; s >> vec.y; s >> vec.z; s >> vec.w;
        return s;
    }
};


/** 4-dimensional floating point vector
*/
template <typename ScalarT,class Vector4T> class Vector4FloatImplementation : public Vector4Implementation<ScalarT,Vector4T>
{

    typedef Vector4Implementation<ScalarT,Vector4T> Super;

public:

    inline Vector4FloatImplementation()
    {}

    inline Vector4FloatImplementation( ScalarT _x, ScalarT _y, ScalarT _z=0, ScalarT _w=1 ) : Super(_x,_y,_z,_w) {}
    inline Vector4FloatImplementation( ScalarT _x) : Super(_x) {}
    
    explicit inline Vector4FloatImplementation(const ScalarT coords[4]) : Super(coords) {}
    
    inline Vector4FloatImplementation(const Vector4FloatImplementation& vec) : Super(vec) {}
    
    /** Return true if each component is a valid floating point value.
    */
    inline bool isFinite(void);

    /** overloaded for efficiency.
    */
    inline Vector4T operator / (ScalarT s) const;

    /** Returns the reciprocal of vector length.
        @warning
            This method is for efficiency, calculating the inverse square root directly being more efficient.
    */
    inline ScalarT invLength () const;

    /** Homogeneous normalization.
    */
    inline void hNormalize(void);
    
    /** Returns the (homogeneous) normalized version of the vector.
        \remarks The vector is left unchanged.
        \see hNormalize
    */
    inline Vector4T hNormalized(void) const;
    
    /** Performs a linear interpolation between the two vectors.
        \code
        v0.lerp(t,v1) <=> (1-t)*v0 + t*v1
        \endcode
    */
    inline Vector4T lerp(ScalarT t, const Vector4T v1) const;

    /** Returns true if this vector is zero length.
    */
    inline bool isZero(void) const;
    
    inline static Vector4T Random(void);
    
};

/** 3-dimensional integer point vector
*/
template <typename ScalarT,class Vector4T> class Vector4IntegerImplementation : public Vector4Implementation<ScalarT,Vector4T>
{

    typedef Vector4Implementation<ScalarT,Vector4T> Super;

public:

    inline Vector4IntegerImplementation()
    {}

    inline Vector4IntegerImplementation( ScalarT _x, ScalarT _y, ScalarT _z=0, ScalarT _w=1 ) : Super(_x,_y,_z,_w) {}
    inline Vector4IntegerImplementation( ScalarT _x) : Super(_x) {}
    
    explicit inline Vector4IntegerImplementation(const ScalarT coords[4]) : Super(coords) {}
    
    inline Vector4IntegerImplementation(const Vector4IntegerImplementation& vec) : Super(vec) {}
    
    /** Return true if each component is a valid floating point value.
    */
    inline bool isFinite(void);

    /** Returns true if this vector is zero length.
    */
    inline bool isZero(void) const;
    
};

}

#include "ExpeMath.h"
#include "ExpeVector4Implementation.inl"

#endif
