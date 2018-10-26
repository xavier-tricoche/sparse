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


#ifndef _ExpePrerequisites_h_
#define _ExpePrerequisites_h_

#include "config.h"
//#include "qglobal.h"

#define GCC_VERSION (__GNUC__ * 10  + __GNUC_MINOR__ )

// this allow to solve conflicts between qt and glx header files.
#if defined EXPE_FORCE_UNDEF_QT_SUPPORT && defined EXPE_QT_SUPPORT
#undef EXPE_QT_SUPPORT
#endif


// namespace std {
// template <typename T> struct allocator;
// template <typename T, typename A = std::allocator<T> > struct vector;
// }

#include <vector>
// #include <list>
// #include <iostream>

#include <assert.h>

#if (GCC_VERSION>=40) && defined (EXPE_USE_SSE)
    #include <ext/malloc_allocator.h>
#endif

#ifdef Bool
    #undef Bool
#endif

class QString;
class QByteArray;

namespace Expe
{

/// used to swizzle vector components
enum {X=0,Y=1,Z=2,W=3};

#define DESTROY_PTR(X) delete X;X = 0;
#define DESTROY_TAB(X) delete[] X;X = 0;

// common types declaration

typedef char byte;
typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef unsigned int uint;
//typedef qint64 int64;
//typedef quint64 uint64;
typedef long long int64;
typedef unsigned long long uint64;
typedef long double ldouble;

/// default floating point type
#ifndef EXPE_DOUBLE_PRECISION
typedef float Real;
#else
typedef double Real;
#endif

typedef ubyte* Pointer;
typedef const ubyte* ConstPointer;
typedef long ArithPtr;
typedef int Enum;

typedef std::vector<bool> BoolArray;

typedef std::vector<Real> RealArray;

typedef uint Index;
typedef std::vector<Index> IndexArray;

struct Rgba;
typedef std::vector<Rgba> RbgaArray;


#ifdef EXPE_USE_SSE
// #ifdef GCC4
typedef float v4sf __attribute__ (( vector_size (16) ));
typedef int v4si __attribute__ (( vector_size (16) ));

typedef float v2sf __attribute__ (( vector_size (8) ));
typedef int v2si __attribute__ (( vector_size (8) ));

// #else
// typedef float v4sf __attribute__ ((mode(V4SF)));
// typedef int v4si __attribute__ ((mode(V4SI)));
// #endif
#endif


/** Add a memeber variable with the trivial get/set associated method.
    \code
        QUICK_MEMBER(MonType,MonAttribut);
    \endcode
    is equivalent to:
    \code
        inline MonType getMonAttribut(void) const
        {
            return mMonAttribut;
        }
    public slots:
        inline void setMonAttribut (const MonType value)
        {
            mMonAttribut = value;
        }
    protected:
        MonType mMonAttribut;
    \endcode
*/
#define QUICK_MEMBER(TYPE,NAME)  public: \
        inline void set##NAME (const TYPE value) { m##NAME = value;} \
        /** Get the respective property value */ \
        inline TYPE get##NAME (void) const { return m##NAME ; } \
    protected: \
        TYPE m##NAME; \
    public:


/** Like \ref QUICK_MEMBER, but for complex types.
    TODO use meta template programming to automatically make the type as a reference if required (see Boost).
*/
#define QUICK_MEMBER_ADDR(TYPE,NAME)  public: \
        inline void set##NAME (const TYPE & value) { m##NAME = value;} \
        /** Get the respective property value */ \
        inline TYPE & get##NAME (void) const { return m##NAME; } \
    protected: \
        TYPE m##NAME; \
    public:


enum Verbosity {VerbAuto, Quiet, Verbose};


typedef QByteArray ByteString;

class LogManager;
class PluginManager;

// Math
class Matrix3;
typedef std::vector<Matrix3> Matrix3Array;

class Matrix4;
#if (GCC_VERSION>=40) && defined (EXPE_USE_SSE)
// Force 128 bits alignment for SSE operations
typedef std::vector<Matrix4,__gnu_cxx::malloc_allocator<Matrix4> > Matrix4Array;
#else
typedef std::vector<Matrix4> Matrix4Array;
#endif

class Quaternion;
typedef std::vector<Quaternion> QuaternionArray;

class Color;
#if (GCC_VERSION>=40) && defined (EXPE_USE_SSE)
// Force 128 bits alignment for SSE operations
typedef std::vector<Color,__gnu_cxx::malloc_allocator<Color> > ColorArray;
#else
typedef std::vector<Color> ColorArray;
#endif

class Transform;
typedef std::vector<Transform> TransformArray;


class PointSet;
class Mesh;
class QueryDataStructure;
class MultiQueryDataStructure;
class QueryQrid;
class MultiQueryQrid;

}

#include "ExpeTypeInfo.h"

#endif
