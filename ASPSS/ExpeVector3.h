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



#ifndef _ExpeVector3_h_
#define _ExpeVector3_h_

#include "ExpeVector3Implementation.h"

namespace Expe
{

#define EXPE_DECLARE_VECTOR3(SCALARTYPE,BASE,SUFFIX) \
    class Vector ## SUFFIX : public BASE<SCALARTYPE,Vector ## SUFFIX> \
    { \
        typedef BASE<SCALARTYPE,Vector ## SUFFIX> Super; \
    public: \
        inline Vector ## SUFFIX() {} \
        inline Vector ## SUFFIX(SCALARTYPE _x, SCALARTYPE _y, SCALARTYPE _z=0) : Super(_x,_y,_z) {} \
        inline Vector ## SUFFIX(SCALARTYPE _x) : Super(_x) {} \
        explicit inline Vector ## SUFFIX(const SCALARTYPE coords[3]) : Super(coords) {} \
        inline Vector ## SUFFIX(const Vector ## SUFFIX& vec) : Super(vec) {} \
    \
        inline friend std::ostream& operator << (std::ostream& o, const Vector ## SUFFIX& v) { \
            o << "Vector" <<  # SUFFIX << '(' << v.x << ',' << v.y << ',' << v.z << ')'; \
            return o; \
        } \
        inline friend QDebug operator<<(QDebug dbg, const Vector ## SUFFIX& v) { \
            dbg.nospace() << "Vector" # SUFFIX << '(' << v.x << ',' << v.y << ',' << v.z << ')'; \
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
        enum {NofComponents=3}; \
        typedef SCALARTYPE ComponentType; \
        typedef GetVector<NofComponents,TypeInfo<ComponentType>::IntegerType>::Type          IntegerType; \
        typedef GetVector<NofComponents,TypeInfo<ComponentType>::FloatingPointType>::Type    FloatingPointType; \
        static_assert( (MTP::compare_type<Vector##SUFFIX,FloatingPointType>::result) \
            || (MTP::compare_type<Vector##SUFFIX,IntegerType>::result), "vector value type must be integral or float number" ); \
    };

EXPE_DECLARE_VECTOR3(float,Vector3FloatImplementation,3f);
EXPE_DECLARE_VECTOR3(double,Vector3FloatImplementation,3d);
EXPE_DECLARE_VECTOR3(int,Vector3IntegerImplementation,3i);
EXPE_DECLARE_VECTOR3(int64,Vector3IntegerImplementation,3i64);


/*
template <>
struct TypeInfo<Vector3f>
{
    // FIXME Id
    enum {NofComponents=3};
    typedef float       ComponentType;
    typedef Vector3i    IntegerType;
    typedef Vector3f    FloatingPointType;
};



template <>
struct TypeInfo<Vector3d>
{
    // FIXME Id
    enum {NofComponents=3};
    typedef double      ComponentType;
    typedef Vector3i64  IntegerType;
    typedef Vector3d    FloatingPointType;
};


template <>
struct TypeInfo<Vector3i>
{
    // FIXME Id
    enum {NofComponents=3};
    typedef int         ComponentType;
    typedef Vector3i    IntegerType;
    typedef Vector3f    FloatingPointType;
};

template <>
struct TypeInfo<Vector3i64>
{
    // FIXME Id
    enum {NofComponents=3};
    typedef int64       ComponentType;
    typedef Vector3i64  IntegerType;
    typedef Vector3d    FloatingPointType;
};

*/

//--------------------------------------------------------------------------------
// Serialization
//--------------------------------------------------------------------------------

/** \relates Vector3
    Writes the given \a vector to the given \a stream, and returns a reference to the stream.
*/
QDataStream& operator<<(QDataStream& stream, const Vector3& vector);

/** \relates Vector3
    Reads a vector3 from the given \a stream into the given \a vector, and returns a reference to the stream.
*/
QDataStream& operator>>(QDataStream& stream, Vector3& vector);

/** \relates Vector3i
    Writes the given \a vector to the given \a stream, and returns a reference to the stream.
*/
QDataStream& operator<<(QDataStream& stream, const Vector3i& vector);

/** \relates Vector3i
    Reads a vector3 from the given \a stream into the given \a vector, and returns a reference to the stream.
*/
QDataStream& operator>>(QDataStream& stream, Vector3i& vector);

}

#include <qmetatype.h>
Q_DECLARE_METATYPE(Expe::Vector3f);
Q_DECLARE_METATYPE(Expe::Vector3d);
Q_DECLARE_METATYPE(Expe::Vector3i);
Q_DECLARE_METATYPE(Expe::Vector3i64);

#endif
