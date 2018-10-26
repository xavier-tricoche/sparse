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

#ifndef _ExpeHalfedgeConnectivity_h_
#define _ExpeHalfedgeConnectivity_h_

#include "ExpeMesh.h"

namespace Expe
{

// forward declaration
namespace HalfedgeConnectivityCirculators
{
    class VertexCirculator;
    class ConstVertexCirculator;
    
    class FaceCirculator;
    class ConstFaceCirculator;
}

class HalfedgeConnectivity;

/// alias to make the use of HalfedgeConnectivity's types easier !
typedef HalfedgeConnectivity HEC;


/** Halfedge dada structure.

    \note Note that this class is based on material from OpenMesh, Copyright (C) 2004 by Computer Graphics Group, RWTH Aachen, http://www.openmesh.org.
*/
class HalfedgeConnectivity
{
public:

    // forward declaration
    class Halfedge;
    class Edge;
    
    typedef Mesh::VertexId VertexId;
    typedef Mesh::VertexList VertexList;
    typedef Mesh::VertexHandle VertexHandle;
    typedef Mesh::ConstVertexHandle ConstVertexHandle;
    
    typedef Mesh::FaceId FaceId;
    typedef Mesh::FaceList FaceList;
    typedef Mesh::FaceHandle FaceHandle;
    typedef Mesh::ConstFaceHandle ConstFaceHandle;
    
    EXPE_DECLARE_NEW_SIMPLEINDEX(HalfedgeId);
    typedef Halfedge& HalfedgeHandle;
    typedef const Halfedge& ConstHalfedgeHandle;
    
    EXPE_DECLARE_NEW_SIMPLEINDEX(EdgeId);
    typedef Edge& EdgeHandle;
    typedef const Edge& ConstEdgeHandle;
    
    
    typedef HalfedgeConnectivityCirculators::VertexCirculator VertexCirculator;
    typedef HalfedgeConnectivityCirculators::ConstVertexCirculator ConstVertexCirculator;
    
    typedef HalfedgeConnectivityCirculators::FaceCirculator FaceCirculator;
    typedef HalfedgeConnectivityCirculators::ConstFaceCirculator ConstFaceCirculator;
    

    // FIXME should we also store a reference to the vertex instance ?
    // or only store it instead of the reference to the vertex id ?
    class Halfedge
    {
    public:
        Mesh::FaceId faceId;
        Mesh::VertexId toVertexId;
        HalfedgeId nextHalfedgeId;
        
        inline bool isBoundary(void) const { return !faceId.isValid(); }
    };
    
    class Status
    {
    public:
        Status()
        {
            mIsDeleted = 0;
            mIsSelected = 0;
        }
        
        inline bool isSelected(void) const { return mIsSelected==1; }
        inline void setSelected(bool on) { mIsSelected = (on?1:0); }
        
        inline bool isDeleted(void) const { return mIsDeleted==1; }
        inline void setDeleted(bool on) { mIsDeleted = (on?1:0); }
        
        inline bool isTagged(void) const { return mIsTagged==1; }
        inline void setTagged(bool on) { mIsTagged = (on?1:0); }
        
    protected:
            ubyte mIsSelected : 1;
            ubyte mIsDeleted : 1;
            ubyte mIsTagged : 1;
    };
    
    struct Edge
    {
        Halfedge halfedges[2];
        Status status;
    };
    
public:

    HalfedgeConnectivity(MeshPtr pMesh, Verbosity verb = Verbose);
    
    ~HalfedgeConnectivity(void);
    
    /** \name index based management
    */
    //@{
    /// Returns an outgoing halfedge id of a vertex
    inline HalfedgeId halfedgeId(VertexId vid) const;
    
    /// Returns the first halfedge id of a face
    inline HalfedgeId halfedgeId(FaceId fid) const;
    
    /** Returns the i-th halfedge id of an egde.
        Of course i must be 0 or 1
    */
    inline HalfedgeId halfedgeId(EdgeId eid, uint i) const { assert(i<2); return HalfedgeId((uint(eid)<<1) + i); }
    
