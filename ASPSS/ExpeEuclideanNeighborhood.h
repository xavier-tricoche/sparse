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



#ifndef _ExpeEuclideanNeighborhood_h_
#define _ExpeEuclideanNeighborhood_h_

#include "ExpeNeighborhood.h"

namespace Expe
{

class QueryGrid;

/** Neighborhood defined by an Euclidean ball of constant radius.
*/

class EuclideanNeighborhood : public Neighborhood
{
//Q_OBJECT

  //  Q_PROPERTY(float     FilterRadius       READ getFilterRadius       WRITE setFilterRadius      DESIGNABLE true STORED true);

public:

    EuclideanNeighborhood(ConstPointSetPtr pPoints);
    
    virtual ~EuclideanNeighborhood();
    
    virtual void computeNeighborhood(const Vector3& p, const WeightingFunction* pWeightingFunc = 0);
    
    QUICK_MEMBER(Real,FilterRadius);
    
protected:
    
    QueryGrid* mGrid;
};


}

#endif

