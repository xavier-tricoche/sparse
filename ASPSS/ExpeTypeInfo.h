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



#ifndef _ExpeTypeInfo_h_
#define _ExpeTypeInfo_h_

#include "ExpePrerequisites.h"

namespace Expe
{

namespace Type
{
//     #define GL_BYTE                           0x1400
//     #define GL_UNSIGNED_BYTE                  0x1401
//     #define GL_SHORT                          0x1402
//     #define GL_UNSIGNED_SHORT                 0x1403
//     #define GL_INT                            0x1404
//     #define GL_UNSIGNED_INT                   0x1405
//     #define GL_FLOAT                          0x1406
//     #define GL_2_BYTES                        0x1407
//     #define GL_3_BYTES                        0x1408
//     #define GL_4_BYTES                        0x1409
//     #define GL_DOUBLE                         0x140A
//     #define GL_DOUBLE_EXT                     0x140A

    enum Enum
    {
        // OpenGL type compatibility
        BYTE        = 0x1400,
        UBYTE       = 0x1401,
        SHORT       = 0x1402,
        USHORT      = 0x1403,
        INT         = 0x1404,
        UINT        = 0x1405,
        REAL        = 0x1406,
        DOUBLE      = 0x140A,
        HALF_REAL   = 0x140B,
        INT24,
        UINT24
    };
}

template <typename T>
struct TypeInfo
{
    enum {
        Id=0,               /// OpenGL type ID (default: unknow by OpenGL)
        NofComponents=1     /// Number of component composing the type (default: single)
    };
    /// Type of the component (default: the type itself)
    typedef T ComponentType;
    /// Closest floating point type
    typedef T FloatingPointType;
    /// Closest integer type
    typedef T IntegerType;
};

#define DECLARE_INT_SCALAR_TYPE(TYPE,TYPEID,FPTYPE) \
    template<> struct TypeInfo<TYPE> \
    { \
        enum {Id=TYPEID, NofComponents=1}; \
        typedef TYPE ComponentType; \
        typedef TYPE IntegerType; \
        typedef FPTYPE FloatingPointType; \
    }

#define DECLARE_FP_SCALAR_TYPE(TYPE,TYPEID,INTEGERTYPE) \
    template<> struct TypeInfo<TYPE> \
    { \
        enum {Id=TYPEID, NofComponents=1}; \
        typedef TYPE ComponentType; \
        typedef INTEGERTYPE IntegerType; \
        typedef TYPE FloatingPointType; \
    }

DECLARE_INT_SCALAR_TYPE(byte,   0x1400,     float); // FIXME half support ?
DECLARE_INT_SCALAR_TYPE(ubyte,  0x1401,     float); // FIXME half support ?
DECLARE_INT_SCALAR_TYPE(short,  0x1402,     float); // FIXME half support
DECLARE_INT_SCALAR_TYPE(ushort, 0x1403,     float); // FIXME half support
DECLARE_INT_SCALAR_TYPE(int,    0x1404,     float);
DECLARE_INT_SCALAR_TYPE(uint,   0x1405,     float);
DECLARE_FP_SCALAR_TYPE (float,  0x1406,     int);
DECLARE_FP_SCALAR_TYPE (double, 0x140A,     int64);
// DECLARE_FP_SCALAR_TYPE (half, -1,     short); // FIXME half support

#undef DECLARE_INT_SCALAR_TYPE
#undef DECLARE_FP_SCALAR_TYPE

}

#endif
