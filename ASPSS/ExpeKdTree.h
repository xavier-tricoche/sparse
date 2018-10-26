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



#ifndef _ExpeKdTree_h_ 
#define _ExpeKdTree_h_

#include "ExpePrerequisites.h"
#include "ExpeQueryDataStructure.h"

namespace Expe
{

/** A basic kd-tree for k-nearest neighbor queries
*/

// If this token is defined, then the positions of the samples
// are duplicated into the internal KdTree representation.
// On one hand, this leads to faster construction and faster queries
// but on the other hand this increasing the memory consumption
// by 12 bytes * nb samples
#define EXPE_KDTREE_EMBEDEDPOSITION

class KdTree : public QueryDataStructure
{
protected:

    #ifdef EXPE_KDTREE_EMBEDEDPOSITION
    struct Element
    {
        Element(){}
        Element(uint id, const Vector3& pos) : position(pos), index(id) {}
        Vector3 position;
        uint index;
    };
    #else
    struct Element
    {
        Element(){}
        Element(uint id) : index(id) {}
        uint index;
    };
    #endif
    
    struct Node
    {
        ~Node()
        {
            if (!leaf)
            {
                delete children[0];
                delete children[1];
            }
        }
        Real splitValue;
        ubyte dim:2;
        ubyte leaf:1;
        union {
            Node* children[2];
            struct {
                uint start;
                uint end;
            };
        };
    };

public:

    KdTree(const PointSet* pPoints, uint nofElementsPerCell = 6);
    
    virtual ~KdTree();

    virtual void doQueryBall(const Vector3& p, Real dMax);

    virtual void doQueryK(const Vector3& p);

protected:

    struct QueryNode
    {
        QueryNode() {}
        QueryNode(Node* apNode, Real asqDist) : pNode(apNode), sqDist(asqDist), nextChildId(0) {}
        Node* pNode;
        Real oldOffset;
        Real newOffset;
        Real sqDist;
        byte nextChildId;
    };

    void split(uint start, uint end, uint dim, float splitValue, uint &midId);
    void createTree(Node &node, uint start, uint end);
    void queryNode(Node& node, Real sq);
    Vector3 mQueryPosition, mQueryOffset;

protected:

    typedef MaxPriorityQueueWrapper<NeighborList> NeighborPriorityQueue;
    NeighborPriorityQueue mNeighborQueue;

    uint mTargetCellSize;
    //typedef std::vector<Node> NodeList;
    Node* mRootNode;
    AxisAlignedBox mAABB;
    std::vector<Element> mElements;

};

}

#endif

