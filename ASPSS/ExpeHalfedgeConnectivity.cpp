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

#include "ExpeHalfedgeConnectivity.h"
#include "ExpeHalfedgeIterators.h"
#include "ExpeLogManager.h"

#include <QFile>
#include <QTextStream>

namespace Expe
{

static const HalfedgeConnectivity::HalfedgeId InvalidHalfedgeId;
static const HalfedgeConnectivity::VertexId InvalidVertexId;
static const HalfedgeConnectivity::FaceId InvalidFaceId;

HalfedgeConnectivity::HalfedgeConnectivity(MeshPtr pMesh, Verbosity verb)
    : mpMesh(pMesh)
{
    mVertexToHalfedge.resize(mpMesh->getNofVertices());
    
    for (uint submeshId=0 ; submeshId<mpMesh->getNofSubMeshes() ; ++submeshId)
    {
        const SubMesh* pSubMesh = mpMesh->getSubMesh(submeshId);
        HalfedgeIdArray* pSubFaceToHalfedge = new HalfedgeIdArray;
        pSubFaceToHalfedge->resize(pSubMesh->getNofFaces());
        mFaceToHalfedge.add(pSubFaceToHalfedge);
        
        for (uint j=0 ; j<pSubMesh->getNofFaces() ; ++j)
        {
            FaceId fid(submeshId,j);
            if (!addFaceInWrapper(fid, verb))
            {
                editFaceStatus(fid).setDeleted(true);
            }
        }
    }
}

HalfedgeConnectivity::~HalfedgeConnectivity()
{
    for (uint subId=0 ; subId<mFaceToHalfedge.getNofSets() ; ++subId)
        delete mFaceToHalfedge.editSet(subId);
}

HalfedgeConnectivity::FaceId HalfedgeConnectivity::createFace(const IndexArray& vertexIds, int subSetId, Verbosity verb)
{
    SubMesh* pSubMesh = 0;
    if (mpMesh->getNofSubMeshes()==0)
    {
        mpMesh->createSubMesh();
    }
    if (subSetId<0 || subSetId>=int(mpMesh->getNofSubMeshes()))
        subSetId = mpMesh->getNofSubMeshes()-1;
    pSubMesh = mpMesh->editSubMesh(subSetId);
    
    FaceHandle face = pSubMesh->createFace(vertexIds.size(),Mesh::None);
    for (uint j=0;j<vertexIds.size();++j)
    {
        face.vertexId(j) = vertexIds[j];
    }
    FaceId fid(subSetId,pSubMesh->getNofFaces()-1);
    if (!addFaceInWrapper(fid,verb))
    {
        // remove the last face and returns an invalid id
        pSubMesh->resize(pSubMesh->getNofFaces()-1);
        return FaceId();
    }
    return fid;
}

void HalfedgeConnectivity::deleteVertex(VertexId vid)
{
    // store incident faces
    std::vector<FaceId> fids;
    fids.reserve(8);
    for (VertexCirculator vf_it(this,vid) ; vf_it; ++vf_it)
    {
        if (vf_it.ccwFaceId().isValid())
            fids.push_back(vf_it.ccwFaceId());
    }
    
    // delete collected faces
    std::vector<FaceId>::iterator f_it(fids.begin()), f_end(fids.end());
    for (; f_it!=f_end; ++f_it)
        deleteFace(*f_it /*, _delete_isolated_vertices*/);
    
    editVertexStatus(vid).setDeleted(true);
}

void HalfedgeConnectivity::deleteFace(FaceId fid)
{
    assert(fid.isValid() && !getFaceStatus(fid).isDeleted());

    // mark face deleted
    editFaceStatus(fid).setDeleted(true);
    
    // this vector will hold all boundary edges of face fid
    // these edges will be deleted
    std::vector<EdgeId> deleted_edges;
    deleted_edges.reserve(3);
    
    // this vector will hold all vertices of face fid
    // for updating their outgoing halfedge
    std::vector<VertexId> vids;
    vids.reserve(3);
    
    // for all halfedges of face fid do:
    //   1) invalidate face handle.
    //   2) collect all boundary halfedges, set them deleted
    //   3) store vertex handles
    for (FaceCirculator fh_it(this,fid) ; fh_it; ++fh_it) {
        HalfedgeId hid = fh_it.halfedgeId();
    
        //set_boundary(hh);//set_face_handle(hh, InvalidFaceHandle);
        setHalfedgeToFace(hid, InvalidFaceId);
    
        if (isBoundary(oppositeHalfedgeId(hid)))
            deleted_edges.push_back(edgeId(hid));
    
        vids.push_back(toVertexId(hid));
        
        hid = nextHalfedgeId(hid);
    }
    
    // delete all collected (half)edges
    // delete isolated vertices (if _delete_isolated_vertices is true)
    if (!deleted_edges.empty())
    {
        std::vector<EdgeId>::iterator del_it(deleted_edges.begin()), del_end(deleted_edges.end());
        HalfedgeId h0, h1, next0, next1, prev0, prev1;
        VertexId v0, v1;
    
        for (; del_it!=del_end; ++del_it)
        {
            h0    = halfedgeId(*del_it, 0);

            v0    = toVertexId(h0);
            next0 = nextHalfedgeId(h0);
            prev0 = previousHalfedgeId(h0);
        
            h1    = halfedgeId(*del_it, 1);
            v1    = toVertexId(h1);
            next1 = nextHalfedgeId(h1);
            prev1 = previousHalfedgeId(h1);
        
            // adjust next and prev handles
            setHalfedgeToHalfedge(prev0, next1);
            setHalfedgeToHalfedge(prev1, next0);
        
            // mark edge deleted
            editEdgeStatus(*del_it).setDeleted(true);
        
            // update v0
            if (halfedgeId(v0) == h1)
            {
                // isolated ?
                if (next0 == h1)
                {
                    //if (_delete_isolated_vertices)
                        editVertexStatus(v0).setDeleted(true);
                    //set_isolated(v0);
                    setVertexToHalfedge(v0,InvalidHalfedgeId);
                }
                else
                    setVertexToHalfedge(v0, next0);
            }
        
            // update v1
            if (halfedgeId(v1) == h0)
            {
                // isolated ?
                if (next1 == h0)
                {
                    //if (_delete_isolated_vertices)
                        editVertexStatus(v1).setDeleted(true);
                    //set_isolated(v1);
                    setVertexToHalfedge(v1,InvalidHalfedgeId);
                }
                else
                    setVertexToHalfedge(v1, next1);
            }
        }
    }
    
    // update outgoing halfedge handles of remaining vertices
    std::vector<VertexId>::iterator v_it(vids.begin()), v_end(vids.end());
    for (; v_it!=v_end; ++v_it)
        adjustOutgoingHalfedge(*v_it);
}

uint HalfedgeConnectivity::computeValence(VertexId vid) const
{
    uint count = 0;
    for (ConstVertexCirculator vv_it(this,vid); vv_it; ++vv_it)
        ++count;
    return count;
}

Vector3 HalfedgeConnectivity::computeNormal(FaceId fid) const
{
    assert(fid.isValid());
    HalfedgeId hid = halfedgeId(fid);
    VertexId vids[3];
    for (uint k=0 ; k<3 ; ++k)
    {
        vids[k] = toVertexId(hid);
        hid = nextHalfedgeId(hid);
    }
    return (getVertex(vids[1]).position() - getVertex(vids[0]).position())
        .cross(getVertex(vids[2]).position() - getVertex(vids[0]).position()).normalized();
}

namespace
{
template <class _Handle>
struct NextCacheEntryT : public std::pair<_Handle, _Handle>
{
  typedef std::pair<_Handle, _Handle> Base;

