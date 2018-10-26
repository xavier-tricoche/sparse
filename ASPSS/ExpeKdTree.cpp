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



#include "ExpeKdTree.h"
#include <stack>
#include <limits>

namespace Expe
{

#ifdef EXPE_KDTREE_EMBEDEDPOSITION
    #define EL_POSITION(EL)    (EL).position
    #define EL_INDEX(EL)       (EL).index
#else
    #define EL_POSITION(EL)    mpPoints->at((EL).index).position()
    #define EL_INDEX(EL)       (EL).index
#endif

KdTree::KdTree(const PointSet* pPoints, uint nofElementsPerCell)
 : QueryDataStructure(pPoints), mNeighborQueue(mNeighborList), mTargetCellSize(nofElementsPerCell)
{
    // fill the elements
    mElements.resize(mpPoints->size());
    for (uint i=0, nb=mpPoints->size(); i<nb ; ++i)
    {
        mElements[i].index = i;
        #ifdef EXPE_KDTREE_EMBEDEDPOSITION
            mElements[i].position = mpPoints->at(i).position();
        #endif
    }
    mAABB = pPoints->computeAABB();
    mRootNode = new Node();
	mRootNode->leaf = 0; // Samer added this line
    createTree(*mRootNode, 0, mpPoints->size());
    
    mNeighborQueue.init();
}

KdTree::~KdTree()
{
    delete mRootNode;
}

void KdTree::doQueryBall(const Vector3& p, Real dMax)
{
    mNeighborQueue.init();
    mNeighborQueue.insert(0xffffffff,dMax*dMax);
    
    mQueryPosition = p;
    mQueryOffset = Vector3::ZERO;
    queryNode(*mRootNode, mAABB.squaredDistanceTo(p));
    
    if (mNeighborQueue.getTopIndex() == 0xffffffff)
        mNeighborQueue.removeTop();
    
    mNofFoundNeighbors = mNeighborQueue.getNofElements();
}

void KdTree::doQueryK(const Vector3& p)
{
    mNeighborQueue.init();
    mNeighborQueue.insert(0xffffffff,std::numeric_limits<float>::max());
    
    #if 1
    mQueryPosition = p;
    mQueryOffset = Vector3::ZERO;
    queryNode(*mRootNode, mAABB.squaredDistanceTo(p));
    
    #else
    std::stack<QueryNode> nodeStack;
    //nodeStack.reserve(20);
    nodeStack.push(QueryNode(mRootNode,mAABB.squaredDistanceTo(p)));
    
    Vector3 offset(Vector3::ZERO);
    
    while (!nodeStack.empty())
    {
        QueryNode& qnode = nodeStack.top();
        Node& node = *qnode.pNode;
        
        if (node.leaf)
        {
            for (uint i=node.start ; i<node.end ; ++i)
            {
                mNeighborQueue.insert(EL_INDEX(mElements[i]), p.squaredDistanceTo(EL_POSITION(mElements[i])));
            }
            nodeStack.pop();
        }
        else
        {
            // for a non-leaf node we have three states
            if (qnode.nextChildId & 0x2)
            {
                // 1 - this is the second visit, check if have to process the second child
                if (qnode.sqDist < 2.1*mNeighborQueue.getTopWeight())
                {
                    offset[node.dim] = qnode.newOffset;
                    nodeStack.push(QueryNode(qnode.pNode->children[uint(qnode.nextChildId & 0x1)], qnode.sqDist));
                    qnode.nextChildId = 0x4;
                }
                else
                {
                    // restore and pop
                    offset[node.dim] = qnode.oldOffset;
                    nodeStack.pop();
                }
            }
            else if (qnode.nextChildId & 0x4)
            {
                // 2 - this is the third visit
                // restore and pop
                offset[node.dim] = qnode.oldOffset;
                nodeStack.pop();
            }
            else
            {
                // 3 - this is the first visit
                qnode.oldOffset = offset[node.dim];
                qnode.newOffset = p[node.dim] - node.splitValue;
                if (qnode.newOffset < 0)
                {
                    nodeStack.push(QueryNode(node.children[0],qnode.sqDist));
                    qnode.nextChildId = 1 | 0x2;
                }
                else
                {
                    nodeStack.push(QueryNode(node.children[1],qnode.sqDist));
                    qnode.nextChildId = 0 | 0x2;
                }
                qnode.sqDist += qnode.newOffset*qnode.newOffset - qnode.oldOffset*qnode.oldOffset;
            }
        }
    }
    #endif

    if (mNeighborQueue.getTopIndex() == 0xffffffff)
        mNeighborQueue.removeTop();
    
    mNofFoundNeighbors = mNeighborQueue.getNofElements();
}

void KdTree::queryNode(Node& node, Real sq)
{
    if (node.leaf)
    {
        for (uint i=node.start ; i<node.end ; ++i)
        {
            mNeighborQueue.insert(EL_INDEX(mElements[i]), mQueryPosition.squaredDistanceTo(EL_POSITION(mElements[i])));
        }
    }
    else
    {
        Real old_off = mQueryOffset[node.dim];
        Real new_off = mQueryPosition[node.dim] - node.splitValue;
        if (new_off < 0)
        {
            queryNode(*node.children[0],sq);
            sq = sq - old_off*old_off + new_off*new_off;
            if (sq < mNeighborQueue.getTopWeight())
            {
                mQueryOffset[node.dim] = new_off;
                queryNode(*node.children[1], sq);
                mQueryOffset[node.dim] = old_off;
            }
        }
        else
        {
            queryNode(*node.children[1],sq);
            sq = sq - old_off*old_off + new_off*new_off;
            if (sq < mNeighborQueue.getTopWeight())
            {
                mQueryOffset[node.dim] = new_off;
                queryNode(*node.children[0], sq);
                mQueryOffset[node.dim] = old_off;
            }
        }
    }
}

void KdTree::split(uint start, uint end, uint dim, float splitValue, uint &midId)
{
    uint l(start), r(end-1);
    for ( ; l<r ; ++l, --r)
    {
        while (l < end && EL_POSITION(mElements[l])[dim] < splitValue)
            l++;
        while (r >= 0 && EL_POSITION(mElements[r])[dim] >= splitValue)
            r--;
        if (l > r)
            break;
        std::swap(mElements[l],mElements[r]);
    }
    midId = EL_POSITION(mElements[l])[dim] < splitValue ? l+1 : l;
}

void KdTree::createTree(Node &node, uint start, uint end)
{
    AxisAlignedBox aabb;
    for (uint i=start ; i<end ; ++i)
        aabb.extend(EL_POSITION(mElements[i]));
    Vector3 diag = aabb.max() - aabb.min();
    uint dim = diag.maxComponentId();
    node.dim = dim;
    
    node.splitValue = 0.5*(aabb.max()[dim] + aabb.min()[dim]);
    uint midId;
    split(start,end,dim,node.splitValue,midId);
    
    {
        // left child
        //mNodes.resize(mNodes.size()+1);
        Node* pChild = new Node();
        node.children[0] = pChild;
        if (midId-start <= mTargetCellSize)
        {
            pChild->leaf = 1;
            pChild->start = start;
            pChild->end = midId;
        }
        else
        {
            pChild->leaf = 0;
            createTree(*pChild, start, midId);
        }
    }
    
    {
        // right child
        //mNodes.resize(mNodes.size()+1);
        Node* pChild = new Node();
        node.children[1] = pChild;
        if (end-midId <= mTargetCellSize)
        {
            pChild->leaf = 1;
            pChild->start = midId;
            pChild->end = end;
        }
        else
        {
            pChild->leaf = 0;
            createTree(*pChild, midId, end);
        }
    }
}

}

