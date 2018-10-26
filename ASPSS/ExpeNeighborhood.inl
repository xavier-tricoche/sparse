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



inline uint Neighborhood::getNofFoundNeighbors(void) const
{
    return mNofFoundNeighbors;
}

inline PointSet::ConstPointHandle Neighborhood::getNeighbor(uint i) const
{
    assert(i<getNofFoundNeighbors());
    
    return mpPoints->at(mpNeighborList->at(i).index);
}

inline Index Neighborhood::getNeighborId(uint i) const
{
    assert(i<getNofFoundNeighbors());
    
    return mpNeighborList->at(i).index;
}


inline Real Neighborhood::getNeighborSquaredDistance(uint i) const
{
    assert(i<getNofFoundNeighbors());
    
    return mpNeighborList->at(i).weight;
}

inline Real Neighborhood::getNeighborWeight(uint i) const
{
    assert(i<getNofFoundNeighbors());
    assert(i<mWeights.size());
    
    return mWeights[i];
}

inline Real Neighborhood::getNeighborDerivativeWeight(uint i) const
{
    assert(i<getNofFoundNeighbors());
    assert(i<mDerivativeWeights.size());
    
    return mDerivativeWeights[i];
}

