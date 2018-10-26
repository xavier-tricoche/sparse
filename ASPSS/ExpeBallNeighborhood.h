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



#ifndef _ExpeBallNeighborhood_h_
#define _ExpeBallNeighborhood_h_

#include "ExpeNeighborhood.h"

namespace Expe
{

class QueryGrid;

/** Neighborhood defined by an Euclidean ball of constant radius.
*/

class BallNeighborhood : public Neighborhood
{
//Q_OBJECT

  //  Q_PROPERTY(float FilterScale READ getFilterScale WRITE setFilterScale DESIGNABLE true STORED true);

public:

    BallNeighborhood(ConstPointSetPtr pPoints);
    
    virtual ~BallNeighborhood();
    
    virtual void computeNeighborhood(const Vector3& p, const WeightingFunction* pWeightingFunc = 0);
    
    QUICK_MEMBER(Real,FilterScale);
    
protected:

    struct Node
    {
        ~Node()
        {
            if (!leaf)
            {
                delete (children[0]);
                delete (children[1]);
            }
            else
            {
                delete[] indices;
            }
        }
        Real splitValue;
        ubyte dim;
        ubyte leaf;
        union {
            Node* children[2];
            struct {
                uint* indices;
                uint size;
            };
        };
    };
    
    void rebuild(void);
    void createTree(Node& node, IndexArray& indices, AxisAlignedBox aabb);
    void split(const IndexArray& indices, const AxisAlignedBox& aabbLeft, const AxisAlignedBox& aabbRight, IndexArray& iLeft, IndexArray& iRight);
    void queryNode(Node& node);
    Vector3 mQueryPosition;
    
protected:

    typedef std::vector< PriorityQueueElement<Index,Real> > NeighborList;
    NeighborList mNeighborList;
    
    typedef MaxPriorityQueueWrapper<NeighborList> NeighborPriorityQueue;
    NeighborPriorityQueue mNeighborQueue;

    uint mTargetCellSize;
    Node* mRootNode;
    Real mCachedFilterScale;
    std::vector<Real> mScales;

	std::vector<Real> mScalarScales;
};


}

#endif

