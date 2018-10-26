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



#include "ExpeMlsSurface.h"
#include "ExpeStaticInitializer.h"

namespace Expe
{

MlsSurface::MlsSurface(ConstPointSetPtr pPoints)
    : mpInputPoints(pPoints)
{
    AxisAlignedBox aabb = pPoints->computeAABB();
    mObjectScale = (aabb.max()-aabb.min()).length();
    mProjectionAccuracy = 1e-3;
    mMaxNofProjectionIterations = 20;
    
    mWeightingFunction = 0;
    mNeighborhood = 0;
}

MlsSurface::~MlsSurface()
{
}

EXPE_SINGLETON_IMPLEMENTATION_AUTO(MlsSurfaceManager);

}

//#include "ExpeMlsSurface.moc"
