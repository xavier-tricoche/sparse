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



#include "ExpeBallNeighborhood.h"
#include "ExpeWeightingFunction.h"
#include "ExpeStaticInitializer.h"
//#include "ExpeLazzyUi.h"

namespace Expe
{
	/*
ELU_DECLARE(BallNeighborhood,
    ELU_PROPERTY("FilterScale",ELU_OPT("description",ELU_STR("Scale factor of the sample radii. (should be around 2.0)")) ELU_OPT("min",0.) ELU_OPT("step",0.1))
);

EXPE_INIT_CODE(BallNeighborhood) {
//    NeighborhoodManager::InstanceSafe().registerFactory(new NeighborhoodFactoryCreator<BallNeighborhood>());
}*/

BallNeighborhood::BallNeighborhood(ConstPointSetPtr pPoints)
    : Neighborhood(pPoints), mNeighborQueue(mNeighborList), mTargetCellSize(24)
{
    mFilterScale = 2.;
    
    mNeighborList.resize(1024);
    mWeights.resize(12);
    mDerivativeWeights.resize(12);
    
    mpNeighborList = &mNeighborList;
    
    mCachedFilterScale = 0.;
    mRootNode = 0;
}

BallNeighborhood::~BallNeighborhood()
{
    delete mRootNode;
}

void BallNeighborhood::rebuild(void)
{
    delete mRootNode;
    
    mRootNode = new Node();
    IndexArray indices(mpPoints->size());
    mScales.resize(mpPoints->size());
	//mScalarScales.resize(mpPoints->size());
    AxisAlignedBox aabb;
    for (uint i=0 ; i<mpPoints->size() ; ++i)
    {
        indices[i] = i;
        aabb.min().makeFloor(mpPoints->at(i).position() - mpPoints->at(i).radius()*mFilterScale);
        aabb.max().makeCeil(mpPoints->at(i).position() + mpPoints->at(i).radius()*mFilterScale);
        mScales[i] = 1./(mpPoints->at(i).radius()*mFilterScale);
        mScales[i] = mScales[i] * mScales[i];

		//mScalarScales[i] = 1./(mpPoints->at(i).scalarradius()*mFilterScale);
        //mScalarScales[i] = mScalarScales[i] * mScalarScales[i];
    }
    createTree(*mRootNode, indices, aabb);
    
    mCachedFilterScale = mFilterScale;
}

void BallNeighborhood::computeNeighborhood(const Vector3& p, const WeightingFunction* pWeightingFunc)
{
    if (mCachedFilterScale != mFilterScale)
    {
        rebuild();
    }
    
    mNeighborQueue.init();
    mQueryPosition = p;
    queryNode(*mRootNode);
    mNofFoundNeighbors = mNeighborQueue.getNofElements();
    
    if (mNofFoundNeighbors>0)
    {
        mCurrentFilterRadius = Math::Sqrt(getNeighborSquaredDistance(0));
        
        if (mNofFoundNeighbors>mWeights.size())
            mWeights.resize(mNofFoundNeighbors+10); // +10 => pre-emption
        if (mNofFoundNeighbors>mDerivativeWeights.size())
            mDerivativeWeights.resize(mNofFoundNeighbors+10);
        
        if (pWeightingFunc)
        {
			for (uint i=0 ; i<mNofFoundNeighbors ; ++i)
                mWeights[i] = mScales[this->getNeighborId(i)] * this->getNeighborSquaredDistance(i);
            pWeightingFunc->computeDerivativeWeights(mWeights, mDerivativeWeights);
            pWeightingFunc->computeWeights(mWeights, mWeights);
            for (uint i=0 ; i<mNofFoundNeighbors ; ++i)
                mDerivativeWeights[i] *= mScales[this->getNeighborId(i)];
        }
    }
}

void BallNeighborhood::queryNode(Node& node)
{
    if (node.leaf)
    {
        for (uint i=0 ; i<node.size ; ++i)
        {
            Real d2 = mQueryPosition.squaredDistanceTo(mpPoints->at(node.indices[i]).position());
            if (d2*mScales[node.indices[i]]<1.)
            {
                mNeighborQueue.insert(node.indices[i], d2);
            }
        }
    }
    else
    {
        if (mQueryPosition[node.dim] - node.splitValue < 0)
            queryNode(*node.children[0]);
        else
            queryNode(*node.children[1]);
    }
}

void BallNeighborhood::split(const IndexArray& indices, const AxisAlignedBox& aabbLeft, const AxisAlignedBox& aabbRight, IndexArray& iLeft, IndexArray& iRight)
{
    for (IndexArray::const_iterator it=indices.begin(), end=indices.end() ; it!=end ; ++it)
    {
        PointSet::ConstPointHandle pt = mpPoints->at(*it);
        if (aabbLeft.distanceTo(pt.position()) < pt.radius()*mFilterScale)
            iLeft.push_back(*it);
        
        if (aabbRight.distanceTo(pt.position()) < pt.radius()*mFilterScale)
            iRight.push_back(*it);
    }
}

void BallNeighborhood::createTree(Node& node, IndexArray& indices, AxisAlignedBox aabb)
{
    //
    Real avgradius = 0.;
    for (IndexArray::const_iterator it=indices.begin(), end=indices.end() ; it!=end ; ++it)
        avgradius += mpPoints->at(*it).radius();
    avgradius /= Real(indices.size());
    Vector3 diag = aabb.max() - aabb.min();
    if (indices.size()<mTargetCellSize || avgradius*0.9 > diag.maxComponent())
    {
        if (indices.size()>200)
		{
			//printf("Leaf cell is huge %d since average radius is %lf\n", indices.size(), avgradius);
           // getchar();
		}
        node.leaf = 1;
        node.size = indices.size();
        node.indices = new uint[node.size];
        for (uint i=0 ; i<node.size ; ++i)
            node.indices[i] = indices[i];
        return;
    }
	node.leaf = 0; //Samer this line was added by me
    uint dim = diag.maxComponentId();
    node.dim = dim;
    node.splitValue = 0.5*(aabb.max()[dim] + aabb.min()[dim]);
    
    AxisAlignedBox aabbLeft=aabb, aabbRight=aabb;
    aabbLeft.max()[dim] = node.splitValue;
    aabbRight.min()[dim] = node.splitValue;
    
    IndexArray iLeft, iRight;
    split(indices, aabbLeft, aabbRight, iLeft,iRight);
    
    {
        // we don't need the index list anymore
        indices.clear();

        {
            // left child
            //mNodes.resize(mNodes.size()+1);
            Node* pChild = new Node();
            node.children[0] = pChild;
            pChild->leaf = 0;
            createTree(*pChild, iLeft, aabbLeft);
        }
        
        {
            // right child
            //mNodes.resize(mNodes.size()+1);
            Node* pChild = new Node();
            node.children[1] = pChild;
            pChild->leaf = 0;
            createTree(*pChild, iRight, aabbRight);
        }
    }
}

}

//#include "ExpeBallNeighborhood.moc"

