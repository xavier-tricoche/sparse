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



#ifndef _ExpeQueryGrid_h_ 
#define _ExpeQueryGrid_h_

#include "ExpePrerequisites.h"
#include "ExpeQueryDataStructure.h"

namespace Expe
{

/** A basic infinite (modulo) grid
*/

class QueryGrid : public QueryDataStructure
{

#define EXPE_QUERYGRID_BRUTEFORCE

#ifdef EXPE_QUERY_DS_EMBEDED
    #define ELEMENT_2_POSITION(EL)    (EL).position
    #define ELEMENT_2_INDEX(EL)       (EL).index
#else
    #define ELEMENT_2_POSITION(EL)    mpPoints->at((EL).index).position()
    #define ELEMENT_2_INDEX(EL)       (EL).index
#endif

protected:
    
    struct Element
    {
        Element(){}
        #ifdef EXPE_QUERY_DS_EMBEDED
        Element(uint id, const Vector3& pos) : index(id), position(pos) {}
        #else
        Element(uint id) : index(id) {}
        #endif
        uint index;
        #ifdef EXPE_QUERY_DS_EMBEDED
        Vector3 position;
        #endif
        Element* next;
    };

public:

    QueryGrid(const PointSet* pPoints, int nofBits = -1, Real diam = -1., bool autoInsertion = true);
    
    virtual ~QueryGrid();
    
    /** Insert the element #id of the point set into the grid.
    */
    inline void insert(uint id)
    {
        assert(id<mpPoints->size());
        
        Element **b;
        Vector3 pos = mpPoints->at(id).position();
        b = getCell(pos);
        Element *e = new Element;
        ELEMENT_2_INDEX(*e) = id;
        #ifdef EXPE_QUERY_DS_EMBEDED
        ELEMENT_2_POSITION(*e) = pos;
        #endif
        e->next = *b;
        *b = e;
    }
    
    /** Remove the element #id of the point set from the grid.
        \warning the element #id must still be in the point set with the correct position.
    */
    inline void remove(uint id)
    {
        assert(id<mpPoints->size());
        
        Element **b;
        Vector3 pos = mpPoints->at(id).position();
        
        b = getCell(pos);
        Element* e = *b;
        if(ELEMENT_2_INDEX(*e)==id)
        {
            *b = e->next;
            delete e;
        }
        else
        {
            bool still = true;
            while(still && e->next)
            {
                if(ELEMENT_2_INDEX(*(e->next))==id)
                {
                    Element* aux = e->next;
                    e->next = aux->next;
                    delete aux;
                    still = false;
                }
                e = e->next;
            }
            assert(!still);
        }
    }
    
    /** Notifies the point #id moved from the position oldPos.
        For instance:
        \code
            Vector3 tmp = pPoints->at(i).position();
            pPoints->at(i).position() = newPosition;
            pGrid->notifyMoveFrom(i,aux);
        \endcode
    */
    inline void notifyMoveFrom(uint id, const Vector3& oldPos)
    {
        Element **oldB;
        oldB = getCell(oldPos);
        
        Vector3 pos = mpPoints->at(id).position();
        Element **b;
        b = getCell(pos);
        
        if (b!=oldB)
        {
            // remove
            Element* e = *oldB;
            if(ELEMENT_2_INDEX(*e)==id)
            {
                *oldB = e->next;
                delete e;
            }
            else
            {
                bool still = true;
                while(still && e->next)
                {
                    if(ELEMENT_2_INDEX(*(e->next))==id)
                    {
                        Element* aux = e->next;
                        e->next = aux->next;
                        delete aux;
                        still = false;
                    }
                    e = e->next;
                }
                assert(!still);
            }
            
            // insert
            e = new Element;
            ELEMENT_2_INDEX(*e) = id;
            #ifdef EXPE_QUERY_DS_EMBEDED
            ELEMENT_2_POSITION(*e) = pos;
            #endif
            e->next = *b;
            *b = e;
        }
    }
    
    /** Remove all samples from the grid.
    */
    void clear(void);
    
    /** Insert all elements of the point set into the grid.
    */
    void insertAll(void);
    
    /** Overloaded method which compute the neighborhood of the i^th points.
        The i^th is not inserted into its own neighborhood.
    */
    void doQueryBall(uint i, Real dMax);

    virtual void doQueryBall(const Vector3& p, Real dMax);
    
    /** Very inefficient method to compute the k-nearest neighbors.
    */
    virtual void doQueryK(const Vector3& p);
    
    inline Element** getCell(int x, int y, int z) const
    {
        return &mCells[(( (x<<mNofBits) | y) <<mNofBits) | z];
    }

    inline Element** getCell(const Vector3& pos)
    {
        return getCell(
            _floorf(pos.x*mCellSize_1) & mMask,
            _floorf(pos.y*mCellSize_1) & mMask,
            _floorf(pos.z*mCellSize_1) & mMask);
    }

    Real getAvgRadius(void) const
    {
        return mAvgRadius;
    }
    
protected:

    inline int getNofCellsPerAxis() const
    {
        return 1<<mNofBits;
    }
    inline int getNofCells() const
    {
        return 1<<(3*mNofBits);
    }
    inline Real getSize() const
    {
        return getNofCellsPerAxis()*mCellSize;
    }
    
    inline int _floorf(Real x)
    {
        return x>0. ? int(x) : int(x) - 1;
    }
    
    inline Vector3i _floorf(Vector3 v)
    {
        return Vector3i(
            v.x>0. ? int(v.x) : int(v.x) - 1,
            v.y>0. ? int(v.y) : int(v.y) - 1,
            v.z>0. ? int(v.z) : int(v.z) - 1);
    }

protected:

    typedef MaxPriorityQueueWrapper<NeighborList> NeighborPriorityQueue;
    NeighborPriorityQueue mNeighborQueue;

    Element **mCells;
	int mNofBits; // number of bits per dimension for hash table
    int mNofCellsPerAxis;
	int mMask; // for hashing, mMask = 2^nbit - 1
	Real mCellSize; // size of a cell
    Real mCellSize_1;
	Real mAvgRadius; // average radius of queries, in world space
	Real mDiam; // diameter of object cloud, in world space

};

}

#endif

