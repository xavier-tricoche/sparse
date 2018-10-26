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



#include "ExpeRadiusEvaluatorFromDensity.h"

#include "ExpeStaticInitializer.h"
#include "ExpeKdTree.h"
#include "ExpeQueryGrid.h"

namespace Expe
{

EXPE_INIT_CODE(RadiusEvaluatorFromDensity) {
    GeometryOperatorManager::InstanceSafe().registerFactory(new GeometryOperatorFactoryCreatorMatch1<RadiusEvaluatorFromDensity,PointSet>());
}

RadiusEvaluatorFromDensity::RadiusEvaluatorFromDensity(GeometryObject* pInput)
    : GeometryOperator(pInput)
{
    mIsInputAsOutputDoable = true;
    mInputAsOutput = true;
    
    mNofNeighbors = 16;
}

RadiusEvaluatorFromDensity::~RadiusEvaluatorFromDensity(void)
{
}


bool RadiusEvaluatorFromDensity::apply(void)
{
    PointSet* pPoints = dynamic_cast<PointSet*>(mpInput);
    if (!pPoints)
        return false;
    
    if (!mInputAsOutput)
        pPoints = pPoints->clone();
    mpOutput = pPoints;
    
    if (!pPoints->hasAttribute(PointSet::Attribute_radius))
        pPoints->addAttribute(PointSet::Attribute_radius);
    
    KdTree kdtree(pPoints, 16);
    kdtree.setMaxNofNeighbors(mNofNeighbors);
    
    for (uint i=0, end=pPoints->size() ; i<end ; ++i)
    {
        kdtree.doQueryK(pPoints->at(i).position());
        pPoints->at(i).radius() = 2. * Math::Sqrt(kdtree.getNeighborSquaredDistance(0)/Real(kdtree.getNofFoundNeighbors()));
    }
    return true;
}

}

//#include "ExpeRadiusEvaluatorFromDensity.moc"
