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



#ifndef _ExpeVector3Implementation_h_
#define _ExpeVector3Implementation_h_

#include "ExpeCore.h"
#include <QDebug>

namespace Expe
{


template <typename ScalarT,class Vector3T> class Vector3Implementation
{
    typedef typename Expe::TypeInfo<ScalarT>::FloatingPointType FloatT;

public:

    ScalarT x, y, z;
    
public:

    inline Vector3Implementation() {}

    inline Vector3Implementation(ScalarT _x, ScalarT _y, ScalarT _z=0)
        : x(_x), y(_y), z(_z)
    {}
    
    inline Vector3Implementation(ScalarT _x)
        : x(_x), y(_x), z(_x)
    {}

    inline Vector3Implementation(const ScalarT* const coords)
        : x(coords[0]), y(coords[1]), z(coords[2])
    {}

    inline Vector3Implementation(const Vector3T& vec)
        : x(vec.x), y(vec.y), z(vec.z)
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
    inline Vector3T& operator = (const Vector3T& vec);
    
    template <int a,int b,int c> inline Vector3T sw (void) const;
    
    template <int a,int b,int c> inline Vector3T operator() (void) const;
    
    
    /** \name Comparison operators
    */
    //@{
    
    inline bool operator == (const Vector3T& vec ) const;
    inline bool operator != (const Vector3T& vec ) const;
    
    /** Returns true if the vector's s components are all greater
        that the ones of the vector it is compared against.
    */
    inline bool operator < ( const Vector3T& vec ) const;

    /** Returns true if the vector's s components are all greater or equal
        that the ones of the vector it is compared against.
    */
    inline bool operator <= ( const Vector3T& vec ) const;

    /** Returns true if the vector's s components are all smaller
        that the ones of the vector it is compared against.
    */
    inline bool operator > ( const Vector3T& vec ) const;

    /** Returns true if the vector's s components are all smaller or equal
        that the ones of the vector it is compared against.
    */
    inline bool operator >= ( const Vector3T& vec ) const;
    
    //@}
    
    
    /** \name Arithmetic vector operations
    */
    //@{
    
    inline Vector3T operator + (const Vector3T& vec) const;
    inline Vector3T& operator += (const Vector3T& vec);
    
    inline Vector3T operator - (const Vector3T& vec) const;
    inline Vector3T& operator -= (const Vector3T& vec);
    
    inline Vector3T operator - () const;
    
    inline Vector3T operator * (const Vector3T& vec) const;
    inline Vector3T& operator *= (const Vector3T& vec);

    inline Vector3T operator / (const Vector3T& v) const;
    inline Vector3T& operator /= (const Vector3T& v);
    
    //@}

    /** \name Arithmetic vector/scalar operations
    */
    //@{
    
    inline Vector3T operator + (ScalarT s) const;
    inline Vector3T& operator += (ScalarT s);
    
    inline Vector3T operator - (ScalarT s) const;
    inline Vector3T& operator -= (ScalarT s);
    
//     inline friend Vector3T operator * (ScalarT s, const Vector3T& v);
    inline friend Vector3T operator * (ScalarT s, const Vector3T& v)
    {
        Vector3T result;
        result.x = s * v.x;
        result.y = s * v.y;
        result.z = s * v.z;
        return result;
    }
    
    inline Vector3T operator * (ScalarT s) const;
    inline Vector3T& operator *= (ScalarT s);
    
    /** Division by a scalar.
        \warning should be overloaded for floating point data.
    */
    inline Vector3T operator / (ScalarT s) const;
    
    /** Division by a scalar.
        \warning should be overloaded for floating point data.
    */
    inline Vector3T& operator /= (ScalarT s);
    
    //@}
    
    /** Returns the length of the vector.
    */
    inline FloatT length () const;

    /** Returns the square of the vector length.
    */
    inline ScalarT squaredLength (void) const;
    
    inline ScalarT squaredDistanceTo(const Vector3T& vec) const;

    inline FloatT distanceTo(const Vector3T& vec) const;
    
    /** Calculates the dot product of this vector with another.
    */
    inline ScalarT dot(const Vector3T& vec) const;
    
    /** Calculates the cross-product of 2 vectors.
    */
    inline Vector3T cross( const Vector3T& vec ) const;
    
    /** Per components sets to minimal value operation.
    */
    inline void makeFloor(const Vector3T& cmp);

    /** Per components sets to maximal value operation.
    */
    inline void makeCeil(const Vector3T& cmp);

    /** Return the maximal component value.
    */
    inline ScalarT maxComponent(void) const;

    /** Return the minimal component value.
    */
    inline ScalarT minComponent(void) const;
    
    /** Return the index of the maximal coordinate value.
    */
    inline uint maxComponentId(void) const;

    /** Return the index of the minimal coordinate value.
    */
    inline uint minComponentId(void) const;
    
    /** Calculates a reflection vector to the plane with the given normal.
    */
    inline Vector3T reflect(const Vector3& normal);
    
};


/** 3-dimensional floating point vector
*/
template <typename ScalarT,class Vector3T> class Vector3FloatImplementation : public Vector3Implementation<ScalarT,Vector3T>
{

    typedef Vector3Implementation<ScalarT,Vector3T> Super;

public:

    inline Vector3FloatImplementation()
    {}

    inline Vector3FloatImplementation( ScalarT _x, ScalarT _y, ScalarT _z=0) : Super(_x,_y,_z) {}
    inline Vector3FloatImplementation( ScalarT _x) : Super(_x) {}
    
    explicit inline Vector3FloatImplementation(const ScalarT coords[3]) : Super(coords) {}
    
    inline Vector3FloatImplementation(const Vector3FloatImplementation& vec) : Super(vec) {}
    
    /** Return true if each component is a valid floating point value.
    */
    inline bool isFinite(void);

    /** overloaded for efficiency.
    */
    inline Vector3T operator / (ScalarT s) const;

    /** Returns the reciprocal of vector length.
        @warning
            This method is for efficiency, calculating the inverse square root directly being more efficient.
    */
    inline ScalarT invLength () const;

    /** Normalizes the vector.
        @remarks
            This method normalises the vector such that it's
            length / magnitude is 1. The result is called a unit vector.
        @note
            This function will not crash for zero-sized vectors, but there
            will be no changes made to their components.
    */
    inline void normalize(void);
    
    /** Returns the normalized version of the vector.
        \remarks The vector is left unchanged.
        \see normalize
    */
    inline Vector3T normalized(void) const;
    
    /** Scales the vector such that its length is l.
        \param l target length
        \remarks the initial vector length must not be null
    */
    inline void scaleTo(ScalarT l);

    /** Returns a vector at a point half way between this and the passed
        in vector.
    */
    inline Vector3T midPoint(const Vector3T& vec) const;

    /** Generates a vector perpendicular to this vector (eg an 'up' vector).
        @remarks
            This method will return a vector which is perpendicular to this
            vector. There are an infinite number of possibilities but this
            method will guarantee to generate one of them. If you need more
            control you should use the Quaternion class.
    */
    inline Vector3T perpendicular(void) const;
    
    /** Performs a linear interpolation between the two vectors.
        \code
        v0.lerp(t,v1) <=> (1-t)*v0 + t*v1
        \endcode
    */
    inline Vector3T lerp(ScalarT t, const Vector3T v1) const;

    /** Generates a new random vector which deviates from this vector by a given angle in a random direction.
        \param
            angle The angle at which to deviate in radians
        \param
            up Any vector perpendicular to this one (which could generated
            by cross-resultuct of this vector and any other non-colinear
            vector). If you choose not to provide this the function will
            derive one on it's own, however if you provide one yourself the
            function will be faster (this allows you to reuse up vectors if
            you call this method more than once)
        \returns
            A random vector which deviates from this vector by angle. This vector will not be normalised.
    */
    inline Vector3T randomDeviant(ScalarT angle, const Vector3T& up = Vector3T::ZERO ) const;

    /** Gets the shortest arc quaternion to rotate this vector to the destination vector.
        \remarks
            Don't call this if you think the dest vector can be close to the inverse
            of this vector, since then ANY axis of rotation is ok.
    */
    inline Quaternion getRotationTo(const Vector3T& dest) const;

    /** Returns true if this vector is zero length.
    */
    inline bool isZero(void) const;
    
    inline static Vector3T Random(void);
    
};

/** 3-dimensional integer point vector
*/
template <typename ScalarT,class Vector3T> class Vector3IntegerImplementation : public Vector3Implementation<ScalarT,Vector3T>
{

    typedef Vector3Implementation<ScalarT,Vector3T> Super;

public:

    inline Vector3IntegerImplementation()
    {}

    inline Vector3IntegerImplementation( ScalarT _x, ScalarT _y, ScalarT _z=0) : Super(_x,_y,_z) {}
    inline Vector3IntegerImplementation( ScalarT _x) : Super(_x) {}
    
    explicit inline Vector3IntegerImplementation(const ScalarT coords[3]) : Super(coords) {}
    
    inline Vector3IntegerImplementation(const Vector3IntegerImplementation& vec) : Super(vec) {}
    
    /** Return true if each component is a valid floating point value.
    */
    inline bool isFinite(void);

    /** Returns true if this vector is zero length.
    */
    inline bool isZero(void) const;
    
};

}

#include "ExpeMath.h"
#include "ExpeQuaternion.h"
#include "ExpeVector3Implementation.inl"

#endif
