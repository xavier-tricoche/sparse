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




namespace Expe
{


//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3Implementation<ScalarT,Vector3T>::operator ScalarT* ()
{
    return &x;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3Implementation<ScalarT,Vector3T>::operator const ScalarT* () const
{
    return &x;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline ScalarT* Vector3Implementation<ScalarT,Vector3T>::data(void)
{
    return &x;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline const ScalarT* Vector3Implementation<ScalarT,Vector3T>::data(void) const
{
    return &x;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T& Vector3Implementation<ScalarT,Vector3T>::operator = (const Vector3T& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;

    return *static_cast<Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
template <int a,int b,int c> inline Vector3T Vector3Implementation<ScalarT,Vector3T>::sw (void) const
{
    return Vector3T(
        reinterpret_cast<const ScalarT*>(this)[a],
        reinterpret_cast<const ScalarT*>(this)[b],
        reinterpret_cast<const ScalarT*>(this)[c]);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
template <int a,int b,int c> inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator() (void) const
{
    return Vector3T(
        reinterpret_cast<const ScalarT*>(this)[a],
        reinterpret_cast<const ScalarT*>(this)[b],
        reinterpret_cast<const ScalarT*>(this)[c]);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline bool Vector3Implementation<ScalarT,Vector3T>::operator == (const Vector3T& vec ) const
{
    return ( x == vec.x && y == vec.y && z == vec.z );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline bool Vector3Implementation<ScalarT,Vector3T>::operator != (const Vector3T& vec ) const
{
    return ( x != vec.x || y != vec.y || z != vec.z );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline bool Vector3Implementation<ScalarT,Vector3T>::operator < ( const Vector3T& vec ) const
{
    if( x < vec.x && y < vec.y && z < vec.z )
        return true;
    return false;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline bool Vector3Implementation<ScalarT,Vector3T>::operator <= ( const Vector3T& vec ) const
{
    if( x <= vec.x && y <= vec.y && z <= vec.z )
        return true;
    return false;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline bool Vector3Implementation<ScalarT,Vector3T>::operator > ( const Vector3T& vec ) const
{
    if( x > vec.x && y > vec.y && z > vec.z )
        return true;
    return false;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline bool Vector3Implementation<ScalarT,Vector3T>::operator >= ( const Vector3T& vec ) const
{
    if( x >= vec.x && y >= vec.y && z >= vec.z )
        return true;
    return false;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator + (ScalarT s) const
{
    Vector3T result;

    result.x = x + s;
    result.y = y + s;
    result.z = z + s;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator - (ScalarT s) const
{
    Vector3T result;

    result.x = x - s;
    result.y = y - s;
    result.z = z - s;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator + (const Vector3T& vec) const
{
    Vector3T result;

    result.x = x + vec.x;
    result.y = y + vec.y;
    result.z = z + vec.z;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator - (const Vector3T& vec) const
{
    Vector3T result;

    result.x = x - vec.x;
    result.y = y - vec.y;
    result.z = z - vec.z;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator * (ScalarT s) const
{
    Vector3T result;

    result.x = s*x;
    result.y = s*y;
    result.z = s*z;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator * (const Vector3T& vec) const
{
    Vector3T result;

    result.x = vec.x * x;
    result.y = vec.y * y;
    result.z = vec.z * z;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator / (ScalarT s) const
{
    assert( s != ScalarT(0) );
    Vector3T result;
    
    result.x = x / s;
    result.y = y / s;
    result.z = z / s;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator / (const Vector3T& v) const
{
    assert( v.x != ScalarT(0) );
    assert( v.y != ScalarT(0) );
    assert( v.z != ScalarT(0) );

    Vector3T result;
    result.x = x / v.x;
    result.y = y / v.y;
    result.z = z / v.z;
    
    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::operator - () const
{
    return Vector3T(-x, -y, -z);
}
//--------------------------------------------------------------------------------
// template <typename ScalarT,class Vector3T>
// inline friend Vector3T Vector3Implementation<ScalarT,Vector3T>::operator * (ScalarT s, const Vector3T& v)
// {
//     Vector3T result;
// 
//     result.x = s * v.x;
//     result.y = s * v.y;
//     result.z = s * v.z;
// 
//     return result;
// }
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T& Vector3Implementation<ScalarT,Vector3T>::operator += (ScalarT s)
{
    x += s;
    y += s;
    z += s;

    return *static_cast<Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T& Vector3Implementation<ScalarT,Vector3T>::operator += (const Vector3T& vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;

    return *static_cast<Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T& Vector3Implementation<ScalarT,Vector3T>::operator -= (ScalarT s)
{
    x -= s;
    y -= s;
    z -= s;

    return *static_cast<Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T& Vector3Implementation<ScalarT,Vector3T>::operator -= (const Vector3T& vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;

    return *static_cast<Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T& Vector3Implementation<ScalarT,Vector3T>::operator *= (ScalarT s)
{
    x *= s;
    y *= s;
    z *= s;
    return *static_cast<Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T& Vector3Implementation<ScalarT,Vector3T>::operator *= (const Vector3T& vec)
{
    x *= vec.x;
    y *= vec.y;
    z *= vec.z;

    return *static_cast<Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T& Vector3Implementation<ScalarT,Vector3T>::operator /= (ScalarT s)
{
    assert( s != ScalarT(0) );

    x /= s;
    y /= s;
    z /= s;

    return *static_cast<Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T& Vector3Implementation<ScalarT,Vector3T>::operator /= (const Vector3T& v)
{
    assert( v.x != ScalarT(0) );
    assert( v.y != ScalarT(0) );
    assert( v.z != ScalarT(0) );

    x /= v.x;
    y /= v.y;
    z /= v.z;

    return *static_cast<Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline typename Vector3Implementation<ScalarT,Vector3T>::FloatT Vector3Implementation<ScalarT,Vector3T>::length () const
{
    return Math::Sqrt(FloatT(x*x + y*y + z*z));
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline ScalarT Vector3Implementation<ScalarT,Vector3T>::squaredLength (void) const
{
    return x*x + y*y + z*z;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline ScalarT Vector3Implementation<ScalarT,Vector3T>::squaredDistanceTo(const Vector3T& vec) const
{
    ScalarT dx,dy,dz;
    dx = x-vec.x;
    dy = y-vec.y;
    dz = z-vec.z;
    return dx*dx + dy*dy + dz*dz;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline typename Vector3Implementation<ScalarT,Vector3T>::FloatT Vector3Implementation<ScalarT,Vector3T>::distanceTo(const Vector3T& vec) const
{
    ScalarT dx,dy,dz;
    dx = x-vec.x;
    dy = y-vec.y;
    dz = z-vec.z;
    return Math::Sqrt(FloatT(dx*dx + dy*dy + dz*dz));
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline ScalarT Vector3Implementation<ScalarT,Vector3T>::dot(const Vector3T& vec) const
{
    return x*vec.x + y*vec.y + z*vec.z;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::cross( const Vector3T& vec ) const
{
    return Vector3T(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline void Vector3Implementation<ScalarT,Vector3T>::makeFloor(const Vector3T& cmp)
{
    if( cmp.x < x ) x = cmp.x;
    if( cmp.y < y ) y = cmp.y;
    if( cmp.z < z ) z = cmp.z;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline void Vector3Implementation<ScalarT,Vector3T>::makeCeil(const Vector3T& cmp)
{
    if( cmp.x > x ) x = cmp.x;
    if( cmp.y > y ) y = cmp.y;
    if( cmp.z > z ) z = cmp.z;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline ScalarT Vector3Implementation<ScalarT,Vector3T>::maxComponent(void) const
{
    return Math::Max(x,y,z);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline ScalarT Vector3Implementation<ScalarT,Vector3T>::minComponent(void) const
{
    return Math::Min(x,y,z);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline uint Vector3Implementation<ScalarT,Vector3T>::maxComponentId(void) const
{
    if (x<y)
        return (y<z ? Z : Y);
    else
        return (x<z ? Z : X);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline uint Vector3Implementation<ScalarT,Vector3T>::minComponentId(void) const
{
    if (x<y)
        return (x<z ? X : Z);
    else
        return (y<Z ? Y : Z);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3Implementation<ScalarT,Vector3T>::reflect(const Vector3& normal)
{
    return Vector3T( ( ScalarT(2) * this->dot(normal) * normal ) - *this );
}

//--------------------------------------------------------------------------------
// End of Vector3Implementation
//--------------------------------------------------------------------------------







//--------------------------------------------------------------------------------
// Begining of Vector3FloatImplementation
//--------------------------------------------------------------------------------

template <typename ScalarT,class Vector3T>
inline bool Vector3FloatImplementation<ScalarT,Vector3T>::isFinite(void)
{
    return Math::isFinite(this->x) && Math::isFinite(this->y) && Math::isFinite(this->z);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3FloatImplementation<ScalarT,Vector3T>::operator / (ScalarT s) const
{
    assert( s != ScalarT(0.0) );
    Vector3T result;

    ScalarT inv = ScalarT(1.0) / s;
    result.x = this->x * inv;
    result.y = this->y * inv;
    result.z = this->z * inv;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline ScalarT Vector3FloatImplementation<ScalarT,Vector3T>::invLength () const
{
    return Math::InvSqrt( this->x*this->x + this->y*this->y + this->z*this->z );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline void Vector3FloatImplementation<ScalarT,Vector3T>::normalize(void)
{
    ScalarT sqlength = this->x*this->x + this->y*this->y + this->z*this->z;

    if(sqlength > ScalarT(1e-12))
    {
        ScalarT invLength = Math::InvSqrt(sqlength);
        this->x *= invLength;
        this->y *= invLength;
        this->z *= invLength;
    }
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3FloatImplementation<ScalarT,Vector3T>::normalized(void) const
{
    ScalarT sqlength = this->x*this->x + this->y*this->y + this->z*this->z;

    if(sqlength > ScalarT(1e-12))
    {
        ScalarT invLength = Math::InvSqrt(sqlength);
        return Vector3T(this->x*invLength,this->y*invLength,this->z*invLength);
    }
    
    return *static_cast<const Vector3T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline void Vector3FloatImplementation<ScalarT,Vector3T>::scaleTo(ScalarT l)
{
    assert(this->squaredLength()>1e-12);
    
    ScalarT invLength = Math::InvSqrt( this->x*this->x + this->y*this->y + this->z*this->z ) * l;
    this->x *= invLength;
    this->y *= invLength;
    this->z *= invLength;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3FloatImplementation<ScalarT,Vector3T>::midPoint(const Vector3T& vec) const
{
    return Vector3T(
        ( this->x + vec.x ) * 0.5,
        ( this->y + vec.y ) * 0.5,
        ( this->z + vec.z ) * 0.5 );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3FloatImplementation<ScalarT,Vector3T>::perpendicular(void) const
{
    static const ScalarT squareZero = 1e-06 * 1e-06;

    Vector3T perp = this->cross( Vector3T::UNIT_X );

    // Check length
    if (perp.squaredLength() < squareZero)
    {
        // This vector is the Y axis multiplied by a s, so we have to use another axis.
        perp = this->cross( Vector3T::UNIT_Y );
    }

    return perp;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3FloatImplementation<ScalarT,Vector3T>::lerp(ScalarT t, const Vector3T v1) const
{
    Vector3T vAux;
    ScalarT t1 = 1.-t;
    vAux.x = t1*this->x + t*v1.x;
    vAux.y = t1*this->y + t*v1.y;
    vAux.z = t1*this->z + t*v1.z;
    return vAux;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3FloatImplementation<ScalarT,Vector3T>::randomDeviant(ScalarT angle, const Vector3T& up /*= Vector3T::ZERO*/ ) const
{
    Vector3T newUp;

    if (up == Vector3T::ZERO)
    {
        // Generate an up vector
        newUp = this->perpendicular();
    }
    else
    {
        newUp = up;
    }

    // Rotate up vector by random amount around this
    Quaternion q;
    q.fromAngleAxis( Math::Rand<ScalarT>()*Math::TWO_PI, *this );
    newUp = q * newUp;

    // Finally rotate this by given angle around randomised up
    q.fromAngleAxis(angle, newUp);
    return q * (*this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Quaternion Vector3FloatImplementation<ScalarT,Vector3T>::getRotationTo(const Vector3T& dest) const
{
    // Based on Stan Melax's article in Game Programming Gems
    Quaternion q;
    // Copy, since cannot modify local
    Vector3T v0 = *this;
    Vector3T v1 = dest;
    v0.normalize();
    v1.normalize();

    Vector3T c = v0.cross(v1);

    // NB if the crossProduct approaches zero, we get unstable because ANY axis will do
    // when v0 == -v1
    ScalarT d = v0.dot(v1);
    // If dot == 1, vectors are the same
    if (d >= 1.0f)
    {
        return Quaternion::IDENTITY;
    }
    ScalarT s = Math::Sqrt( (1+d)*2 );
    ScalarT invs = 1 / s;


    q.x = c.x * invs;
    q.y = c.y * invs;
    q.z = c.z * invs;
    q.w = s * 0.5;
    return q;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline bool Vector3FloatImplementation<ScalarT,Vector3T>::isZero(void) const
{
    ScalarT sqlen = this->x*this->x + this->y*this->y + this->z*this->z;
    return (sqlen < (1e-06 * 1e-06));

}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline Vector3T Vector3FloatImplementation<ScalarT,Vector3T>::Random(void)
{
    return Vector3T(Math::Rand<ScalarT>(),Math::Rand<ScalarT>(),Math::Rand<ScalarT>());
}

//--------------------------------------------------------------------------------
// End of Vector3FloatImplementation
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
// Begining of Vector3IntegerImplementation
//--------------------------------------------------------------------------------

template <typename ScalarT,class Vector3T>
inline bool Vector3IntegerImplementation<ScalarT,Vector3T>::isFinite(void)
{
    return true;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector3T>
inline bool Vector3IntegerImplementation<ScalarT,Vector3T>::isZero(void) const
{
    return this->x==0 && this->y==0 && this->z==0;
}
//--------------------------------------------------------------------------------

}

