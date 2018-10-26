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



#include "ExpeQueryDataStructure.h"

namespace Expe
{

QueryDataStructure::QueryDataStructure(const PointSet* pPoints)
    : mpPoints(pPoints)
{
    // init with an improbable position !
    mQueriedPoint = Vector3(1e99, 1e99, 1e99);
    mNofFoundNeighbors = 0;
    setMaxNofNeighbors(1);
}

QueryDataStructure::~QueryDataStructure()
{
}


Real QueryDataStructure::getAverageDistance(void)
{
    return mAvgDistance;
}


// PointSet::Point QueryDataStructure::getNearestNeighbor(const Vector3& p)
// {
//     return mpPoints->at(getNearestNeighborId(p));
// }
// Index QueryDataStructure::getNearestNeighborId(const Vector3& p)
// {
//     if((p.squaredDistance(mQueriedPoint)>1e-6) && (getNofFoundNeighbors()>=1))
//     {
//         uint oldMaxNofNeighbors = mMaxNofNeighbors;
//         setMaxNofNeighbors(1);
//         Real r0=getAverageDistance();
//         Real r = r0;
//         do
//         {
//             doQueryBallv(p, r);
//             r *= 2.;
//         } while( (getNofFoundNeighbors()>=1) && (r<1e3 * r0) );
//         setMaxNofNeighbors(oldMaxNofNeighbors);
//     }
//     
//     assert(getNofFoundNeighbors()!=0 && "NearestNeighbor not find !");
//     
// //     if(getNofFoundNeighbors()==0)
// //     {
// //         // return an invalid index
// //         LOG_DEBUG("NearestNeighbor not find ! (return an invalid index)")
// //         return EXPE_INVALID_INDEX_VALUE;
// //     }
//     
//     
//     return getNeighborId(0);
// }
// 
// PointSet::Point QueryDataStructure::getNearestNeighborv(const Vector3& p)
// {
//     return getNearestNeighbor(p);
// }
// 
// Index QueryDataStructure::getNearestNeighborIdv(const Vector3& p)
// {
//     return getNearestNeighborId(p);
// }


void QueryDataStructure::sortNeighbors(void)
{
    NeighborList::iterator end = mNeighborList.begin();
    end += getNofFoundNeighbors();
    std::sort(mNeighborList.begin(),end);
}



} // namespace

