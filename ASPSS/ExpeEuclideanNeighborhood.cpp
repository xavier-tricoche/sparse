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



#include "ExpeEuclideanNeighborhood.h"
#include "ExpeQueryGrid.h"
#include "ExpeWeightingFunction.h"
#include "ExpeStaticInitializer.h"
//#include "ExpeLazzyUi.h"

namespace Expe
{

//ELU_DECLARE(EuclideanNeighborhood,
  //  ELU_PROPERTY("FilterRadius",ELU_OPT("description",ELU_STR("Radius of the weight function")) ELU_OPT("min",0.) ELU_OPT("step",0.1))
//);

//EXPE_INIT_CODE(EuclideanNeighborhood) {
//    NeighborhoodManager::InstanceSafe().registerFactory(new NeighborhoodFactoryCreator<EuclideanNeighborhood>());
//}

EuclideanNeighborhood::EuclideanNeighborhood(ConstPointSetPtr pPoints)
    : Neighborhood(pPoints), mGrid(0)
{
    // estimate the best filter radius
    mFilterRadius = 0.;
    for (PointSet::const_iterator iter=pPoints->begin() ; iter!=pPoints->end() ; ++iter)
        mFilterRadius += iter->radius();
    mFilterRadius *= 2.0/Real(pPoints->size());
    
    LOG_DEBUG(5,"EuclideanNeighborhood::mFilterRadius = " << mFilterRadius);
    
    mGrid = new QueryGrid(pPoints);
    mGrid->setMaxNofNeighbors(1000); // almost no limit !
    mWeights.resize(12);
    mDerivativeWeights.resize(12);
    
    mpNeighborList = mGrid->getNeighborList();
}

EuclideanNeighborhood::~EuclideanNeighborhood()
{
    delete mGrid;
}

void EuclideanNeighborhood::computeNeighborhood(const Vector3& p, const WeightingFunction* pWeightingFunc)
{
    mCurrentFilterRadius = mFilterRadius;
    mGrid->doQueryBall(p, mCurrentFilterRadius);
    
    mNofFoundNeighbors = mGrid->getNofFoundNeighbors();
    if (mNofFoundNeighbors>mWeights.size())
        mWeights.resize(mNofFoundNeighbors+10); // +10 => pre-emption
    if (mNofFoundNeighbors>mDerivativeWeights.size())
        mDerivativeWeights.resize(mNofFoundNeighbors+10);
    
    if (pWeightingFunc && mNofFoundNeighbors>0)
    {
        Real s = 1./(mCurrentFilterRadius*mCurrentFilterRadius);
        for (uint i=0 ; i<mNofFoundNeighbors ; ++i)
            mWeights[i] = s * mGrid->getNeighborSquaredDistance(i);
        pWeightingFunc->computeDerivativeWeights(mWeights, mDerivativeWeights);
        pWeightingFunc->computeWeights(mWeights, mWeights);
        for (uint i=0 ; i<mNofFoundNeighbors ; ++i)
            mDerivativeWeights[i] *= s;
    }
}

}

//#include "ExpeEuclideanNeighborhood.moc"

