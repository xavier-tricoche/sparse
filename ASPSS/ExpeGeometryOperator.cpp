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



#include "ExpeGeometryOperator.h"
#include "ExpeLogManager.h"
#include "ExpeStaticInitializer.h"

namespace Expe
{

GeometryOperator::GeometryOperator(GeometryObject* pInput)
    : mpInput(pInput), mpOutput(0), mIsInputAsOutputDoable(false), mInputAsOutput(false)
{
}

GeometryOperator::~GeometryOperator()
{
}


//EXPE_SINGLETON_IMPLEMENTATION_AUTO(GeometryOperatorManager);

// void GeometryOperatorManager::registerFactory(const String& name, GeometryOperatorFactory* pFactory)
// {
//     FactoryMap::iterator iter = mFactories.find(name);
//     if (iter!=mFactories.end())
//     {
//         LOG_ERROR("GeometryOperatorManager: factory " << name << " already registered.");
//     }
//     mFactories[name] = pFactory;
// }
// 
// GeometryOperator* GeometryOperatorManager::create(const String& name, Geometry* pInput)
// {
//     FactoryMap::iterator iter = mFactories.find(name);
//     if ( (iter==mFactories.end()) || (!iter->second->match(pInput)) )
//         return 0;
//     return iter->second->create(pInput);
// }
// 
// StringArray GeometryOperatorManager::getAvailableTypes(void) const
// {
//     StringArray list;
//     for (FactoryMap::const_iterator iter=mFactories.begin() ; iter!=mFactories.end() ; ++iter)
//     {
//         list.push_back(iter->first);
//     }
//     return list;
// }
// 
// StringArray GeometryOperatorManager::getAvailableTypes(const Geometry* pGeometry) const
// {
//     StringArray list;
//     for (FactoryMap::const_iterator iter=mFactories.begin() ; iter!=mFactories.end() ; ++iter)
//     {
//         if (iter->second->match(pGeometry))
//             list.push_back(iter->first);
//     }
//     return list;
// }

}

//#include "ExpeGeometryOperator.moc"