    inline FaceId faceId(HalfedgeId hid) const { return getHalfedge(hid).faceId; }
    inline VertexId toVertexId(HalfedgeId hid) const { return getHalfedge(hid).toVertexId; }

    /// Returns the opposite halfedge id of an halfedge
    inline HalfedgeId oppositeHalfedgeId(HalfedgeId hid) const;
    
    /// Returns the next halfedge id of an halfedge
    inline HalfedgeId nextHalfedgeId(HalfedgeId hid) const;
    
    /** Returns the previous halfedge id of an halfedge
    */
    inline HalfedgeId previousHalfedgeId(HalfedgeId hid) const;
    
    /** Returns the halfedge id of the next halfedge around the ingoing vertex (CW rotation)
    */
    inline HalfedgeId cwRotatedHalfedgeId(HalfedgeId hid) const;
    
    /** Returns the halfedge id of the previous halfedge around the ingoing vertex (CCW rotation)
    */
    inline HalfedgeId ccwRotatedHalfedgeId(HalfedgeId hid) const;
    
    /// Returns the id of the edge related to the halfedge
    inline EdgeId edgeId(HalfedgeId hid) const { return (hid >> 1); }
    
    inline bool isBoundary(HalfedgeId hid) const { return getHalfedge(hid).isBoundary(); }
    inline bool isBoundary(VertexId vid) const;
    inline bool isBoundary(EdgeId eid) const { return (getEdge(eid).halfedges[0].isBoundary() || getEdge(eid).halfedges[1].isBoundary()); }
    //@}
    
    
    /** \name connectivity updates
    */
    //@{
    /** set the first halfedge of a face
    */
    inline void setFaceToHalfedge(FaceId fid, HalfedgeId hid);
    
    /** set the outgoing halfedge of a vertex
    */
    inline void setVertexToHalfedge(VertexId fid, HalfedgeId hid);
    
    /** set the next halfedge of an halfedge
    */
    inline void setHalfedgeToHalfedge(HalfedgeId hid, HalfedgeId nextHid) { editHalfedge(hid).nextHalfedgeId = nextHid; }
    
    /** set the target vertex of an halfedge
    */
    inline void setHalfedgeToVertex(HalfedgeId hid, VertexId vid) { editHalfedge(hid).toVertexId = vid; }
    
    /** set the face of an halfedge
    */
    inline void setHalfedgeToFace(HalfedgeId hid, FaceId fid) { editHalfedge(hid).faceId = fid; }
    //@}
    
//     inline bool isBoundary(HalfedgeId hid) const;
    
    /** \name entity accessors and Mesh wrapper members
    */
    //@{
    inline uint getNofHalfedges(void) const { return getNofEdges()*2; }
    inline ConstHalfedgeHandle getHalfedge(HalfedgeId hid) const;
    inline HalfedgeHandle editHalfedge(HalfedgeId hid);
    
    inline uint getNofEdges(void) const { return mEdges.size(); }
    inline ConstEdgeHandle getEdge(EdgeId eid) const { return mEdges[eid]; }
    inline EdgeHandle editEdge(EdgeId eid) { return mEdges[eid]; }
    inline ConstEdgeHandle getEdge(HalfedgeId hid) const { return mEdges[hid>>1]; }
    inline EdgeHandle editEdge(HalfedgeId hid) { return mEdges[hid>>1]; }
    
    inline uint getNofFaces(void) const { return mpMesh->getNofFaces(); }
    inline ConstFaceHandle getFace(FaceId fid) const { return mpMesh->getFace(fid); }
    inline FaceHandle editFace(FaceId fid) { return mpMesh->editFace(fid); }
    inline const FaceList& getFaces(void) const { return mpMesh->getFaces(); }
    inline FaceList& editFaces(void) { return mpMesh->editFaces(); }
    
