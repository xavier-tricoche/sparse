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



#include "ExpeStaticInitializer.h"

namespace Expe
{

EXPE_SINGLETON_IMPLEMENTATION(StaticInitializer);

StaticInitializer::StaticInitializer(void)
{
}

void StaticInitializer::add(StaticFunction* sf)
{
    mPool.push_back(sf);
}

void StaticInitializer::processAll(void)
{
    for (uint i=0 ; i<mPool.size() ; ++i)
    {
        mPool[i]->call();
    }
    mPool.clear();
}

}
