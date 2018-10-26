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



#ifndef _ExpeVector2_h_
#define _ExpeVector2_h_

#include <iostream>

#include "ExpeCore.h"
#include "ExpeMath.h"
#include <QtDebug>
#include "ExpeVector2Implementation.h"

namespace Expe
{

#define EXPE_DECLARE_VECTOR2(SCALARTYPE,BASE,SUFFIX) \
    class Vector ## SUFFIX : public BASE<SCALARTYPE,Vector ## SUFFIX> \
    { \
        typedef BASE<SCALARTYPE,Vector ## SUFFIX> Super; \
    public: \
        inline Vector ## SUFFIX() {} \
        inline Vector ## SUFFIX(SCALARTYPE _x, SCALARTYPE _y) : Super(_x,_y) {} \
        inline Vector ## SUFFIX(SCALARTYPE _x) : Super(_x) {} \
        explicit inline Vector ## SUFFIX(const SCALARTYPE coords[2]) : Super(coords) {} \
        inline Vector ## SUFFIX(const Vector ## SUFFIX& vec) : Super(vec) {} \
    \
        inline friend std::ostream& operator << (std::ostream& o, const Vector ## SUFFIX& v) { \
            o << "Vector" << # SUFFIX << '(' << v.x << ',' << v.y << ')'; \
            return o; \
        } \
        inline friend QDebug operator<<(QDebug dbg, const Vector ## SUFFIX& v) { \
            dbg.nospace() << "Vector" << # SUFFIX << '(' << v.x << ',' << v.y << ')'; \
            return dbg.space(); \
        } \
    \
        static const Vector ## SUFFIX ZERO;         \
        static const Vector ## SUFFIX UNIT_X;       \
        static const Vector ## SUFFIX UNIT_Y;       \
        static const Vector ## SUFFIX UNIT_SCALE;   \
    }; \
    \
    template <> struct TypeInfo<Vector##SUFFIX> { \
        enum {NofComponents=2}; \
        typedef SCALARTYPE ComponentType; \
        typedef GetVector<NofComponents,TypeInfo<ComponentType>::IntegerType>::Type          IntegerType; \
        typedef GetVector<NofComponents,TypeInfo<ComponentType>::FloatingPointType>::Type    FloatingPointType; \
        static_assert( (MTP::compare_type<Vector##SUFFIX,FloatingPointType>::result) \
            || (MTP::compare_type<Vector##SUFFIX,IntegerType>::result), "vector value type must be integral or float number" ); \
    };

EXPE_DECLARE_VECTOR2(float,Vector2FloatImplementation,2f);
EXPE_DECLARE_VECTOR2(double,Vector2FloatImplementation,2d);
EXPE_DECLARE_VECTOR2(int,Vector2IntegerImplementation,2i);
EXPE_DECLARE_VECTOR2(int64,Vector2IntegerImplementation,2i64);


/*
template <>
struct TypeInfo<Vector2f>
{
    enum {NofComponents=2};
    typedef float        ComponentType;
    typedef Vector2i    IntegerType;
    typedef Vector2f    FloatingPointType;
};


template <>
struct TypeInfo<Vector2d>
{
    enum {NofComponents=2};
    typedef double      ComponentType;
    typedef Vector2i64  IntegerType;
    typedef Vector2d    FloatingPointType;
};


template <>
struct TypeInfo<Vector2i>
{
    enum {NofComponents=2};
    typedef int         ComponentType;
    typedef Vector2i    IntegerType;
    typedef Vector2f    FloatingPointType;
};
*/

//--------------------------------------------------------------------------------
// Serialization
//--------------------------------------------------------------------------------

/** \relates Vector2
    Writes the given \a vector to the given \a stream, and returns a reference to the stream.
*/
QDataStream& operator<<(QDataStream& stream, const Vector2& vec);

/** \relates Vector2
    Reads a vector3 from the given \a stream into the given \a vector, and returns a reference to the stream.
*/
QDataStream& operator>>(QDataStream& stream, Vector2& vector);

/** \relates Vector2i
    Writes the given \a vector to the given \a stream, and returns a reference to the stream.
*/
QDataStream& operator<<(QDataStream& stream, const Vector2i& vec);

/** \relates Vector2i
    Reads a vector3 from the given \a stream into the given \a vector, and returns a reference to the stream.
*/
QDataStream& operator>>(QDataStream& stream, Vector2i& vector);


}

#include <qmetatype.h>
Q_DECLARE_METATYPE(Expe::Vector2f);
Q_DECLARE_METATYPE(Expe::Vector2d);
Q_DECLARE_METATYPE(Expe::Vector2i);
Q_DECLARE_METATYPE(Expe::Vector2i64);

#endif
