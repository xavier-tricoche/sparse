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



// include the QVariant header with public facility...
#include <QtCore/qatomic.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qmap.h>
#include <QtCore/qstring.h>
#define protected public
#include <QVariant>
#undef protected
// ...done.

#include "ExpeSmartCast.h"

namespace Expe
{

SmartCastManager::CastFuncMap SmartCastManager::staticCastFuncMap;
std::vector<SmartCastManager::CastFuncPtr> SmartCastManager::staticCastFuncGrid;

void SmartCastManager::buildPerfectGrid(void)
{
    // search for the greater supported typeid
    uint maxTypeID = 0;
    for (CastFuncMap::const_iterator iter=staticCastFuncMap.begin() ; iter!=staticCastFuncMap.end() ; ++iter)
    {
        if (iter->first>maxTypeID)
            maxTypeID = iter->first;
    }
    std::cout << "SmartCastManager::maxTypeID = " << maxTypeID << "\n";
    
    staticCastFuncGrid.resize(maxTypeID+1);
    for (uint i=0 ; i<staticCastFuncGrid.size() ; ++i)
        staticCastFuncGrid[i] = 0;
    for (CastFuncMap::const_iterator iter=staticCastFuncMap.begin() ; iter!=staticCastFuncMap.end() ; ++iter)
    {
        staticCastFuncGrid[iter->first] = iter->second;
    }
}


// generic vector to vector conversion using smart_cast
template <typename FromT, typename ToT> void ConvertVecToVec(const QVariant& v, void* dest) {
    *reinterpret_cast<ToT*>(dest) = smart_cast<ToT>(qvariant_reinterpret_cast<FromT>(v));
}
// e.g.: REGISTER_VEC_TO_VEC(2,3)
#define REGISTER_VEC_TO_VEC(FROMS,TOS) registerCastFunc<Vector ## FROMS,Vector ## TOS>(ConvertVecToVec<Vector ## FROMS,Vector ## TOS>)

// generic scalar to float vector conversion using smart_cast (partially)
template <typename FromT, typename ToT> void ConvertScalarToVecF(const QVariant& v, void* dest) {
    *reinterpret_cast<ToT*>(dest) = smart_cast<ToT>(Real(qvariant_reinterpret_cast<FromT>(v)));
}
// e.g.: REGISTER_SCALAR_TO_VECF(uint,3)
#define REGISTER_SCALAR_TO_VECF(FROMT,TOS) registerCastFunc<FROMT,Vector ## TOS>(ConvertScalarToVecF<FROMT,Vector ## TOS>)


QVariant::f_convert qtCastFunc = 0;

static bool smartCastFunc(const QVariant::Private *d, QVariant::Type t, void *result, bool *ok)
{
    bool dummy;
    if (!ok)
        ok = &dummy;
    
    uint typein = d->type;
    uint typeout = t;
    
    // TODO: handling the trivial case seems to be far to be trivial :(
    
    SmartCastManager::CastFuncPtr ptr =
        SmartCastManager::staticCastFuncGrid[SmartCastManager::makeKey(typein,typeout)];
    
    if (ptr)
    {
        (*ptr)(*reinterpret_cast<const QVariant*>(d),result);
        return true;
    }
    
    return qtCastFunc(d,t,result,ok);
}

void SmartCastManager::Initialize(void)
{
    if (qtCastFunc==0)
        qtCastFunc = QVariant::handler->convert;
    const_cast<QVariant::Handler*>(QVariant::handler)->convert = smartCastFunc;

//     registerCastFunc<Real,Vector2>(ConvertVector<Real,Vector2>);
//     registerCastFunc<Real,Vector3>(ConvertVector<Real,Vector3>);
//     registerCastFunc<Real,Vector4>(ConvertVector<Real,Vector4>);

    
    REGISTER_SCALAR_TO_VECF(int,2);
    REGISTER_SCALAR_TO_VECF(uint,2);
    REGISTER_SCALAR_TO_VECF(short,2);
    REGISTER_SCALAR_TO_VECF(unsigned short,2);
    REGISTER_SCALAR_TO_VECF(long,2);
    REGISTER_SCALAR_TO_VECF(unsigned long,2);
    REGISTER_SCALAR_TO_VECF(long long,2);
    REGISTER_SCALAR_TO_VECF(unsigned long long,2);
    REGISTER_SCALAR_TO_VECF(float,2);
    REGISTER_SCALAR_TO_VECF(double,2);
//     REGISTER_SCALAR_TO_VECF(long double,2);
    
    REGISTER_SCALAR_TO_VECF(int,3);
    REGISTER_SCALAR_TO_VECF(uint,3);
    REGISTER_SCALAR_TO_VECF(short,3);
    REGISTER_SCALAR_TO_VECF(unsigned short,3);
    REGISTER_SCALAR_TO_VECF(long,3);
    REGISTER_SCALAR_TO_VECF(unsigned long,3);
    REGISTER_SCALAR_TO_VECF(long long,3);
    REGISTER_SCALAR_TO_VECF(unsigned long long,3);
    REGISTER_SCALAR_TO_VECF(float,3);
    REGISTER_SCALAR_TO_VECF(double,3);
    //REGISTER_SCALAR_TO_VECF(long double,3);
    
    REGISTER_SCALAR_TO_VECF(int,4);
    REGISTER_SCALAR_TO_VECF(uint,4);
    REGISTER_SCALAR_TO_VECF(short,4);
    REGISTER_SCALAR_TO_VECF(unsigned short,4);
    REGISTER_SCALAR_TO_VECF(long,4);
    REGISTER_SCALAR_TO_VECF(unsigned long,4);
    REGISTER_SCALAR_TO_VECF(long long,4);
    REGISTER_SCALAR_TO_VECF(unsigned long long,4);
    REGISTER_SCALAR_TO_VECF(float,4);
    REGISTER_SCALAR_TO_VECF(double,4);
    //REGISTER_SCALAR_TO_VECF(long double,4);
    
    REGISTER_VEC_TO_VEC(2,3);
    REGISTER_VEC_TO_VEC(2,4);
    REGISTER_VEC_TO_VEC(3,2);
    REGISTER_VEC_TO_VEC(3,4);
    REGISTER_VEC_TO_VEC(4,2);
    REGISTER_VEC_TO_VEC(4,3);
    
    buildPerfectGrid();
}

}

