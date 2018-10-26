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



#ifndef _ExpeSmartCast_h_
#define _ExpeSmartCast_h_

#include "ExpeCore.h"
#include <hash_map>
#include <QVariant>
#include "ExpeVector2.h"
#include "ExpeVector3.h"
#include "ExpeVector4.h"
#include "ExpeColor.h"

using namespace std;

namespace Expe
{

class SmartCastManager
{
public:
    typedef void (*CastFuncPtr)(const QVariant& v, void* dest);
    typedef std::map<uint,CastFuncPtr> CastFuncMap;
    static CastFuncMap staticCastFuncMap;
    static std::vector<CastFuncPtr> staticCastFuncGrid;
    
    template <typename FromT, typename ToT> inline static void registerCastFunc(CastFuncPtr func)
    {
        staticCastFuncMap[makeKey(qMetaTypeId<FromT>(),qMetaTypeId<ToT>())] = func;
    }
    
    static void buildPerfectGrid(void);
    static void Initialize(void);
    
    inline static uint makeKey(uint a, uint b)
    {
        return a | (b<<9);
    }
};


template <typename ToT,typename FromT> inline ToT smart_cast(const FromT& valueIn, bool* ok = 0)
{
    if (typeid(ToT)==typeid(FromT))
    {
        if (ok)
            *ok = true;
        return valueIn;
    }

    if (ok)
        *ok = false;
    return ToT();
}


template <> inline Vector2 smart_cast<>(const Real& valueIn, bool* ok) {if(ok) *ok=true; return Vector2(valueIn, valueIn);}
template <> inline Vector2 smart_cast<>(const Vector3& v, bool* ok) {if(ok) *ok=true; return Vector2(v.x, v.y);}
template <> inline Vector2 smart_cast<>(const Vector4& v, bool* ok) {if(ok) *ok=true; return Vector2(v.x, v.y);}

template <> inline Vector3 smart_cast<>(const Real& valueIn, bool* ok) {if(ok) *ok=true; return Vector3(valueIn, valueIn, valueIn);}
template <> inline Vector3 smart_cast<>(const Vector2& v, bool* ok) {if(ok) *ok=true; return Vector3(v.x, v.y, 0.f);}
template <> inline Vector3 smart_cast<>(const Vector4& v, bool* ok) {if(ok) *ok=true; return Vector3(v.x, v.y, v.z);}

template <> inline Vector4 smart_cast<>(const Real& valueIn, bool* ok) {if(ok) *ok=true; return Vector4(valueIn, valueIn, valueIn, valueIn);}
template <> inline Vector4 smart_cast<>(const Vector2& v, bool* ok) {if(ok) *ok=true; return Vector4(v.x, v.y, 0.f, 1.f);}
template <> inline Vector4 smart_cast<>(const Vector3& v, bool* ok) {if(ok) *ok=true; return Vector4(v.x, v.y, v.z, 1.f);}

template <> inline Color smart_cast<>(const Rgba& i,    bool* ok) {if(ok) *ok=true; Color c; c.fromRGBA(i); return c;} // int to RGBA
template <> inline Color smart_cast<>(const Real& val,  bool* ok) {if(ok) *ok=true; return Color(val, val, val, 1.f);} // luminance
template <> inline Color smart_cast<>(const Vector2& v, bool* ok) {if(ok) *ok=true; return Color(v.x, v.x, v.x, v.y);} // luminance,alpha
template <> inline Color smart_cast<>(const Vector3& v, bool* ok) {if(ok) *ok=true; return Color(v.x, v.y, v.z, 1.f);} // RGB
template <> inline Color smart_cast<>(const Vector4& v, bool* ok) {if(ok) *ok=true; return Color(v.x, v.y, v.z, v.w);} // RGBA
template <> inline Color smart_cast<>(const QColor&  c, bool* ok) {if(ok) *ok=true; return Color(c);} // auto

// fast data access to a QVariant without additionnal check
// template <typename T> inline T reinterpret_qvariant(const QVariant& v) {
//     //assert(qMetaTypeId<T>()==v.userType());
//     return *reinterpret_cast<const T *>(v.constData());
// }


/** Like the C++ standard reinterpret_cast, this function cast the QVariant held value without any check.
    \remarks Wrong types leads to a fatal error in debug mode.
*/
template <typename T> inline T qvariant_reinterpret_cast(const QVariant& v) {
    assert(qMetaTypeId<T>()==v.userType());
    const QVariant::Private& d = reinterpret_cast<const QVariant::Private&>(v);

    if ((d.type+1)&0xfffffff8) //if (d.type>6) but faster !
        return *reinterpret_cast<const T *>(d.is_shared ? d.data.shared->ptr : reinterpret_cast<const void *>(&d.data.ptr));
    else
        return *reinterpret_cast<const T *>(&d.data);
}




/** QVariant automatic conversion
*/
template <typename T> inline T smart_cast(const QVariant& valueIn, bool* ok = 0)
{
    if (ok)
        *ok = true;
    
    uint typein = valueIn.userType();
    uint typeout = qMetaTypeId<T>();
    
    // First, handle the trivial case.
    if (typein==typeout)
        return qvariant_reinterpret_cast<T>(valueIn);
    
    // Secondly, try our own convert system.
    SmartCastManager::CastFuncPtr ptr =
        SmartCastManager::staticCastFuncGrid[SmartCastManager::makeKey(typein,typeout)];
    if (ptr)
    {
        T aux;
        (*ptr)(valueIn,&aux);
        return aux;
    }
    
    // Finally, fall back to the default QVariant conversion functionallity.
    if (ok)
        *ok = valueIn.canConvert<T>();
    return valueIn.value<T>();
}


}


#endif

