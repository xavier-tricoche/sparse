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



//--------------------------------------------------------------------------------
// NOTE THAT THIS FILE IS BASED ON MATERIAL FROM:
//
//  OpenMesh
//  Copyright (C) 2004 by Computer Graphics Group, RWTH Aachen
//  http://www.openmesh.org
//
//  This library is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, version 2.1.
//--------------------------------------------------------------------------------

inline HalfedgeConnectivity::HalfedgeId HalfedgeConnectivity::halfedgeId(VertexId vid) const
{
    assert(vid.isValid() && "invalid vertex id");
    assert(uint(vid)<mVertexToHalfedge.size() && "out of range");
    
    return mVertexToHalfedge[vid].halfedgeId;
}

inline HalfedgeConnectivity::HalfedgeId HalfedgeConnectivity::halfedgeId(FaceId fid) const
{
    assert(fid.isValid() && "invalid face id");
    assert(mFaceToHalfedge.isInRange(fid) && "out of range");
    
    return mFaceToHalfedge.at(fid).halfedgeId;
}

inline HalfedgeConnectivity::ConstHalfedgeHandle HalfedgeConnectivity::getHalfedge(HalfedgeId hid) const
{
    assert(hid.isValid() && "invalid halfedge id");
    assert((hid>>1)<mEdges.size() && "out of range");
    
    return mEdges[hid>>1].halfedges[hid&1];
}
inline HalfedgeConnectivity::HalfedgeHandle HalfedgeConnectivity::editHalfedge(HalfedgeId hid)
{
    assert(hid.isValid() && "invalid halfedge id");
    assert((hid>>1)<mEdges.size() && "out of range");
    
    return mEdges[hid>>1].halfedges[hid&1];
}

inline HalfedgeConnectivity::HalfedgeId HalfedgeConnectivity::oppositeHalfedgeId(HalfedgeId hid) const
{
    return HalfedgeId((uint(hid)&1) ? uint(hid)-1 : uint(hid)+1);
}

inline HalfedgeConnectivity::HalfedgeId HalfedgeConnectivity::nextHalfedgeId(HalfedgeId hid) const
{
    return getHalfedge(hid).nextHalfedgeId;
}

inline HalfedgeConnectivity::HalfedgeId HalfedgeConnectivity::previousHalfedgeId(HalfedgeId hid) const
{
    assert(!(getEdgeStatus(edgeId(hid)).isDeleted()));
    
    if (isBoundary(hid))
    {//iterating around the vertex should be faster than iterating the boundary
//         std::cerr << " previousHalfedgeId boundary\n";
        HalfedgeId curr_heh(oppositeHalfedgeId(hid));
        HalfedgeId next_heh(nextHalfedgeId(curr_heh));
        do
        {
            assert(!getEdgeStatus(edgeId(curr_heh)).isDeleted());
            assert(!getEdgeStatus(edgeId(next_heh)).isDeleted());
            curr_heh = oppositeHalfedgeId(next_heh);
            next_heh = nextHalfedgeId(curr_heh);
        } while (next_heh != hid);
        return curr_heh;
    }
    else
    {
//         std::cerr << " previousHalfedgeId simple\n";
        
        HalfedgeId  i(hid);
        HalfedgeId  next(nextHalfedgeId(i));
        while (next != hid) {
            assert(!(getEdgeStatus(edgeId(i)).isDeleted()));
            assert(!(getEdgeStatus(edgeId(next)).isDeleted()));
            i = next;
            next = nextHalfedgeId(next);
        }
        return i;
    }
}

inline HalfedgeConnectivity::HalfedgeId HalfedgeConnectivity::cwRotatedHalfedgeId(HalfedgeId hid) const
{
    return nextHalfedgeId(oppositeHalfedgeId(hid));
}

inline HalfedgeConnectivity::HalfedgeId HalfedgeConnectivity::ccwRotatedHalfedgeId(HalfedgeId hid) const
{
    return oppositeHalfedgeId(previousHalfedgeId(hid));
}

bool HalfedgeConnectivity::isBoundary(VertexId vid) const
{
    HalfedgeId hid(halfedgeId(vid));
    return (!(hid.isValid() && faceId(hid).isValid()));
}

void HalfedgeConnectivity::setFaceToHalfedge(FaceId fid, HalfedgeId hid)
{
    assert(fid.isValid());
    mFaceToHalfedge.at(fid).halfedgeId = hid;
}

void HalfedgeConnectivity::setVertexToHalfedge(VertexId vid, HalfedgeId hid)
{
    assert(vid.isValid());
    mVertexToHalfedge[vid].halfedgeId = hid;
}


