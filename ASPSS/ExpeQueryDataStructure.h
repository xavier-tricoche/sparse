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



#ifndef _ExpeQueryDataStructure_h_ 
#define _ExpeQueryDataStructure_h_

#include "ExpePrerequisites.h"
#include "ExpePrioritySet.h"
#include "ExpePointSet.h"

namespace Expe
{

/** Base class to perform basic spatial queries on a set of 3D points
*/
class QueryDataStructure
{

public:

    //QueryDataStructure(ConstPointSetPtr pPoints);
    QueryDataStructure(const PointSet* pPoints);
    
    virtual ~QueryDataStructure();
    
    inline const PointSet* getPoints(void) const {return mpPoints;}
    
    /** Specifies the maximal number of neighbors.
        0 means no limits.
    */
    inline void setMaxNofNeighbors(uint nb);
    
    /** Returns the maximal number of neighbors.
        0 means no limits.
    */
    inline uint getMaxNofNeighbors(void) const;
    
    /** Returns a coarse estimation of the resolution of the point cloud.
    */
    virtual Real getAverageDistance(void);
    
    /** Search all points which are in the sphere of center p and radius dMax.
    */
    virtual void doQueryBall(const Vector3& p, Real dMax) = 0;
    
    /** Search the k nearest neighbor of p where k is set by setMaxNofNeighbors().
    */
    virtual void doQueryK(const Vector3& p) = 0;
    
    inline uint getNofFoundNeighbors(void) const;
    inline PointSet::ConstPointHandle getNeighbor(uint i) const;
    inline Index getNeighborId(uint i) const;
    inline Real getNeighborSquaredDistance(uint i) const;
    
    void sortNeighbors(void);
    
    typedef std::vector< PriorityQueueElement<Index,Real> > NeighborList;
    
    const NeighborList* getNeighborList(void) const {return &mNeighborList;}
    
protected:
    
    NeighborList mNeighborList;
    uint mNofFoundNeighbors;
    
    //ConstPointSetPtr mpPoints;
    const PointSet* mpPoints;
    
    Vector3 mQueriedPoint;
    uint mMaxNofNeighbors;
    Real mAvgDistance;
};

#include "ExpeQueryDataStructure.inl"

}

#endif

