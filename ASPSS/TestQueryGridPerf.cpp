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



#include "ExpeCore.h"
#include "ExpeVector3.h"
#include "ExpeStringHelper.h"

#include "ExpePointSet.h"
#include "ExpeQueryGrid.h"
#include "ExpeKdTree.h"
#include "ExpeTimer.h"


using namespace Expe;

int main(int argc, char* argv[])
{
    // create a random set of points
    PointSet* pPoints = new PointSet(PointSet::Attribute_position);
    uint nb = 100000;
    for (int i=0 ; i<nb ; ++i)
    {
        PointSet::PointHandle pt = pPoints->append();
        pt.position() = (Vector3::Random()-0.5).normalized()*50.;
        pt.radius() = 0.15;
    }
    
    Timer timer;
    
    timer.reset(); timer.start();
    QueryGrid* pGrid = new QueryGrid(pPoints);
    timer.stop(); std::cout << "QueryGrid construction: " << timer.value() << "s\n";
    
    timer.reset(); timer.start();
    KdTree* pKdTree = new KdTree(pPoints,12);
    timer.stop(); std::cout << "KdTree construction:    " << timer.value() << "s\n";
    
    pGrid->setMaxNofNeighbors(12);
    pKdTree->setMaxNofNeighbors(12);

    
    
    for (;;)
    {
        timer.reset();
        timer.start();
        for (int k=0 ; k<100000 ; ++k)
        {
            //pGrid->doQueryK(pPoints->at(k%nb).position());
            pGrid->doQueryBall(pPoints->at(k%nb).position(), 0.15);
//             pGrid->sortNeighbors();
//             pKdTree->doQueryK(pPoints->at(k%nb).position());
//             pKdTree->sortNeighbors();
//             
//             if (pGrid->getNeighborId(pGrid->getNofFoundNeighbors()-1) != pKdTree->getNeighborId(pKdTree->getNofFoundNeighbors()-1))
//             {
//                 for (uint j=0 ; j<pGrid->getNofFoundNeighbors() ; ++j)
//                     std::cout << pGrid->getNeighborId(j)<<","
//                         <<pGrid->getNeighborSquaredDistance(j)/*<<","
//                         <<pGrid->getNeighbor(j).position()*/ << " ";
//                 std::cout << "\n";
//                 for (uint j=0 ; j<pKdTree->getNofFoundNeighbors() ; ++j)
//                     std::cout << pKdTree->getNeighborId(j)<<","
//                         <<pKdTree->getNeighborSquaredDistance(j)/*<<","
//                         <<pKdTree->getNeighbor(j).position()*/ << " ";
//                 std::cout << "\n";
//                 std::cout << "\n";
//             }
        }
        timer.stop();
        std::cout << "Time for querygrid:\t" << timer.value() << "\n";
        
        timer.reset();
        timer.start();
        for (int k=0 ; k<100000 ; ++k)
        {
            pKdTree->doQueryK(pPoints->at(k%nb).position());
        }
        timer.stop();
        std::cout << "Time for kdtree:\t" << timer.value() << "\n";
    }
    return 0;
}
