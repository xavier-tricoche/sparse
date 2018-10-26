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



#include "ExpeQueryGrid.h"

#include "ExpePointSet.h"

namespace Expe
{

#define EXPE_QUERYGRID_BRUTEFORCE

QueryGrid::QueryGrid(const PointSet* pPoints, int nofBits /*=-1*/, Real diam /*=-1*/, bool autoInsertion /*= true*/)
    : QueryDataStructure(pPoints), mNeighborQueue(mNeighborList)
{
    if(nofBits<0 || diam<0.)
    {
        // compute the diameter and the number of bit automatically
        Vector3 min, max;
        min = max = pPoints->begin()->position();
        Real resolution = 0.;
        for(PointSet::const_iterator point_it = pPoints->begin() ; point_it!=pPoints->end() ; ++point_it)
        {
            min.makeFloor(point_it->position());
            max.makeCeil(point_it->position());
            resolution += point_it->radius();
        }
        resolution /= pPoints->size();
        Vector3 vDiam = max-min;
        diam = vDiam.maxComponent();

        nofBits=0;
        Real width = 0.25*diam;
        do
        {
            width *= 0.5;
            ++nofBits;
        } while(width>1.*resolution && nofBits<7);
    }

    // construct the grid
    int nofCells;

    mNofBits = nofBits;
    mMask = getNofCellsPerAxis() - 1;
    nofCells = getNofCells();
    mCells = 0;
    
    mCellSize = diam / (Real)getNofCellsPerAxis();
    mCellSize_1 = 1./mCellSize;
    mDiam = diam;
    mAvgRadius = mCellSize;

    mNofCellsPerAxis = getNofCellsPerAxis();

    mCells = new Element*[nofCells];
    for (int i=0; i<nofCells; i++)
    {
        mCells[i] = 0;
    }

    mNeighborQueue.init();

    // insert all points
    if(autoInsertion)
    {
        insertAll();
    }
}

QueryGrid::~QueryGrid()
{
    clear();
    DESTROY_TAB(mCells);
}


void QueryGrid::clear(void)
{
    if (mCells)
    {
        uint nofCells = getNofCells();
        for (uint i=0; i<nofCells; i++)
        {
            for (Element *e=mCells[i]; e; )
            {
                Element* tmp = e->next;
                delete e;
                e = tmp;
            }
        }
        
        for (uint i=0; i<nofCells; i++)
        {
            mCells[i] = 0;
        }
    }
}

void QueryGrid::insertAll(void)
{
    clear();
    
    uint i=0;
    for(PointSet::const_iterator point_it = mpPoints->begin() ; point_it!=mpPoints->end() ; ++point_it, ++i)
    {
        Element **b;
        b = getCell(point_it->position());
        Element *e = new Element;
        ELEMENT_2_INDEX(*e) = i;
        #ifdef EXPE_QUERY_DS_EMBEDED
        ELEMENT_2_POSITION(*e) = point_it->position();
        #endif
        e->next = *b;
        *b = e;
    }

//     // comptute and print stats
//     uint nofCells = getNofCells();
//     int nofPtPerCell, nofPtPerCellMin, nofPtPerCellMax, nofNotEmtyCell;
//     nofPtPerCell = 0;
//     nofPtPerCellMin = mpPoints->size();
//     nofPtPerCellMax = 0;
//     nofNotEmtyCell = 0;
// 
//     for (uint i=0; i<nofCells; i++)
//     {
//         int nofElements = 0;
//         for (Element *e=mCells[i]; e; e=e->next)
//         {
//             nofElements++;
//         }
//         if(nofElements)
//         {
//             nofPtPerCell += nofElements;
//             nofNotEmtyCell++;
//             nofPtPerCellMin = Math::Min(nofPtPerCellMin, nofElements);
//             nofPtPerCellMax = Math::Max(nofPtPerCellMax, nofElements);
//         }
//     }
//     std::cout << "********************************" << std::endl;
//     std::cout << "NofElements = " << nofPtPerCell << " (" << mpPoints->size() << ")" << std::endl;
//     std::cout << "NofElementsPerSell = " << nofPtPerCell/nofNotEmtyCell << std::endl;
//     std::cout << "MinMax = " << nofPtPerCellMin << " / " << nofPtPerCellMax << std::endl;
//     std::cout << "NofNotEmtyCell = " << nofNotEmtyCell << " (" << getNofCells() << ")" << std::endl;
//     std::cout << "********************************" << std::endl;
}

void QueryGrid::doQueryBall(const Vector3& p, Real dMax)
{
    mNeighborQueue.init();

    Real d2Max = dMax*dMax;
    Real sqrDist;
    Vector3 vAux;

    // indices of cell containing the point p
    #ifndef EXPE_QUERYGRID_BRUTEFORCE
    Vector3i c = _floorf(p*mCellSize_1);
    #endif

    Vector3i c0, c1;
    c0 = _floorf((p - dMax) * mCellSize_1);
    c1 = _floorf((p + dMax) * mCellSize_1);
    
    int size = (c1-c0).maxComponent() + 1;

    if (size==1)
    {
        // the sphere fits in a single cell
        Element *e = *getCell(c0.x & mMask, c0.y & mMask, c0.z & mMask);
        for(; e; e=e->next)
        {
            sqrDist = ELEMENT_2_POSITION(*e).squaredDistanceTo(p);
            if (sqrDist < d2Max)
            {
                mNeighborQueue.insert(ELEMENT_2_INDEX(*e), sqrDist);
            }
        }
    }
    else if (size > mNofCellsPerAxis)
    {
        // the sphere contains the whole grid => loop on every cells
        int i;
        int nofCells = getNofCells();
        Element *e;
        for (i=0; i<nofCells; i++)
        {
            for (e=mCells[i]; e; e=e->next)
            {
                sqrDist = ELEMENT_2_POSITION(*e).squaredDistanceTo(p);
                if (sqrDist < d2Max)
                {
                    mNeighborQueue.insert(e->index, sqrDist);
                }
            }
        }
    }
    else
    {
        // visit all cells within bounding box of the sphere
        int x, y, z;
        #ifndef EXPE_QUERYGRID_BRUTEFORCE
        Real dx,dy, dz, r2_x2_y2;
        #endif
        for (x=c0.x ; x<=c1.x ; ++x)
        {
            #ifndef EXPE_QUERYGRID_BRUTEFORCE
            if (x<c.x)
                dx = (x+1)*mCellSize - p.x;
            else if (x>c.x)
                dx = x*mCellSize - p.x;
            else
                dx = 0;
            #endif

            for (y=c0.y ; y<=c1.y ; ++y)
            {
                #ifndef EXPE_QUERYGRID_BRUTEFORCE
                if (y<c.y)
                    dy = (y+1)*mCellSize - p.y;
                else if (y>c.y)
                    dy = y*mCellSize - p.y;
                else
                    dy = 0;

                r2_x2_y2 = d2Max - dx*dx - dy*dy;
                if (r2_x2_y2 < 0)
                    continue;
                #endif

                for (z=c0.z ; z<=c1.z ; ++z)
                {
                    #ifndef EXPE_QUERYGRID_BRUTEFORCE
                    if (z<c.z)
                        dz = (z+1)*mCellSize - p.z;
                    else if (z>c.z)
                        dz = z*mCellSize - p.z;
                    else
                        dz = 0;

                    // skip cells that do not intersect the sphere of radius r
                    if (dz*dz < r2_x2_y2)
                    #endif
                    {
                        Element* e = mCells[(( ((x&mMask)<<mNofBits) | (y&mMask)) <<mNofBits) | (z&mMask)];
                        for ( ; e ; e=e->next)
                        {
                            sqrDist = ELEMENT_2_POSITION(*e).squaredDistanceTo(p);
                            if ( (sqrDist < d2Max) )
                            {
                                mNeighborQueue.insert(e->index, sqrDist);
                            }
                        }
                    }
                }
            }
        }
    }

    mNofFoundNeighbors = mNeighborQueue.getNofElements();
}

void QueryGrid::doQueryBall(uint id, Real dMax)
{
    Vector3 p = mpPoints->at(id).position();

    mNeighborQueue.init();

    Real d2Max = dMax*dMax;
    Real sqrDist;
    Vector3 vAux;

    // indices of cell containing the point p
    #ifndef EXPE_QUERYGRID_BRUTEFORCE
    Vector3i c = _floorf(p*mCellSize_1);
    #endif
    // find bounding box of sphere of radius r around p
    Vector3i c0, c1;
    c0 = _floorf((p - dMax) * mCellSize_1);
    c1 = _floorf((p + dMax) * mCellSize_1);
    
    int size = (c1-c0).maxComponent() + 1;

    if (size==1)
    {
        // the sphere fits in a single cell
        Element *e = *getCell(c0.x & mMask, c0.y & mMask, c0.z & mMask);
        for(; e; e=e->next)
        {
            if(id!=e->index)
            {
                sqrDist = ELEMENT_2_POSITION(*e).squaredDistanceTo(p);
                if (sqrDist < d2Max)
                {
                    mNeighborQueue.insert(ELEMENT_2_INDEX(*e), sqrDist);
                }
            }
        }
    }
    else if (size > mNofCellsPerAxis)
    {
        // the sphere contains the whole grid => loop on every cells
        int i;
        int nofCells = getNofCells();
        Element *e;
        for (i=0; i<nofCells; i++)
        {
            for (e=mCells[i]; e; e=e->next)
            {
                if(id!=e->index)
                {
                    sqrDist = ELEMENT_2_POSITION(*e).squaredDistanceTo(p);
                    if (sqrDist < d2Max)
                    {
                        mNeighborQueue.insert(e->index, sqrDist);
                    }
                }
            }
        }
    }
    else
    {
        // visit all cells within the bounding box of the sphere
        int x, y, z;
        #ifndef EXPE_QUERYGRID_BRUTEFORCE
        Real dx,dy, dz, r2_x2_y2;
        #endif
        for (x=c0.x ; x<=c1.x ; ++x)
        {
            #ifndef EXPE_QUERYGRID_BRUTEFORCE
            if (x<c.x)
                dx = (x+1)*mCellSize - p.x;
            else if (x>c.x)
                dx = x*mCellSize - p.x;
            else
                dx = 0;
            #endif

            for (y=c0.y ; y<=c1.y ; ++y)
            {
                #ifndef EXPE_QUERYGRID_BRUTEFORCE
                if (y<c.y)
                    dy = (y+1)*mCellSize - p.y;
                else if (y>c.y)
                    dy = y*mCellSize - p.y;
                else
                    dy = 0;

                r2_x2_y2 = d2Max - dx*dx - dy*dy;
                if (r2_x2_y2 < 0)
                    continue;
                #endif

                for (z=c0.z ; z<=c1.z ; ++z)
                {
                    #ifndef EXPE_QUERYGRID_BRUTEFORCE
                    if (z<c.z)
                        dz = (z+1)*mCellSize - p.z;
                    else if (z>c.z)
                        dz = z*mCellSize - p.z;
                    else
                        dz = 0;

                    // skip cells that do not intersect the sphere of radius r
                    if (dz*dz < r2_x2_y2)
                    #endif
                    {
                        Element* e = mCells[(( ((x&mMask)<<mNofBits) | (y&mMask)) <<mNofBits) | (z&mMask)];
                        for ( ; e ; e=e->next)
                        {
                            if(id!=e->index)
                            {
                                sqrDist = ELEMENT_2_POSITION(*e).squaredDistanceTo(p);
                                if ( (sqrDist < d2Max) )
                                {
                                    mNeighborQueue.insert(e->index, sqrDist);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    mNofFoundNeighbors = mNeighborQueue.getNofElements();
}

void QueryGrid::doQueryK(const Vector3& p)
{
    uint k = Math::Min(mMaxNofNeighbors, mpPoints->size());
    Real queryRadius = 0.5*mCellSize;
    Real maxQuerySize = getSize();
    do
    {
        doQueryBall(p, queryRadius);
        queryRadius += mCellSize;
    } while(getNofFoundNeighbors()<k && queryRadius<maxQuerySize);
}

} // namespace