  NextCacheEntryT(_Handle _heh0, _Handle _heh1)
    : Base(_heh0, _heh1)
  {
    assert(_heh0.isValid());
    assert(_heh1.isValid());
  }
};
}

HalfedgeConnectivity::HalfedgeId HalfedgeConnectivity::createEdge(VertexId v0, VertexId v1)
{
    mEdges.push_back(Edge());
    EdgeId eid(mEdges.size()-1);
    HalfedgeId hid0(halfedgeId(eid, 0));
    HalfedgeId hid1(halfedgeId(eid, 1));
    setHalfedgeToVertex(hid0, v1);
    setHalfedgeToVertex(hid1, v0);
    return hid0;
}

bool HalfedgeConnectivity::addFaceInWrapper(FaceId fid, Verbosity verb)
{
    Mesh::FaceHandle face = mpMesh->editFace(fid);
    uint nb = face.nofVertices();
    
    uint i, ii;
    
    // cache for set_next_halfedge and vetex' set_halfedge
    typedef NextCacheEntryT<HalfedgeId> NextCacheEntry;
    typedef std::vector<NextCacheEntry> NextCache;
    NextCache next_cache;
    next_cache.reserve(3*nb);
    // don't allow degenerated faces
    assert (nb > 2);
    
    HalfedgeId* halfedgeIds = (HalfedgeId*) malloc(nb * sizeof(HalfedgeId));
    bool* isNew = (bool*) malloc(nb * sizeof(bool));
    bool* needs_adjust = (bool*) malloc(nb * sizeof(bool));
    
    // search for old half edge and check manifolness
    for (i=0, ii=1; i<nb; ++i, ++ii, ii%=nb)
    {
        if ( !isBoundary(VertexId(face.vertexId(i))) )
        {
            if (verb!=Quiet)
                LOG_ERROR("HalfedgeConnectivity::addFaceInWrapper: complex vertex");
            return false;
        }
    
        halfedgeIds[i] = findHalfedge(face.vertexId(i), face.vertexId(ii));
        isNew[i] = !halfedgeIds[i].isValid();

        if (halfedgeIds[i].isValid() && !isBoundary(halfedgeIds[i]))
        {
            if (verb!=Quiet)
                LOG_ERROR("HalfedgeConnectivity::addFaceInWrapper: complex edge");
            return false;
        }
    }
    // re-link patches if necessary
    for (i=0, ii=1; i<nb; ++i, ++ii, ii%=nb)
    {
        if (!isNew[i] && !isNew[ii])
        {
            HalfedgeId inner_prev = halfedgeIds[i];
            HalfedgeId inner_next = halfedgeIds[ii];
        
            if (nextHalfedgeId(inner_prev) != inner_next)
            {
                LOG_DEBUG(7,"re-link patches");
                // here comes the ugly part... we have to relink a whole patch
        
                // search a free gap
                // free gap will be between boundary_prev and boundary_next
                HalfedgeId outer_prev = oppositeHalfedgeId(inner_next);
                HalfedgeId outer_next = oppositeHalfedgeId(inner_prev);
                HalfedgeId boundary_prev = outer_prev;
                do {
                    boundary_prev =
                        oppositeHalfedgeId(nextHalfedgeId(boundary_prev));
                } while (!isBoundary(boundary_prev) || boundary_prev==inner_prev);
                
                HalfedgeId boundary_next = nextHalfedgeId(boundary_prev);
                assert(isBoundary(boundary_prev));
                assert(isBoundary(boundary_next));
                // ok ?
                if (boundary_next == inner_next)
                {
                    if (verb!=Quiet)
                        LOG_ERROR("HalfedgeConnectivity::addFaceInWrapper: patch re-linking failed");
                    return false;
                }
        
                // other halfedges' handles
                HalfedgeId patch_start = nextHalfedgeId(inner_prev);
                HalfedgeId patch_end   = previousHalfedgeId(inner_next);
        
                // relink
                next_cache.push_back(NextCacheEntry(boundary_prev, patch_start));
                next_cache.push_back(NextCacheEntry(patch_end, boundary_next));
                next_cache.push_back(NextCacheEntry(inner_prev, inner_next));
            }
        }
    }
    
    // create missing edges
    for (i=0, ii=1; i<nb; ++i, ++ii, ii%=nb)
        if (isNew[i])
            halfedgeIds[i] = createEdge(face.vertexId(i), face.vertexId(ii));
    
    {
        // check that the connectivity is well sync with the IFS mesh
        if (!mFaceToHalfedge.isInRange(fid))
        {
            assert(mpMesh->mEveryFaces.isInRange(fid));
            uint sid = fid.getSetId();
            if (sid<mFaceToHalfedge.getNofSets())
            {
                // simply resize the sub set
                mFaceToHalfedge.editSet(sid)->resize(mpMesh->mEveryFaces.getSet(sid)->size());
            }
            else
            {
                // add the missing sub set(s)
                while (mFaceToHalfedge.getNofSets()<mpMesh->mEveryFaces.getNofSets())
                {
                    mFaceToHalfedge.add( new HalfedgeIdArray(mpMesh->mEveryFaces.getSet(sid)->size()) );
                }
            }
        }
        for (i=0 ; i<nb ; ++i)
        {
            VertexId vid  = face.vertexId(i);
            if (vid>=mVertexToHalfedge.size())
            {
                assert(uint(vid)<mpMesh->getNofVertices());
                mVertexToHalfedge.resize(mpMesh->getNofVertices());
            }
        }
    }
    
    // setup the face
    setFaceToHalfedge(fid, halfedgeIds[nb-1]);
    
    // setup halfedges
    for (i=0, ii=1; i<nb; ++i, ++ii, ii%=nb)
    {
        VertexId vid  = face.vertexId(ii); // outgoing vertex
        HalfedgeId inner_prev = halfedgeIds[i];
        HalfedgeId inner_next = halfedgeIds[ii];
    
        uint id = (isNew[i] ? 1 : 0) | (isNew[ii] ? 2 : 0);
    
        if (id)
        {
            HalfedgeId outer_prev = oppositeHalfedgeId(inner_next);
            HalfedgeId outer_next = oppositeHalfedgeId(inner_prev);
        
            // set outer links
            switch (id)
            {
                case 1: // prev is new, next is old
                {
                    HalfedgeId boundary_prev = previousHalfedgeId(inner_next);
                    next_cache.push_back(NextCacheEntry(boundary_prev, outer_next));
                    setVertexToHalfedge(vid, outer_next);
                    break;
                }
                case 2: // next is new, prev is old
                {
                    HalfedgeId boundary_next = nextHalfedgeId(inner_prev);
                    next_cache.push_back(NextCacheEntry(outer_prev, boundary_next));
                    setVertexToHalfedge(vid, boundary_next);
                    break;
                }
                case 3: // both are new
                {
                    if (!halfedgeId(vid).isValid())
                    {
                        setVertexToHalfedge(vid, outer_next);
                        next_cache.push_back(NextCacheEntry(outer_prev, outer_next));
                    }
                    else
                    {
                        HalfedgeId boundary_next = halfedgeId(vid);
                        HalfedgeId boundary_prev = previousHalfedgeId(boundary_next);
                        next_cache.push_back(NextCacheEntry(boundary_prev, outer_next));
                        next_cache.push_back(NextCacheEntry(outer_prev, boundary_next));
                    }
                    break;
                }
            }
        
            // set inner link
            next_cache.push_back(NextCacheEntry(inner_prev, inner_next));
        }
        else
        {
            needs_adjust[ii] = (halfedgeId(vid) == inner_next);
        }
    
        // set face handle
        setHalfedgeToFace(halfedgeIds[i],fid);
    }
    
    // process next halfedge cache
    NextCache::const_iterator  ncIt(next_cache.begin()), ncEnd(next_cache.end());
    for (; ncIt != ncEnd; ++ncIt)
        setHalfedgeToHalfedge(ncIt->first,ncIt->second);
    
    // adjust vertices' halfedge handle
    for (i=0; i<nb; ++i)
        if (needs_adjust[i])
            adjustOutgoingHalfedge(face.vertexId(i));


	free(halfedgeIds);
    free(isNew);
    free(needs_adjust);


    return true;
}

void HalfedgeConnectivity::adjustOutgoingHalfedge(VertexId vid)
{
    HalfedgeId ref=halfedgeId(vid);
    
    if (ref.isValid())
    {
        HalfedgeId i=ref;
        do {
            if (isBoundary(i))
            {
                setVertexToHalfedge(vid, i);
                break;
            }
            i = cwRotatedHalfedgeId(i);
        } while (i!=ref);
    }
}

HalfedgeConnectivity::HalfedgeId HalfedgeConnectivity::findHalfedge(VertexId v0, VertexId v1) const
{
    assert(v0.isValid() && v1.isValid());
    
    HalfedgeId ref=halfedgeId(v0);
    if (ref.isValid())
    {
        HalfedgeId i=ref;
        do {
            if (toVertexId(i) == v1)
                return i;
            i = nextHalfedgeId(oppositeHalfedgeId(i));
        } while (i!=ref);
    }
    return HalfedgeId();
}





//--------------------------------------------------------------------------------
// EDGE COLLAPSE IMPLEMENTATION
//--------------------------------------------------------------------------------

void HalfedgeConnectivity::collapse(HalfedgeId hid)
{
    if (getEdgeStatus(edgeId(hid)).isDeleted())
        return;
    
    HalfedgeId h0 = hid;
    HalfedgeId h1 = nextHalfedgeId(h0);
    HalfedgeId o0 = oppositeHalfedgeId(h0);
    HalfedgeId o1 = nextHalfedgeId(o0);
    
    // remove edge
    collapse_edge(h0);

    // remove loops
    HalfedgeId nh1 = nextHalfedgeId(h1);
    if (nextHalfedgeId(nh1) == h1)
        //collapse_loop(h1);
        collapse_loop(nh1);
    
    if (nextHalfedgeId(nextHalfedgeId(o1)) == o1)
        collapse_loop(o1);
}

//-----------------------------------------------------------------------------
void HalfedgeConnectivity::collapse_edge(HalfedgeId hid)
{
    HalfedgeId  h  = hid;
    HalfedgeId  hn = nextHalfedgeId(h);
    HalfedgeId  hp = previousHalfedgeId(h);
    
    HalfedgeId  o  = oppositeHalfedgeId(h);
    HalfedgeId  on = nextHalfedgeId(o);
    HalfedgeId  op = previousHalfedgeId(o);
    
    FaceId      fh = faceId(h);
    FaceId      fo = faceId(o);
    
    VertexId    vh = toVertexId(h);
    VertexId    vo = toVertexId(o);
    
    // halfedge -> vertex
    for (VertexCirculator vih_it(this,vo); vih_it; ++vih_it)
        setHalfedgeToVertex(vih_it.oppositeHalfedgeId(), vh);
    
    // halfedge -> halfedge
    setHalfedgeToHalfedge(hp, hn);
    setHalfedgeToHalfedge(op, on);
    
    // face -> halfedge
    if (fh.isValid())
        setFaceToHalfedge(fh, hn);
    if (fo.isValid())
        setFaceToHalfedge(fo, on);
    
    // vertex -> halfedge
    if (halfedgeId(vh) == o)
        setVertexToHalfedge(vh, hn);
    
    adjustOutgoingHalfedge(vh);
    mVertexToHalfedge[vo].halfedgeId.invalidate();
    
    // delete stuff
    editEdgeStatus(edgeId(h)).setDeleted(true);
    editVertexStatus(vo).setDeleted(true);
}

//-----------------------------------------------------------------------------
void HalfedgeConnectivity::collapse_loop(HalfedgeId _hh)
{
    HalfedgeId  h0 = _hh;
    HalfedgeId  h1 = nextHalfedgeId(h0);
    
    HalfedgeId  o0 = oppositeHalfedgeId(h0);
    HalfedgeId  o1 = oppositeHalfedgeId(h1);
    
    VertexId    v0 = toVertexId(h0);
    VertexId    v1 = toVertexId(h1);
    
    FaceId      fh = faceId(h0);
    FaceId      fo = faceId(o0);
    
    // is it a loop ?
    assert(nextHalfedgeId(h1) == h0);
    assert(h1 != o0);
    
    // halfedge -> halfedge
    setHalfedgeToHalfedge(h1, nextHalfedgeId(o0));
    setHalfedgeToHalfedge(previousHalfedgeId(o0), h1);
    
    // halfedge -> face
    setHalfedgeToFace(h1, fo);
    
    // vertex -> halfedge
    setVertexToHalfedge(v0, h1);  adjustOutgoingHalfedge(v0);
    setVertexToHalfedge(v1, o1);  adjustOutgoingHalfedge(v1);
    
    // face -> halfedge
    if (fo.isValid() && halfedgeId(fo) == o0)
    {
        setFaceToHalfedge(fo, h1);
    }

    // delete stuff
    if (fh.isValid())
    {
        setFaceToHalfedge(fh, InvalidHalfedgeId);
        editFaceStatus(fh).setDeleted(true);
    }
    editEdgeStatus(edgeId(h0)).setDeleted(true);
}

bool HalfedgeConnectivity::isCollapseOk(HalfedgeId v0v1)
{
    HalfedgeId v1v0(oppositeHalfedgeId(v0v1));
    VertexId v0(toVertexId(v1v0));
    VertexId v1(toVertexId(v0v1));
    
    // are vertices already deleted ?
    if (getEdgeStatus(edgeId(v0v1)).isDeleted() || getVertexStatus(v0).isDeleted() || getVertexStatus(v1).isDeleted())
        return false;
    
    VertexId vl, vr;
    HalfedgeId h1, h2;
    
    // the edges v1-vl and vl-v0 must not be both boundary edges
    if (!isBoundary(v0v1))
    {
        vl = toVertexId(nextHalfedgeId(v0v1));
    
        h1 = nextHalfedgeId(v0v1);
        h2 = nextHalfedgeId(h1);
        if (isBoundary(oppositeHalfedgeId(h1)) && isBoundary(oppositeHalfedgeId(h2)))
            return false;
    }
    
    // the edges v0-vr and vr-v1 must not be both boundary edges
    if (!isBoundary(v1v0))
    {
        vr = toVertexId(nextHalfedgeId(v1v0));
    
        h1 = nextHalfedgeId(v1v0);
        h2 = nextHalfedgeId(h1);
        if (isBoundary(oppositeHalfedgeId(h1)) && isBoundary(oppositeHalfedgeId(h2)))
            return false;
    }
    
    // if vl and vr are equal or both invalid -> fail
    if (vl == vr)
        return false;
    
    // test intersection of the one-rings of v0 and v1
    for (VertexCirculator vv_it(this,v0); vv_it; ++vv_it)
        editVertexStatus(vv_it.vertexId()).setTagged(false);
    
    for (VertexCirculator vv_it(this,v1); vv_it; ++vv_it)
        editVertexStatus(vv_it.vertexId()).setTagged(true);
    
    for (VertexCirculator vv_it(this,v0); vv_it; ++vv_it)
        // if (status(vv_it).tagged() && vv_it.handle() != vl && vv_it.handle() != vr)
        if (getVertexStatus(vv_it.vertexId()).isTagged() && vv_it.vertexId()!=vl && vv_it.vertexId() != vr)
            return false;
    
    // edge between two boundary vertices should be a boundary edge
    if (isBoundary(v0) && isBoundary(v1) && !isBoundary(v0v1) && !isBoundary(v1v0))
        return false;
    
    // passed all tests
    return true;
}




bool HalfedgeConnectivity::isFlipOk(EdgeId eid) const
{
    // boundary edges cannot be flipped
    if (isBoundary(eid))
        return false;
    
    HalfedgeId h = halfedgeId(eid, 0);
    HalfedgeId o = halfedgeId(eid, 1);
    
    // check if the flipped edge is already present
    // in the mesh
    VertexId va = toVertexId(nextHalfedgeId(h));
    VertexId vb = toVertexId(nextHalfedgeId(o));
    
    if (va == vb)   // this is generally a bad sign !!!
        return false;
    
    for (ConstVertexCirculator vv_it(this,va); vv_it; ++vv_it)
        if (vv_it.vertexId() == vb)
            return false;
    
    return true;
}

//-----------------------------------------------------------------------------
void HalfedgeConnectivity::flip(EdgeId eid)
{
    // CAUTION : Flipping a halfedge may result in
    // a non-manifold mesh, hence check for yourself
    // whether this operation is allowed or not!
    assert(isFlipOk(eid));//let's make it sure it is actually checked
    
    HalfedgeId a0 = halfedgeId(eid, 0);
    HalfedgeId b0 = halfedgeId(eid, 1);
    
    HalfedgeId a1 = nextHalfedgeId(a0);
    HalfedgeId a2 = nextHalfedgeId(a1);
    
    HalfedgeId b1 = nextHalfedgeId(b0);
    HalfedgeId b2 = nextHalfedgeId(b1);
    
    VertexId   va0 = toVertexId(a0);
    VertexId   va1 = toVertexId(a1);
    
    VertexId   vb0 = toVertexId(b0);
    VertexId   vb1 = toVertexId(b1);
    
    FaceId     fa  = faceId(a0);
    FaceId     fb  = faceId(b0);
    
    setHalfedgeToVertex(a0, va1);
    setHalfedgeToVertex(b0, vb1);
    
    setHalfedgeToHalfedge(a0, a2);
    setHalfedgeToHalfedge(a2, b1);
    setHalfedgeToHalfedge(b1, a0);
    
    setHalfedgeToHalfedge(b0, b2);
    setHalfedgeToHalfedge(b2, a1);
    setHalfedgeToHalfedge(a1, b0);
    
    setHalfedgeToFace(a1, fb);
    setHalfedgeToFace(b1, fa);
    
    setFaceToHalfedge(fa, a0);
    setFaceToHalfedge(fb, b0);
    
    if (halfedgeId(va0) == b0)
        setVertexToHalfedge(va0, a1);
    if (halfedgeId(vb0) == a0)
        setVertexToHalfedge(vb0, b1);
    
    // update the IFS mesh
    // FIXME update per vertex instance indices if any
    FaceHandle facea = editFace(fa);
    FaceHandle faceb = editFace(fb);
    assert(facea.nofVertices()==3);
    assert(faceb.nofVertices()==3);
    for (uint k=0 ; k<3 ; ++k)
    {
        if (facea.vertexId(k)==va0)
        {
            facea.vertexId(k) = vb1;
            break;
        }
    }
    for (uint k=0 ; k<3 ; ++k)
    {
        if (faceb.vertexId(k) ==vb0)        {
            faceb.vertexId(k) = va1;
            break;
        }
    }
}








void HalfedgeConnectivity::garbage_collection(bool _v, bool _e, bool _f)
{
    int i, i0, i1, nV(getNofVertices()), nE(getNofEdges()), nH(getNofHalfedges()), nF(getNofFaces());
    
    std::vector<VertexId>    vh_map;
    std::vector<HalfedgeId>  hh_map;
    std::map<FaceId,FaceId>  fh_map;
    
    // setup id mapping:
    vh_map.reserve(nV);
    for (i=0; i<nV; ++i)
        vh_map.push_back(VertexId(i));
    
    hh_map.reserve(nH);
    for (i=0; i<nH; ++i)
        hh_map.push_back(HalfedgeId(i));
    
    //fh_map.reserve(nF);
    FaceConnectivityList::iterator f_it(mFaceToHalfedge.begin()), f_end(mFaceToHalfedge.end());
    for ( ; f_it!=f_end ; ++f_it)
        fh_map[f_it.id()] = f_it.id();
    
    // remove deleted vertices
    if (_v && nV > 0)
    {
        i0=0;  i1=nV-1;
    
        for (;;)
        {
            // find 1st deleted and last un-deleted
            while (!getVertexStatus(VertexId(i0)).isDeleted() && i0 < i1)
                ++i0;
            while ( getVertexStatus(VertexId(i1)).isDeleted() && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;
        
            // swap
            std::swap(mVertexToHalfedge[i0], mVertexToHalfedge[i1]);
            std::swap(vh_map[i0], vh_map[i1]);
            editVertices().swap(i0,i1);
        }
    
        editVertices().resize(getVertexStatus(VertexId(i0)).isDeleted() ? i0 : i0+1);
        mVertexToHalfedge.resize(getVertices().size());
    }
    
    // remove deleted edges
    if (_e && nE > 0)
    {
        i0=0;  i1=nE-1;
    
        for (;;)
        {
            // find 1st deleted and last un-deleted
            while (!getEdgeStatus(i0).isDeleted() && i0 < i1)
                ++i0;
            while ( getEdgeStatus(i1).isDeleted() && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;
        
            // swap
            std::swap(mEdges[i0], mEdges[i1]);
            std::swap(hh_map[2*i0], hh_map[2*i1]);
            std::swap(hh_map[2*i0+1], hh_map[2*i1+1]);
        };
    
        mEdges.resize(getEdgeStatus(i0).isDeleted() ? i0 : i0+1);
    }
    
    // remove deleted faces
    // here, we have to work per sub mesh
    if (_f && nF > 0)
    {
        for (uint subId=0 ; subId!=mpMesh->getNofSubMeshes() ; ++subId)
        {
            HalfedgeIdArray& subFaceToHalfedge = *mFaceToHalfedge.editSet(subId);
            SubMesh& subMesh = *(mpMesh->editSubMesh(subId));
            assert(subMesh.size() == subFaceToHalfedge.size());
            
            if (subMesh.size()>0)
            {
                i0=0; i1=subMesh.size()-1;
            
                for (;;)
                {
                    // find 1st deleted and last un-deleted
                    while (!subFaceToHalfedge[i0].status.isDeleted() && i0 < i1)
                        ++i0;
                    while ( subFaceToHalfedge[i1].status.isDeleted() && i0 < i1)
                        --i1;
                    if (i0 >= i1)
                        break;
                
                    // swap
                    std::swap(subFaceToHalfedge[i0], subFaceToHalfedge[i1]);
                    std::swap(fh_map[FaceId(subId,i0)], fh_map[FaceId(subId,i1)]);
                    subMesh.swapFaces(i0,i1);
                };
            
                subFaceToHalfedge.resize(subFaceToHalfedge[i0].status.isDeleted() ? i0 : i0+1);
                subMesh.resize(subFaceToHalfedge.size());
            }
        }
    }
    
    
    // update halfedge index of vertices
    if (_e)
    {
        for (uint vid=0 ; vid<getNofVertices() ; ++vid)
        {
            if (halfedgeId(vid).isValid())
            {
                setVertexToHalfedge(vid, hh_map[halfedgeId(vid)]);
            }
        }
    }
    
    HalfedgeId hid;
    // update index of halfedges
    for (uint eid=0 ; eid<getNofEdges() ; ++eid)
    {//in the first pass update the (half)edges vertices
        hid = eid<<1;
        setHalfedgeToVertex(hid, vh_map[toVertexId(hid)]);
        hid+=1;
        setHalfedgeToVertex(hid, vh_map[toVertexId(hid)]);
    }
    
    for (uint eid=0 ; eid<getNofEdges() ; ++eid)
    {//in the second pass update the connectivity of the (half)edges
        hid = eid<<1;
        setHalfedgeToHalfedge(hid, hh_map[nextHalfedgeId(hid)]);
        if (!getHalfedge(hid).isBoundary())
        {
            setHalfedgeToFace(hid, fh_map[faceId(hid)]);
        }
        
        hid+=1;
        setHalfedgeToHalfedge(hid, hh_map[nextHalfedgeId(hid)]);
        if (!getHalfedge(hid).isBoundary())
        {
            setHalfedgeToFace(hid, fh_map[faceId(hid)]);
        }
    }
    
    // update handles of faces
    if (_e)
    {
        for (FaceConnectivityList::iterator  f_it(mFaceToHalfedge.begin()), f_end(mFaceToHalfedge.end());
             f_it!=f_end; ++f_it)
        {
            FaceId fid = f_it.id();
            
            assert(!getFaceStatus(fid).isDeleted());
            
            HalfedgeId hid = halfedgeId(fid);
            HalfedgeId newhid = hh_map[hid];
            setFaceToHalfedge(fid, newhid);
            
            FaceHandle face = editFace(fid);
            for (uint k=0 ; k<face.nofVertices() ; ++k)
                face.vertexId(k) = vh_map[face.vertexId(k)];
        }
    }
}

void HalfedgeConnectivity::dumpAsOFF(const QString& filename)
{
    QFile file(filename);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    
    stream << "OFF\n" << getNofVertices() << " " << getNofFaces() << " 0\n";

    for (uint i=0 ; i<getNofVertices() ; ++i)
    {
        stream << getVertex(i).position().x
        << " " << getVertex(i).position().y
        << " " << getVertex(i).position().z << "\n";
    }

    for(uint i=0 ; i<mFaceToHalfedge.getSet(0)->size() ; ++i)
    {
        FaceId fid(0,i);
        if (!getFaceStatus(fid).isDeleted())
        {
            stream << "3 ";
            HalfedgeId hid = halfedgeId(fid);
            HalfedgeId ref = hid;
            uint count = 0;
            do {
                stream << toVertexId(hid) << " ";
                count++;
                hid = nextHalfedgeId(hid);
            } while (hid!=ref);
            stream << "\n";
            if (count!=3)
            {
				std::cout << filename.toStdString() << " count = " << count << "  fid = " << i << "/" << mFaceToHalfedge.getSet(0)->size() << "\n";
                assert(count==3);
            }
        }
    }
    
    file.close();
}

bool HalfedgeConnectivity::checkIntegrity(void)
{
//     std::vector<Mesh::Edge>* pMeshEdges = mpMesh->buildEdgeList<Mesh::Edge>();
//     std::cout << "Nof edges: " << getNofEdges() << " == " << pMeshEdges->size() << "\n";
//     std::cout << "Nof vertices: " << mVertexToHalfedge.size() << " == " << mpMesh->getNofVertices() << "\n";
//     std::cout << "Nof faces: " << mFaceToHalfedge.size() << " == " << mpMesh->getNofFaces() << "\n";
//     
//     if (pMeshEdges->size() != getNofEdges())
//         return false;
//     for (uint eid=0 ; eid<pMeshEdges->size() ; ++eid)
//     {
//         // find the corresponding halfedge
//         HalfedgeId hid = findHalfedge(pMeshEdges->at(eid).vertexId[0], pMeshEdges->at(eid).vertexId[1]);
//         if (!hid.isValid())
//         {
//             std::cout << "\tERROR cannot find edge #" << eid << "\n";
//             return false;
//         }
//         if ( (pMeshEdges->at(eid).faceId[0] != faceId(hid))
//             || (pMeshEdges->at(eid).faceId[1] != faceId(oppositeHalfedgeId(hid))) )
//         {
//             std::cout << "\t(" << pMeshEdges->at(eid).faceId[0]._asInt() << "," << pMeshEdges->at(eid).faceId[1]._asInt()
//                 << ") == ( " << faceId(hid)._asInt() << "," << faceId(oppositeHalfedgeId(hid))._asInt() << ")\n";
//         }
//     }
#define print_assert(A,B) if (!(A)) {std::cerr << # A << "\n"; B; return false;}
    
    for (uint eid=0 ; eid<getNofEdges() ; ++eid)
    {
        if (!(getEdgeStatus(EdgeId(eid)).isDeleted()))
        {
            for (uint k=0 ; k<2 ; ++k)
            {
                HalfedgeId hid = halfedgeId(EdgeId(eid), k);
                VertexId vid = toVertexId(hid);
                print_assert(hid.isValid(), std::cerr << "eid=" << eid << "\n" );
                
                print_assert(vid.isValid(),std::cerr << "eid=" << eid << "\n");
                print_assert(!getVertexStatus(vid).isDeleted(), std::cerr << "eid=" << eid << "\n");
                
                print_assert(nextHalfedgeId(hid).isValid(), std::cerr << "eid=" << eid << "\n");
                print_assert(!getEdgeStatus(edgeId(nextHalfedgeId(hid))).isDeleted(), std::cerr << "eid=" << eid << "\n");
                
                print_assert(oppositeHalfedgeId(hid).isValid(), std::cerr << "eid=" << eid << "\n");
                print_assert(!getEdgeStatus(edgeId(oppositeHalfedgeId(hid))).isDeleted(), std::cerr << "eid=" << eid << "\n");
                
                if (faceId(hid).isValid())
                {
                    print_assert(!getFaceStatus(faceId(hid)).isDeleted(),
                        std::cerr << "eid=" << eid << " fid=" << faceId(hid)._asInt() << " hid=" << hid << "\n");
                }

                print_assert(nextHalfedgeId(previousHalfedgeId(hid))==hid, );
                
                print_assert(previousHalfedgeId(nextHalfedgeId(hid))==hid, );
                
                print_assert(toVertexId(oppositeHalfedgeId(hid))!=toVertexId(hid), 
                    std::cerr << "hid=" << hid << " nhid=" << nextHalfedgeId(hid)
                        << " fid=" << faceId(hid)._asInt() << " fid=" << faceId(nextHalfedgeId(hid))._asInt() << "\n" );
                
                print_assert(edgeId(nextHalfedgeId(hid))!=edgeId(hid), 
                    std::cerr << "hid=" << hid << " nhid=" << nextHalfedgeId(hid)
                        << " fid=" << faceId(hid)._asInt() << " fid=" << faceId(nextHalfedgeId(hid))._asInt() << "\n");
                
                print_assert(edgeId(previousHalfedgeId(hid))!=edgeId(hid), 
                    std::cerr << "hid=" << hid << " nhid=" << nextHalfedgeId(hid)
                        << " fid=" << faceId(hid)._asInt() << " fid=" << faceId(nextHalfedgeId(hid))._asInt() << "\n");
                
                print_assert(nextHalfedgeId(nextHalfedgeId(hid))!=hid && "loop",
                    std::cerr << "hid=" << hid << " nhid=" << nextHalfedgeId(hid)
                        << " fid=" << faceId(hid)._asInt() << " fid=" << faceId(nextHalfedgeId(hid))._asInt() << "\n");
                
                
                for (VertexCirculator vih_it(this,toVertexId(oppositeHalfedgeId(hid))); vih_it; ++vih_it)
                {
                    if (vih_it.vertexId()==vid)
                    {
                        print_assert(vih_it.halfedgeId() == hid && "double edge",
                            std::cerr << "hid=" << hid );
                    }
                }
            }
        }
    }
    
    for (FaceConnectivityList::iterator f_it(mFaceToHalfedge.begin()), f_end(mFaceToHalfedge.end()); f_it!=f_end; ++f_it)
    {
        FaceId fid = f_it.id();
        if (!(getFaceStatus(fid).isDeleted()))
        {
            HalfedgeId hid = halfedgeId(fid);
            print_assert(nextHalfedgeId(nextHalfedgeId(hid)) != hid,std::cerr << "fid = " << fid._asInt() << "\n");
            
            uint count=0;
            HalfedgeId ref=hid;
            do {
                print_assert(hid.isValid(),);
                print_assert(uint(edgeId(hid))<mEdges.size(),);
                print_assert(!getEdgeStatus(edgeId(hid)).isDeleted(),std::cerr << "fid = " << fid._asInt() << " eid=" << edgeId(hid) << "\n");
                count++;
                hid = nextHalfedgeId(hid);
            } while (hid!=ref && count<1000);
            print_assert(count==getFace(fid).nofVertices(), std::cerr << "fid = " << fid._asInt() << " count=" << count << "\n" );
        }
    }
    
    return true;
}

}
