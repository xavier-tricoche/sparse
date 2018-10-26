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
template <typename ScalarT,class Vector4T>
inline Vector4Implementation<ScalarT,Vector4T>::operator ScalarT* ()
{
    return &x;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4Implementation<ScalarT,Vector4T>::operator const ScalarT* () const
{
    return &x;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline ScalarT* Vector4Implementation<ScalarT,Vector4T>::data(void)
{
    return &x;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline const ScalarT* Vector4Implementation<ScalarT,Vector4T>::data(void) const
{
    return &x;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T& Vector4Implementation<ScalarT,Vector4T>::operator = (const Vector4T& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;

    return *static_cast<Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
template <int a,int b,int c,int d> inline Vector4T Vector4Implementation<ScalarT,Vector4T>::sw (void) const
{
    return Vector4T(
        reinterpret_cast<const ScalarT*>(this)[a],
        reinterpret_cast<const ScalarT*>(this)[b],
        reinterpret_cast<const ScalarT*>(this)[c],
        reinterpret_cast<const ScalarT*>(this)[d]);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
template <int a,int b,int c,int d> inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator() (void) const
{
    return Vector4T(
        reinterpret_cast<const ScalarT*>(this)[a],
        reinterpret_cast<const ScalarT*>(this)[b],
        reinterpret_cast<const ScalarT*>(this)[c],
        reinterpret_cast<const ScalarT*>(this)[d]);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline bool Vector4Implementation<ScalarT,Vector4T>::operator == (const Vector4T& vec ) const
{
    return ( x==vec.x && y==vec.y && z==vec.z && w==vec.w );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline bool Vector4Implementation<ScalarT,Vector4T>::operator != (const Vector4T& vec ) const
{
    return ( x!=vec.x || y!=vec.y || z!=vec.z || w!=vec.W);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline bool Vector4Implementation<ScalarT,Vector4T>::operator < ( const Vector4T& vec ) const
{
    return ( x < vec.x && y < vec.y && z < vec.z && w < vec.w);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline bool Vector4Implementation<ScalarT,Vector4T>::operator <= ( const Vector4T& vec ) const
{
    return ( x <= vec.x && y <= vec.y && z <= vec.z  && w <= vec.w );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline bool Vector4Implementation<ScalarT,Vector4T>::operator > ( const Vector4T& vec ) const
{
    return ( x > vec.x && y > vec.y && z > vec.z && w > vec.w );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline bool Vector4Implementation<ScalarT,Vector4T>::operator >= ( const Vector4T& vec ) const
{
    return ( x >= vec.x && y >= vec.y && z >= vec.z && w >= vec.w );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator + (ScalarT s) const
{
    Vector4T result;

    result.x = x + s;
    result.y = y + s;
    result.z = z + s;
    result.w = w + s;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator - (ScalarT s) const
{
    Vector4T result;

    result.x = x - s;
    result.y = y - s;
    result.z = z - s;
    result.w = w - s;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator + (const Vector4T& vec) const
{
    Vector4T result;

    result.x = x + vec.x;
    result.y = y + vec.y;
    result.z = z + vec.z;
    result.w = w + vec.w;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator - (const Vector4T& vec) const
{
    Vector4T result;

    result.x = x - vec.x;
    result.y = y - vec.y;
    result.z = z - vec.z;
    result.w = w - vec.w;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator * (ScalarT s) const
{
    Vector4T result;

    result.x = s*x;
    result.y = s*y;
    result.z = s*z;
    result.w = s*w;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator * (const Vector4T& vec) const
{
    Vector4T result;

    result.x = vec.x * x;
    result.y = vec.y * y;
    result.z = vec.z * z;
    result.w = vec.w * w;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator / (ScalarT s) const
{
    assert( s != ScalarT(0) );
    Vector4T result;
    
    result.x = x / s;
    result.y = y / s;
    result.z = z / s;
    result.w = w / s;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator / (const Vector4T& v) const
{
    assert( v.x != ScalarT(0) );
    assert( v.y != ScalarT(0) );
    assert( v.z != ScalarT(0) );
    assert( v.w != ScalarT(0) );

    Vector4T result;
    result.x = x / v.x;
    result.y = y / v.y;
    result.z = z / v.z;
    result.w = w / v.w;
    
    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::operator - () const
{
    return Vector4T(-x, -y, -z, -w);
}
//--------------------------------------------------------------------------------
// template <typename ScalarT,class Vector4T>
// inline friend Vector4T Vector4Implementation<ScalarT,Vector4T>::operator * (ScalarT s, const Vector4T& v)
// {
//     Vector4T result;
// 
//     result.x = s * v.x;
//     result.y = s * v.y;
//     result.z = s * v.z;
//     result.w = s * v.w;
// 
//     return result;
// }
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T& Vector4Implementation<ScalarT,Vector4T>::operator += (ScalarT s)
{
    x += s;
    y += s;
    z += s;
    w += s;

    return *static_cast<Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T& Vector4Implementation<ScalarT,Vector4T>::operator += (const Vector4T& vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    w += vec.w;

    return *static_cast<Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T& Vector4Implementation<ScalarT,Vector4T>::operator -= (ScalarT s)
{
    x -= s;
    y -= s;
    z -= s;
    w -= s;

    return *static_cast<Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T& Vector4Implementation<ScalarT,Vector4T>::operator -= (const Vector4T& vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    w -= vec.w;

    return *static_cast<Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T& Vector4Implementation<ScalarT,Vector4T>::operator *= (ScalarT s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= s;

    return *static_cast<Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T& Vector4Implementation<ScalarT,Vector4T>::operator *= (const Vector4T& vec)
{
    x *= vec.x;
    y *= vec.y;
    z *= vec.z;
    w *= vec.w;

    return *static_cast<Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T& Vector4Implementation<ScalarT,Vector4T>::operator /= (ScalarT s)
{
    assert( s != ScalarT(0) );

    x /= s;
    y /= s;
    z /= s;
    w /= s;

    return *static_cast<Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T& Vector4Implementation<ScalarT,Vector4T>::operator /= (const Vector4T& v)
{
    assert( v.x != ScalarT(0) );
    assert( v.y != ScalarT(0) );
    assert( v.z != ScalarT(0) );
    assert( v.w != ScalarT(0) );

    x /= v.x;
    y /= v.y;
    z /= v.z;
    w /= v.w;

    return *static_cast<Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline typename Vector4Implementation<ScalarT,Vector4T>::FloatT Vector4Implementation<ScalarT,Vector4T>::length () const
{
    return Math::Sqrt(FloatT(x*x + y*y + z*z + w*w));
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline ScalarT Vector4Implementation<ScalarT,Vector4T>::squaredLength (void) const
{
    return x*x + y*y + z*z + w*w;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline ScalarT Vector4Implementation<ScalarT,Vector4T>::squaredDistanceTo(const Vector4T& vec) const
{
    ScalarT dx,dy,dz,dw;
    dx = x-vec.x;
    dy = y-vec.y;
    dz = z-vec.z;
    dw = w-vec.w;
    return dx*dx + dy*dy + dz*dz + dw*dw;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline typename Vector4Implementation<ScalarT,Vector4T>::FloatT Vector4Implementation<ScalarT,Vector4T>::distanceTo(const Vector4T& vec) const
{
    ScalarT dx,dy,dz,dw;
    dx = x-vec.x;
    dy = y-vec.y;
    dz = z-vec.z;
    dw = w-vec.w;
    return Math::Sqrt(FloatT(dx*dx + dy*dy + dz*dz + dw*dw));
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline ScalarT Vector4Implementation<ScalarT,Vector4T>::dot(const Vector4T& vec) const
{
    return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline ScalarT Vector4Implementation<ScalarT,Vector4T>::dot3(const Vector4T& vec) const
{
    return x*vec.x + y*vec.y + z*vec.z;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4Implementation<ScalarT,Vector4T>::cross3( const Vector4T& vec ) const
{
    return Vector4T(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline void Vector4Implementation<ScalarT,Vector4T>::makeFloor(const Vector4T& cmp)
{
    if( cmp.x < x ) x = cmp.x;
    if( cmp.y < y ) y = cmp.y;
    if( cmp.z < z ) z = cmp.z;
    if( cmp.w < w ) w = cmp.w;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline void Vector4Implementation<ScalarT,Vector4T>::makeCeil(const Vector4T& cmp)
{
    if( cmp.x > x ) x = cmp.x;
    if( cmp.y > y ) y = cmp.y;
    if( cmp.z > z ) z = cmp.z;
    if( cmp.w > w ) w = cmp.w;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline ScalarT Vector4Implementation<ScalarT,Vector4T>::maxComponent(void) const
{
    return Math::Max( Math::Max(x,y), Math::Max(z,w) );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline ScalarT Vector4Implementation<ScalarT,Vector4T>::minComponent(void) const
{
    return Math::Min( Math::Min(x,y), Math::Min(z,w) );
}

//--------------------------------------------------------------------------------
// End of Vector4Implementation
//--------------------------------------------------------------------------------







//--------------------------------------------------------------------------------
// Begining of Vector4FloatImplementation
//--------------------------------------------------------------------------------

template <typename ScalarT,class Vector4T>
inline bool Vector4FloatImplementation<ScalarT,Vector4T>::isFinite(void)
{
    return Math::isFinite(this->x) && Math::isFinite(this->y) && Math::isFinite(this->z) && Math::isFinite(this->w);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4FloatImplementation<ScalarT,Vector4T>::operator / (ScalarT s) const
{
    assert( s != ScalarT(0.0) );
    Vector4T result;

    ScalarT inv = ScalarT(1.0) / s;
    result.x = this->x * inv;
    result.y = this->y * inv;
    result.z = this->z * inv;
    result.w = this->w * inv;

    return result;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline ScalarT Vector4FloatImplementation<ScalarT,Vector4T>::invLength () const
{
    return Math::InvSqrt( this->x*this->x + this->y*this->y + this->z*this->z + this->w*this->w );
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline void Vector4FloatImplementation<ScalarT,Vector4T>::hNormalize(void)
{
    ScalarT sqlength = this->x*this->x + this->y*this->y + this->z*this->z;

    if(sqlength > ScalarT(1e-12))
    {
        ScalarT invLength = Math::InvSqrt(sqlength) * this->w;
        this->x *= invLength;
        this->y *= invLength;
        this->z *= invLength;
        this->w = 1.;
    }
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4FloatImplementation<ScalarT,Vector4T>::hNormalized(void) const
{
    ScalarT sqlength = this->x*this->x + this->y*this->y + this->z*this->z;

    if(sqlength > ScalarT(1e-12))
    {
        ScalarT invLength = Math::InvSqrt(sqlength) * this->w;
        return Vector4T(this->x*invLength,this->y*invLength,this->z*invLength);
    }
    
    return *static_cast<const Vector4T*>(this);
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4FloatImplementation<ScalarT,Vector4T>::lerp(ScalarT t, const Vector4T v1) const
{
    Vector4T vAux;
    ScalarT t1 = 1.-t;
    vAux.x = t1*this->x + t*v1.x;
    vAux.y = t1*this->y + t*v1.y;
    vAux.z = t1*this->z + t*v1.z;
    vAux.w = t1*this->w + t*v1.w;
    return vAux;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline bool Vector4FloatImplementation<ScalarT,Vector4T>::isZero(void) const
{
    ScalarT sqlen = this->x*this->x + this->y*this->y + this->z*this->z + this->w*this->w;
    return (sqlen < (1e-06 * 1e-06));

}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline Vector4T Vector4FloatImplementation<ScalarT,Vector4T>::Random(void)
{
    return Vector4T(Math::Rand<ScalarT>(),Math::Rand<ScalarT>(),Math::Rand<ScalarT>(),Math::Rand<ScalarT>());
}

//--------------------------------------------------------------------------------
// End of Vector4FloatImplementation
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
// Begining of Vector4IntegerImplementation
//--------------------------------------------------------------------------------

template <typename ScalarT,class Vector4T>
inline bool Vector4IntegerImplementation<ScalarT,Vector4T>::isFinite(void)
{
    return true;
}
//--------------------------------------------------------------------------------
template <typename ScalarT,class Vector4T>
inline bool Vector4IntegerImplementation<ScalarT,Vector4T>::isZero(void) const
{
    return this->x==0 && this->y==0 && this->z==0 && this->w==0;
}
//--------------------------------------------------------------------------------

}