    inline uint getNofVertices(void) const { return mpMesh->getNofVertices(); }
    inline ConstVertexHandle getVertex(VertexId vid) const { return mpMesh->getVertex(vid); }
    inline VertexHandle editVertex(VertexId vid) { return mpMesh->editVertex(vid); }
    inline const VertexList& getVertices(void) const { return mpMesh->getVertices(); }
    inline VertexList& editVertices(void) { return mpMesh->editVertices(); }
    //@}
    
    
    /** \name Mesh creation
    */
    //@{
    /** Create a new face with the vertices referenced by vertexIds.
        \par
        The face is created both into the IFS mesh and the halfedge connectivity.
        \param vertexIds the list of vertex indices of the face
        \param subSetId allow to optionnaly specify in which sub-mesh the face should be added. By default, the face is inserted into the last sub-mesh. If no sub-mesh exist, then one is automatically created.
        \return the index of the face or an invalid index if the insertion failed.
    */
    FaceId createFace(const IndexArray& vertexIds, int subSetId=-1, Verbosity verb = Verbose);
    
    void deleteFace(FaceId fid);
    
    void deleteVertex(VertexId vid);
    //@}
    
    /** \name status get/set
    */
    //@{
    inline const Status& getVertexStatus(VertexId vid) const {return mVertexToHalfedge[vid].status; }
    inline Status& editVertexStatus(VertexId vid) {return mVertexToHalfedge[vid].status; }
    
    inline const Status& getFaceStatus(FaceId fid) const {return mFaceToHalfedge.at(fid).status; }
    inline Status& editFaceStatus(FaceId fid) {return mFaceToHalfedge.at(fid).status; }
    
    inline const Status& getEdgeStatus(EdgeId eid) const {return mEdges[eid].status; }
    inline Status& editEdgeStatus(EdgeId eid) {return mEdges[eid].status; }
    //@}
    
    /** \name Topology modifying operators
    */
    //@{
    /** Check whether collapsing hid is topologically correct.
        \warning this method currently only holds for triangular meshes.
    */
    bool isCollapseOk(HalfedgeId hid);
    
    /** Edge collapse.
        Check for topological correctness first using isCollapseOk().
    */
    void collapse(HalfedgeId hid);
    
    /** Check whether flipping eid is topologically correct.
        \warning edge flipping only hold for triangular meshes.
    */
    bool isFlipOk(EdgeId eid) const;

    /** Flip edge _eh.
        Check for topological correctness first using isFlipOk().
        \warning edge flipping only hold for triangular meshes.
    */
    void flip(EdgeId eid);
    //@}
    
    /** \name Other useful operators
    */
    //@{
    /** Computes the valence of the vertex.
    */
    uint computeValence(VertexId vid) const;
    
    Vector3 computeNormal(FaceId fid) const;
    //@}
    
    void garbage_collection(bool _v=true, bool _e=true, bool _f=true);
    
    bool checkIntegrity(void);
    void dumpAsOFF(const QString& filename);
    
protected:

    // internal methods
    bool addFaceInWrapper(FaceId fid, Verbosity verb = Verbose);
    
    HalfedgeId createEdge(VertexId v0, VertexId v1);
    
    HalfedgeId findHalfedge(VertexId v0, VertexId v1) const;
    
    void adjustOutgoingHalfedge(VertexId vid);
    
    void collapse_edge(HalfedgeId _hh);
    void collapse_loop(HalfedgeId _hh);
    
protected:

    MeshPtr mpMesh;
    
    struct HalfedgeIdArrayElement
    {
        HalfedgeId halfedgeId;
        Status status;
        operator HalfedgeId& () {return halfedgeId; }
        operator const HalfedgeId& () const {return halfedgeId; }
    };
    
    std::vector<Edge> mEdges;
    typedef std::vector<HalfedgeIdArrayElement> HalfedgeIdArray;
    HalfedgeIdArray mVertexToHalfedge;
    typedef MultiSet<HalfedgeIdArray,HalfedgeIdArrayElement&,const HalfedgeIdArrayElement&,FaceId> FaceConnectivityList;
    FaceConnectivityList mFaceToHalfedge;
};

#include "ExpeHalfedgeConnectivity.inl"

}

#endif

