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



#ifndef _ExpeMath_h_ 
#define _ExpeMath_h_

#include <stdlib.h> // for "abs"
#include <math.h> // for "fabs"
#include "ExpePrerequisites.h"
#include <limits>

#define EXPE_TRIGO_TAB_SIZE 4096
#define EXPE_TRIGO_TAB_SIZE_1 4095
#define EXPE_TRIGO_TAB_SIZE2 2048

#define EXPE_TRIGO_TAB_FACTOR 651.8986469044f
#define EXPE_TRIGO_TAB_SIZE2f 2048.f

#define EXPE_ATRIGO_GUARD_SIZE 256

#define EXPE_ATAN_TAB_SIZE 512
#define EXPE_ATAN_BOUND 10
#define EXPE_ATAN_MASK 0x3FF
#define EXPE_ATAN_SCALE 51.2f

#ifndef M_PI
#define M_PI 3.1415926535897932384626433
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923132165
#endif

namespace Expe
{

namespace Math
{

	/*
template <typename T=Real>
struct Constant
{
    static const T PI        = M_PI;
    static const T TWO_PI    = 2.*PI;
    static const T PI_OVER_2 = PI/2.;
    static const T HALF_PI   = 0.5*PI;
    static const T PI_OVER_3 = PI/3.;
    static const T PI_OVER_4 = PI/4.;
    static const T PI_OVER_6 = PI/6.;
    
    static const T TWO_PI_OVER_3    = 2.*PI/3.;
    static const T RAD_TO_DEGREE    = 180./PI;
    static const T DEGREE_TO_RAD    = PI/180.;
    static const T ONE_OVER_3       = 1./3.;
    static const T ONE_OVER_SQRT_3  = 0.57735026918963;
};

static const float PI        = Constant<Real>::PI;
static const float TWO_PI    = Constant<Real>::TWO_PI;
static const float PI_OVER_2 = Constant<Real>::PI_OVER_2;
static const float HALF_PI   = Constant<Real>::HALF_PI;
static const float PI_OVER_3 = Constant<Real>::PI_OVER_3;
static const float PI_OVER_4 = Constant<Real>::PI_OVER_4;
static const float PI_OVER_6 = Constant<Real>::PI_OVER_6;

static const float TWO_PI_OVER_3    = Constant<Real>::TWO_PI_OVER_3;
static const float RAD_TO_DEGREE    = Constant<Real>::RAD_TO_DEGREE;
static const float DEGREE_TO_RAD    = Constant<Real>::DEGREE_TO_RAD;
static const float ONE_OVER_3       = Constant<Real>::ONE_OVER_3;
static const float ONE_OVER_SQRT_3  = Constant<Real>::ONE_OVER_SQRT_3;
*/

static const float PI        = M_PI;
static const float TWO_PI    = 2.*PI;
static const float PI_OVER_2 = PI/2.;
static const float HALF_PI   = 0.5*PI;
static const float PI_OVER_3 = PI/3.;
static const float PI_OVER_4 = PI/4.;
static const float PI_OVER_6 = PI/6.;
    
static const float TWO_PI_OVER_3    = 2.*PI/3.;
static const float RAD_TO_DEGREE    = 180./PI;
static const float DEGREE_TO_RAD    = PI/180.;
static const float ONE_OVER_3       = 1./3.;
static const float ONE_OVER_SQRT_3  = 0.57735026918963;


#ifdef EXPE_USE_SSE
/** fast SSE constant */
static const v4si ABSPS_MASK = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
static const v4si NEGPS_MASK = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
#endif

// static const float INF = INFINITY;
// static const float NaN = NAN;


//--------------------------------------------------------------------------------
// TRIGO
//--------------------------------------------------------------------------------
extern float* msCosTable;
extern float* msSinTable;
extern float* msTanTable;

extern float* msACosTable;
extern float* msASinTable;
extern float* msATanTable;
extern uint* msSqrtTable;

//static int msTrigTableSize;
//static float msTrigTableFactor;
//static float msATrigTableFactor;
//static int msATrigTableOffset;


template <typename T> inline const T& Min(const T& a, const T& b) { return a<b ? a : b; }
template <typename T> inline const T& Max(const T& a, const T& b) { return a<b ? b : a; }
template <typename T> inline const T& Min(const T& a, const T& b, const T& c) { return Min(Min(a,b),c); }
template <typename T> inline const T& Max(const T& a, const T& b, const T& c) { return Max(Max(a,b),c); }


template <typename T> inline T Cos(T x) { return cos(x); }
template <> inline float Cos<float>(float x) { return cosf(x); }

template <typename T> inline T Sin(T x) { return sin(x); }
template <> inline float Sin<float>(float x) { return sinf(x); }

template <typename T> inline T Tan(T x) { return tan(x); }
template <> inline float Tan<float>(float x) { return tanf(x); }


template <typename T> inline T ACos(T x) { return acos(x); }
template <> inline float ACos<float>(float x) { return acosf(x); }

template <typename T> inline T ASin(T x) { return asin(x); }
template <> inline float ASin<float>(float x) { return asinf(x); }

template <typename T> inline T ATan(T x) { return atan(x); }
template <> inline float ATan<float>(float x) { return atanf(x); }


template <typename T> inline T Sqrt(T x) { return sqrt(x); }
template <> inline float Sqrt<float>(float x) { return sqrtf(x); }


template <typename T> inline T Abs(T x) { return x<T(0) ? -X : x; }
template <> inline byte Abs<byte>(byte x) { return abs(x); }
template <> inline short Abs<short>(short x) { return abs(x); }
template <> inline int Abs<int>(int x) { return abs(x); }
template <> inline int64 Abs<int64>(int64 x) { return abs(x); }
template <> inline float Abs<float>(float x) { return fabsf(x); }
template <> inline double Abs<double>(double x) { return fabs(x); }


template <typename T> inline bool isFinite(T value)
{
    return (value>=std::numeric_limits<T>::min()) && (value<=std::numeric_limits<T>::max());
}
// template <> inline bool isFinite<float>(float value) { return (value<INF) && (value>-INF); }
// template <> inline bool isFinite<double>(double value) { return (value<INF) && (value>-INF); }
// template <> inline bool isFinite<ldouble>(ldouble value) { return (value<INF) && (value>-INF); }


template <typename T>
inline T Rand(void) { return T(random() % 32767)/T(32767.0); }

template <typename T> inline const T& Clamp(const T& x, const T& min, const T& max) { return x<min ? min : (x>max ? max : x); }

inline static float CosTable(float angle)
{
    int id;
    if (angle >= 0.)
    {
        id = (int)(angle * EXPE_TRIGO_TAB_FACTOR) & EXPE_TRIGO_TAB_SIZE_1;
    }
    else
    {
        id = EXPE_TRIGO_TAB_SIZE_1 - ((int)(-angle * EXPE_TRIGO_TAB_FACTOR) & EXPE_TRIGO_TAB_SIZE_1);
    }

    return msCosTable[id];
}

inline static float SinTable(float angle)
{
    int id;
    if (angle >= 0.)
    {
        id = (int)(angle * EXPE_TRIGO_TAB_FACTOR) & EXPE_TRIGO_TAB_SIZE_1;
    }
    else
    {
        id = EXPE_TRIGO_TAB_SIZE_1 - ((int)(-angle * EXPE_TRIGO_TAB_FACTOR) & EXPE_TRIGO_TAB_SIZE_1);
    }

    return msSinTable[id];
}

inline static float TanTable(float angle)
{
    int id;
    if (angle >= 0.)
    {
        id = (int)(angle * EXPE_TRIGO_TAB_FACTOR) & EXPE_TRIGO_TAB_SIZE_1;
    }
    else
    {
        id = EXPE_TRIGO_TAB_SIZE_1 - ((int)(-angle * EXPE_TRIGO_TAB_FACTOR) & EXPE_TRIGO_TAB_SIZE_1);
    }

    return msTanTable[id];
}

inline static float ATanTable(float x)
{
    //return atanf(x);
    
    return msATanTable[EXPE_ATAN_MASK & (EXPE_ATAN_TAB_SIZE + (int)( x * EXPE_ATAN_SCALE))];
}

inline static float ACosTable(float x)
{
    //return acos(x);
     //return msACosTable[ (int)( x * 2048.f) + 2048 + 256 ];
     return msACosTable[
        (int)( x * EXPE_TRIGO_TAB_SIZE2f)
        + EXPE_TRIGO_TAB_SIZE2 + EXPE_ATRIGO_GUARD_SIZE ];
}

inline static float ASinTable(float x)
{
     return msASinTable[
        (int)( x * EXPE_TRIGO_TAB_SIZE2f)
        + EXPE_TRIGO_TAB_SIZE2 + EXPE_ATRIGO_GUARD_SIZE ];
}
 
inline static float FastATan(float fValue)
{
  if (fValue == 0.0f)
    return 0.0f;
  
  float fV       = ( (fValue>1.0f) || (fValue<-1.0f) ) ? 1.0f/fValue : fValue;
  float fVSqr    = fV*fV;
  float fResult  = 0.0208351f;
  fResult       *= fVSqr;
  fResult       -= 0.085133f;
  fResult       *= fVSqr;
  fResult       += 0.180141f;
  fResult       *= fVSqr;
  fResult       -= 0.3302995f;
  fResult       *= fVSqr;
  fResult       += 0.999866f;
  fResult       *= fV;
    
  return (fValue>1.0f) ? (Math::PI_OVER_2 - fResult)
                       : (fValue<-1.0f) ? -(fResult + Math::PI_OVER_2)
                                        : fResult;
}

//--------------------------------------------------------------------------------

//#define FP_BITS(X) (reinterpret_cast<uint&>(X))
#define FP_BITS(X) (*(uint*)(&X))
// inline static float SqrtTable(float x)
// {
//   if (FP_BITS(x) == 0)
//     return 0.0;                 // check for square root of 0
//   
//   FP_BITS(x) = msSqrtTable[(FP_BITS(x) >> 8) & 0xFFFF] | ((((FP_BITS(x) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
//   
//   return x;
// }

#ifdef EXPE_USE_SSE
/** 
    note : le rcpss à 1 cycle de moins que le mulss mais forcer l'utilisation du rcpss via ASM empeche le compilo d'entrelacer des instructions ...alors ?? return 1/x; peut être ??
*/
inline static float FastSqrt2(float x)
{
    float r;
    asm (
        "rsqrtss %[value], %[result] \n\t"
        : [result] "=x,x" (r)
        : [value]   "x,m"(x)
    );
    return 1.f/(r*(1.5f-0.5*x*r*r));
}
inline static float FastSqrt(float x)
{
    float r;
    asm (
        "rsqrtss %[value], %[result] \n\t"
        : [result] "=x,x" (r)
        : [value]   "x,m"(x)
    );
    asm (
        "rcpss %[value], %[result] \n\t"
        : [result] "=x" (r)
        : [value]   "x"(r)
    );
    return r;
}
#else

// inline static float FastSqrt(float x)
// {
//   if (FP_BITS(x) == 0)
//     return 0.0;                 // check for square root of 0
//   
//   FP_BITS(x) = msSqrtTable[(FP_BITS(x) >> 8) & 0xFFFF] | ((((FP_BITS(x) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
//   
//   return x;
// }
#endif






// #ifdef EXPE_USE_SSE
// inline static float FastInvSqrt(float x)
// {
//     float r;
//     asm (
//         "rsqrtss %[value], %[result] \n\t"
//         : [result] "=x,x" (r)
//         : [value]   "x,m"(x)
//     );
//     return r;
// }
// inline static float InvSqrt(float x)
// {
//     float r;
//     asm (
//         "rsqrtss %[value], %[result] \n\t"
//         : [result] "=x,x" (r)
//         : [value]   "x,m"(x)
//     );
//     return r*(1.5f-0.5*x*r*r); // Improve the accuracy with one Newton step
// }
// #else
// inline static float FastInvSqrt(float x)
// {
//     register float half = 0.5f*x;
//     int i = reinterpret_cast<int&>(x);
//     i = 0x5f3759df - (i >> 1);
//     x = reinterpret_cast<float&>(i);
//     return x*(1.5f - half*x*x);
// }


template <typename T>
inline static T InvSqrt(T x) { return 1./Sqrt(x); }

// template <>
// inline static float InvSqrt<float>(float x)
// {
//     register float half = 0.5f*x;
//     int i = reinterpret_cast<int&>(x);
//     i = 0x5f3759df - (i >> 1);
//     x = reinterpret_cast<float&>(i);
//     x = x*(1.5f - half*x*x);
//     return x*(1.5f - half*x*x);
// }
// #endif


template <typename T = float>
struct SmoothStep
{
    inline SmoothStep(T e0_, T e1_)
        : e0(e0_), e1(e1_)
    {
        T alpha = 1./(e1-e0);
        T alpha2 = alpha * alpha;
        T alpha3 = alpha2 * alpha;
        a = -2.*alpha3;
        b = alpha2*(3.+6.*alpha*e0);
        c = -6.*e0*alpha2*(1.+alpha*e0);
        d = e0*e0*alpha2*(3.+2.*e0*alpha);
    }
    T a, b, c, d, e0, e1;
    inline T operator() (T x)
    {
        if(x<e0)
            return 0.;
        else if(x>e1)
            return 1.;
        else
            return x*(x*(a*x+b)+c)+d;
    }
};


}

}



#endif

