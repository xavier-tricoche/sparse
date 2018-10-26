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



#ifndef _ExpeVectorPrerequisites_h_
#define _ExpeVectorPrerequisites_h_

#include "ExpePrerequisites.h"
#include "ExpeMetaProgramming.h"

namespace Expe
{

/** Template structure allowing to get the vector type of size N and with element type BaseType.
    For instance:
    \code
    GetVector<3,float>::Type is equivalent to Vector3f.
    \endcode
    In combination with the TypeInfo<> framework it is possible to, for instance,
    get the vector type with integer elements and one more element than a given
    vector type:
    \code
    typedef Vector2d MyVectorType;
    typedef GetVector<
        TypeInfo<MyVectorType>::NofComponents+1, // size
        TypeInfo<TypeInfo<MyVectorType>::ComponentType>::IntegerType
        >::Type MyNewVectorType; // equivalent to Vector3i64
    \endcode
*/
template <int N, typename BaseType> struct GetVector
{
    #ifdef DOXYGEN
    typedef _GenericVector_ Type;
    #endif
};

#ifndef DOXYGEN
//--------------------------------------------------------------------------------
class Vector2f;
class Vector2d;
class Vector2i;
class Vector2i64;

template <> struct GetVector<2,float>  { typedef Vector2f   Type; };
template <> struct GetVector<2,double> { typedef Vector2d   Type; };
template <> struct GetVector<2,int>    { typedef Vector2i   Type; };
template <> struct GetVector<2,int64>  { typedef Vector2i64 Type; };

/// Default 2-dimensional vector.
typedef GetVector<2,Real>::Type Vector2;
typedef std::vector<Vector2> Vector2Array;


//--------------------------------------------------------------------------------
class Vector3f;
class Vector3d;
class Vector3i;
class Vector3i64;

template <> struct GetVector<3,float>  { typedef Vector3f   Type; };
template <> struct GetVector<3,double> { typedef Vector3d   Type; };
template <> struct GetVector<3,int>    { typedef Vector3i   Type; };
template <> struct GetVector<3,int64>  { typedef Vector3i64 Type; };

/// Default 3-dimensional vector.
typedef GetVector<3,Real>::Type Vector3;
typedef std::vector<Vector3> Vector3Array;


//--------------------------------------------------------------------------------
class Vector4f;
class Vector4d;
class Vector4i;
class Vector4i64;

template <> struct GetVector<4,float>  { typedef Vector4f   Type; };
template <> struct GetVector<4,double> { typedef Vector4d   Type; };
template <> struct GetVector<4,int>    { typedef Vector4i   Type; };
template <> struct GetVector<4,int64>  { typedef Vector4i64 Type; };

/// Default 4-dimensional vector.
typedef GetVector<4,Real>::Type Vector4;

#if (GCC_VERSION>=40) && defined (EXPE_USE_SSE)
// Force 128 bits alignment for SSE operations
typedef std::vector<Vector4,__gnu_cxx::malloc_allocator<Vector4> > Vector4Array;
#else
typedef std::vector<Vector4> Vector4Array;
#endif

#endif // doxygen

//--------------------------------------------------------------------------------
// vector cast
//--------------------------------------------------------------------------------

#ifndef DOXYGEN

/** C++ does not allow partial template function specialization, hence the use of a structure.
    The parameters inputLargerThanOutput and sameType are used to avoid
    ambiguous class template instantiation.

    Currently the convertion between base types is let to the compiler. Then some warnings may arise.
    This could be fixed by adding some static_cast (and using our TypeInfo<> framework), however, come
    vector class may want to handle this kind of conversion itself. For instance, a floating point Color
    class may want to cast ubyte to float by normalizing the value between 0 and 1. So, keeping the warnings
    is probably a better solution. TODO: an alternative would be to specialize vector_cast_impl<> to explicitly handle
    such cases...
*/
template <int ToN, int FromN, bool inputLargerThanOutput, typename ToT,typename FromT,bool sameType> struct vector_cast_impl
{
    static inline ToT cast(const FromT& valueIn)
    {
        return ToT::WRONG_VECTOR_CAST;
    }
};

// trivial case
template <int N, typename T> struct vector_cast_impl<N,N,false,T,T,true>
{
    static inline const T& cast(const T& valueIn) {return valueIn;}
};

// N to 1 (N!=1)
template <int N, typename ToT, typename FromT> struct vector_cast_impl<1,N,true,ToT,FromT,false>
{
    static inline ToT cast(const FromT& valueIn)
    {
        static_assert( TypeInfo<ToT>::NofComponents==1, "Only 1 component is admissible with a 1-vector" );
        return valueIn[0];
    }
};

// N>2 to 2
template <int N, typename ToT, typename FromT> struct vector_cast_impl<2,N,true,ToT,FromT,false>
{
    static inline ToT cast(const FromT& valueIn)
    {
        static_assert( TypeInfo<ToT>::NofComponents>=2, "Only 2 components are admissible with a 2-vector" );
        return ToT(valueIn[0],valueIn[1]);
    }
};

// N>3 to 3
template <int N, typename ToT, typename FromT> struct vector_cast_impl<3,N,true,ToT,FromT,false>
{
    static inline ToT cast(const FromT& valueIn)
    {
        static_assert( TypeInfo<ToT>::NofComponents>=3, "Only 3 components are admissible with a 3-vector" );
        return ToT(valueIn[0],valueIn[1],valueIn[2]);
    }
};

// 1 to N
template <int N, typename ToT, typename FromT> struct vector_cast_impl<N,1,false,ToT,FromT,false>
{
    static inline ToT cast(const FromT& valueIn)
    {
        static_assert( TypeInfo<FromT>::NofComponents==1, "Only 1 component is admissible with a 1-vector" );
        return ToT(valueIn);
    }
};

// 2 to N>=2
template <int N, typename ToT, typename FromT> struct vector_cast_impl<N,2,false,ToT,FromT,false>
{
    static inline ToT cast(const FromT& valueIn)
    {
        static_assert( TypeInfo<FromT>::NofComponents==2, "Only 2 components are admissible with a 2-vector" );
        return ToT(valueIn[0],valueIn[1]);
    }
};

// 3 to N>=3
template <int N, typename ToT, typename FromT> struct vector_cast_impl<N,3,false,ToT,FromT,false>
{
    static inline ToT cast(const FromT& valueIn)
    {
        static_assert( TypeInfo<FromT>::NofComponents==3, "Only 3 components are admissible with a 3-vector" );
        return ToT(valueIn[0],valueIn[1],valueIn[2]);
    }
};

// 4 to 4
template <typename ToT, typename FromT> struct vector_cast_impl<4,4,false,ToT,FromT,false>
{
    static inline ToT cast(const FromT& valueIn)
    {
        static_assert( TypeInfo<FromT>::NofComponents==4, "Only 4 components are admissible with a 4-vector" );
        static_assert( TypeInfo<ToT>::NofComponents==4, "Only 4 components are admissible with a 4-vector" );
        return ToT(valueIn[0],valueIn[1],valueIn[2],valueIn[4]);
    }
};

#endif // doxygen

/** Cast operator for casting between any vector type (including the Color class)
*/
template <typename ToT,typename FromT> inline
typename MTP::IF< MTP::compare_type<ToT,FromT>::result , const ToT& , ToT >::RET
vector_cast(const FromT& valueIn)
{
    return vector_cast_impl< TypeInfo<ToT>::NofComponents, TypeInfo<FromT>::NofComponents,
        (int(TypeInfo<FromT>::NofComponents)>int(TypeInfo<ToT>::NofComponents)),
        ToT,FromT,MTP::compare_type<ToT,FromT>::result>::cast(valueIn);
}

}

#endif
