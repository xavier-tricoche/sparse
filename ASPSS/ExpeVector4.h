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



#ifndef _ExpeVector4_h_
#define _ExpeVector4_h_

#include <iostream>

#include "ExpeCore.h"
#include "ExpeVector4Implementation.h"

namespace Expe
{

#define EXPE_DECLARE_VECTOR4(SCALARTYPE,BASE,SUFFIX) \
    class Vector ## SUFFIX : public BASE<SCALARTYPE,Vector ## SUFFIX> \
    { \
        typedef BASE<SCALARTYPE,Vector ## SUFFIX> Super; \
    public: \
        inline Vector ## SUFFIX() {} \
        inline Vector ## SUFFIX(SCALARTYPE _x, SCALARTYPE _y, SCALARTYPE _z=0, SCALARTYPE _w=1) : Super(_x,_y,_z, _w) {} \
        inline Vector ## SUFFIX(SCALARTYPE _x) : Super(_x) {} \
        explicit inline Vector ## SUFFIX(const SCALARTYPE coords[4]) : Super(coords) {} \
        inline Vector ## SUFFIX(const Vector ## SUFFIX& vec) : Super(vec) {} \
    \
        inline friend std::ostream& operator << (std::ostream& o, const Vector ## SUFFIX& v) { \
            o << "Vector" << # SUFFIX << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')'; \
            return o; \
        } \
        inline friend QDebug operator<<(QDebug dbg, const Vector ## SUFFIX& v) { \
            dbg.nospace() << "Vector" << # SUFFIX << '(' << v.x << ',' << v.y << ',' << v.z <<  ',' << v.w << ')'; \
            return dbg.space(); \
        } \
    \
        static const Vector ## SUFFIX ZERO;         \
        static const Vector ## SUFFIX UNIT_X;       \
        static const Vector ## SUFFIX UNIT_Y;       \
        static const Vector ## SUFFIX UNIT_Z;       \
        static const Vector ## SUFFIX UNIT_SCALE;   \
    }; \
    \
    template <> struct TypeInfo<Vector##SUFFIX> { \
        enum {NofComponents=4}; \
        typedef SCALARTYPE ComponentType; \
        typedef GetVector<NofComponents,TypeInfo<ComponentType>::IntegerType>::Type          IntegerType; \
        typedef GetVector<NofComponents,TypeInfo<ComponentType>::FloatingPointType>::Type    FloatingPointType; \
        static_assert( (MTP::compare_type<Vector##SUFFIX,FloatingPointType>::result) \
            || (MTP::compare_type<Vector##SUFFIX,IntegerType>::result), "vector value type must be integral or float number" ); \
    };

EXPE_DECLARE_VECTOR4(float,Vector4FloatImplementation,4f);
EXPE_DECLARE_VECTOR4(double,Vector4FloatImplementation,4d);
EXPE_DECLARE_VECTOR4(int,Vector4IntegerImplementation,4i);
EXPE_DECLARE_VECTOR4(int64,Vector4IntegerImplementation,4i64);

/*

template <>
struct TypeInfo<Vector4f>
{
    // FIXME Id
    enum {NofComponents=4};
    typedef float       ComponentType;
    typedef Vector4i    IntegerType;
    typedef Vector4f    FloatingPointType;
};


template <>
struct TypeInfo<Vector4d>
{
    // FIXME Id
    enum {NofComponents=4};
    typedef double      ComponentType;
    typedef Vector4i64  IntegerType;
    typedef Vector4d    FloatingPointType;
};


template <>
struct TypeInfo<Vector4i>
{
    // FIXME Id
    enum {NofComponents=4};
    typedef int         ComponentType;
    typedef Vector4i    IntegerType;
    typedef Vector4f    FloatingPointType;
};

template <>
struct TypeInfo<Vector4i64>
{
    // FIXME Id
    enum {NofComponents=4};
    typedef int64       ComponentType;
    typedef Vector4i64  IntegerType;
    typedef Vector4d    FloatingPointType;
};
*/

}

#include <qmetatype.h>
Q_DECLARE_METATYPE(Expe::Vector4f);
Q_DECLARE_METATYPE(Expe::Vector4d);
Q_DECLARE_METATYPE(Expe::Vector4i);
Q_DECLARE_METATYPE(Expe::Vector4i64);

#endif
