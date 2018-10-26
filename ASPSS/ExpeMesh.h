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



#ifndef _ExpeMesh_h_
#define _ExpeMesh_h_

#include "ExpePointSet.h"
#include "ExpeAxisAlignedBox.h"
#include "ExpeMultiSet.h"
#include "ExpeGeometryObject.h"

namespace Expe
{

class SubMesh;

/** Base class to create index types.
*/
class BaseIndex
{
public:
    inline BaseIndex(void) : mValue(0xffffffff) {}
    inline BaseIndex(uint val) : mValue(val) {}
    inline bool isValid(void) const { return mValue!=0xffffffff; }
    inline void invalidate(void) { mValue = 0xffffffff; }
    
    inline bool operator==(const BaseIndex& i) const { return mValue == i.mValue; }
    inline bool operator!=(const BaseIndex& i) const { return mValue != i.mValue; }
    inline bool operator<(const BaseIndex& i) const { return mValue < i.mValue; }
    
//     inline SimpleIndex& operator++ (void) { ++mValue; return *this; }
//     inline SimpleIndex& operator-- (void) { --mValue; return *this; }
    
protected:
    uint mValue;
};

/** Convenient macro to instanciate a new basic index type.
*/
#define EXPE_DECLARE_NEW_SIMPLEINDEX(TYPENAME) class TYPENAME : public BaseIndex { public: \
    inline TYPENAME(void) : BaseIndex() {} \
    inline TYPENAME(uint val) : BaseIndex(val) {} \
    inline operator uint& () { return mValue; } \
    inline operator uint () const { return mValue; } \
}

/** Convenient macro to instanciate a new extra index type.
*/
#define EXPE_DECLARE_NEW_EXTRAINDEX(TYPENAME) class TYPENAME : public BaseIndex { public: \
    inline TYPENAME(void) : BaseIndex() {} \
    inline TYPENAME(uint val, bool extra) : BaseIndex(val) { if (extra) val|=0x80000000; }  \
    inline bool isExtra(void) const { return mValue&0x80000000; }  \
    inline operator uint () const { return mValue&0x7fffffff; } \
}

/** Represent a mesh as an indexed face set.
    A mesh stores the list of vertex attributes (position, textcoord, normal).
    A Mesh is composed of several SubMesh that holds the faces.
*/
class Mesh : public GeometryObject
{

//Q_OBJECT
friend class HalfedgeConnectivity;

public:

    /** Face index.
    */
    EXPE_DECLARE_NEW_MULTIINDEX(FaceId);
    
    /** Vertex's normal index.
    */
    EXPE_DECLARE_NEW_EXTRAINDEX(VNormalId);
    
    /** Vertex's texture coordinate index.
    */
    EXPE_DECLARE_NEW_EXTRAINDEX(VTexcoordId);

    
    /** Vertex index.
    */
    EXPE_DECLARE_NEW_SIMPLEINDEX(VertexId);
//     class VertexId : public BaseIndex { public:
//         inline VertexId(void) : BaseIndex() {}
//         inline VertexId(uint val) : BaseIndex(val) {}
// //         inline operator const VNormalId& () const { return *reinterpret_cast<const VNormalId*>(this); }
//         inline operator uint& () { return mValue; }
//         inline operator uint () const { return mValue; }
//     };
    
    typedef PointSet VertexList;
    typedef VertexList::PointHandle VertexHandle;
    typedef VertexList::ConstPointHandle ConstVertexHandle;

    enum Options {None=0,Texcoord=1,Normal=2,AllAttribs=3,Triangulate=4};

    /** Allows to access to a face (polygon)
    */
    class ConstFaceHandle
    {
    friend class SubMesh;
    public:
    
        inline uint nofVertices(void) const;
        
        inline int materialId(void) const;
        
        inline VertexId vertexId(uint i) const;
        inline VTexcoordId vTexcoordId(uint i) const;
        inline VNormalId vNormalId(uint i) const;
        
    protected:
    
        ConstFaceHandle(const SubMesh* pOwner, uint faceId, uint offset, uint nofVertices);
        
        const SubMesh* mpOwner;
        uint mFaceId;
        uint mOffset;
        uint mNofVertices;
    };
    
    /** Allows to access and modify a face (polygon)
    */
    class FaceHandle
    {
    friend class SubMesh;
    public:
        
        inline uint nofVertices(void) const;
        
        inline int materialId(void) const;
        
        inline VertexId& vertexId(uint i);
        inline VTexcoordId& vTexcoordId(uint i);
        inline VNormalId& vNormalId(uint i);
        
    protected:
    
        FaceHandle(SubMesh* pOwner, uint faceId, uint offset, uint nofVertices);
        
        SubMesh* mpOwner;
        uint mFaceId;
        uint mOffset;
        uint mNofVertices;
    };
    
    typedef MultiSet<SubMesh,FaceHandle,ConstFaceHandle,FaceId> FaceList;

public:

    Mesh(const VertexList::Format& vertexFormat = VertexList::Format(VertexList::Attribute_position | VertexList::Attribute_normal));
    
    ~Mesh();
    
    
    /** Create a mesh where all the attribute indices of a vertex are the same
        and where all the faces of a sub mesh have the material and some number of vertices.
        \param options determines which attributes have to be preserved.
        For instance, if mode==None, then the normals and texcoords are lost.
        On the other hand, if mode contains Texcoord, then one vertex is created
        per pair of different position/texcoord and a vertex's position may appear several times.
        
        In particular, the returned mesh satisfies:
            - hasUniqueMaterial() is true for all sub meshes.
            - getConstNofVerticesPerFace()>=3 for all sub meshes.
            - vertexId(), vNormalId() and vTexcoordId() returns the same value
    */
    Mesh* createRegularIFS(Options options = Options(AllAttribs|Triangulate)) const;
    
    Mesh* clone(void) const;
    
    /** Return true if the mesh is a regular indexed face set.
    */
    bool isRegularIFS(void) const;


    /// \name Sub-mesh managment
    //@{
    SubMesh* createSubMesh(const QString& name = "");
    uint getNofSubMeshes(void) const {return mEveryFaces.getNofSets();}
    
    const SubMesh* getSubMesh(int i) const {return mEveryFaces.getSet(i);}
    SubMesh* editSubMesh(int i) {return mEveryFaces.editSet(i);}
    
    const SubMesh* getSubMesh(const QString& name) const;
    SubMesh* editSubMesh(const QString& name);
    //@}
    
    
    AxisAlignedBox computeAABB(void) const;

    /// \name Vertex related stuff
    //{@
    
    /// returns the total number of vertices
    inline uint getNofVertices(void) const { return mVertices.size(); }
    
    /// Returns the vi-th vertex with read-write capability
    inline VertexHandle editVertex(VertexId vi) { return mVertices[vi]; }
    
    /// Returns the vi-th vertex with read-only capability
    inline ConstVertexHandle getVertex(VertexId vi) const { return mVertices[vi]; }
    
    /// shortcut to access a vertex (==editVertex)
    inline VertexHandle vertex(VertexId vi) { return mVertices[vi]; }
    
    /// shortcut to access a vertex (==getVertex)
    inline ConstVertexHandle vertex(VertexId vi) const { return mVertices[vi]; }
    
    /// add a vertex of position p and returns an handle the new vertex
    inline VertexHandle addVertex(const Vector3f& p) { VertexHandle v = mVertices.append(); v.position() = p; return v;}
    
    /** Gives full access to the list of vertices.
        Useful to use the iterator, to reserve memory, to directly apply meshless algorithms defined for PointSet to the vertices, etc.
    */
    inline VertexList& editVertices(void) {return mVertices;}
    
    /** Gives full access to the list of vertices.
        Useful to use the iterator, to directly apply meshless algorithms defined for PointSet to the vertices, etc.
    */
    inline const VertexList& getVertices(void) const {return mVertices;}
    
    inline bool hasExtraNormals(void) const { return mExtraNormals.size(); }
    inline std::vector<Vector3f>& editExtraNormals(void) { return mExtraNormals; }
    inline Vector3f& normal(VertexId i) { return mVertices.at(i).normal(); }
    inline const Vector3f& normal(VertexId i) const { return mVertices.at(i).normal(); }
    inline Vector3f& normal(VNormalId i);
    inline const Vector3f& normal(VNormalId i) const;
    
    inline bool hasExtraTexcoords(void) const { return mExtraTexcoords.size(); }
    inline std::vector<Vector2f>& editExtraTexcoords(void) { return mExtraTexcoords; }
    inline Vector2f& texcoord(VertexId i) { return mVertices.at(i).texcoord(); }
    inline const Vector2f& texcoord(VertexId i) const { return mVertices.at(i).texcoord(); }
    inline Vector2f& texcoord(VTexcoordId i);
    inline const Vector2f& texcoord(VTexcoordId i) const;
    //@}
    
    /** \name Face related stuff
        Since the face are stored per sub mesh, \see SubMesh for more advanced Face related features...
        \warning Remark that the list of valid face indices is continuous if and only if there is only one submesh. Therefore it not possible to loop over the set of faces by incrementing the face id for 0 to getNofFaces(). Instead you have to use the approriate iterator or first loop over the submeshes...
    */
    //{@
    
    /// returns the total number of faces
    inline uint getNofFaces(void) const { return mEveryFaces.size(); }
    
    /// returns a face with read-write capability
    inline FaceHandle editFace(FaceId fi) { return mEveryFaces.at(fi); }
    
    /// returns a face with read-only capability
    inline ConstFaceHandle getFace(FaceId fi) const { return mEveryFaces.at(fi); }
    
    /// shortcut (==editFace)
    inline FaceHandle face(FaceId fi) { return mEveryFaces.at(fi); }
    
    /// shortcut (==getFace)
    inline ConstFaceHandle face(FaceId fi) const { return mEveryFaces.at(fi); }
    
    /** Gives full access to the list of faces.
        Useful to use the iterators..
    */
    inline const FaceList& getFaces(void) const { return mEveryFaces; }
    
    /** Gives full access to the list of faces.
        Useful to use the iterators..
    */
    inline FaceList& editFaces(void) { return mEveryFaces; }
    
    // should we export iterator for each specific item ??
    // I think using Mesh::FaceList::iterator, and pMesh->getFaces().begin() is fine...
    //typedef FaceList::iterator face_iterator;
    //typedef FaceList::const_iterator const_face_iterator;
    
    //@}
    
protected:

    VertexList mVertices;
    
    /// List of vertex's extra texture coordinates
    std::vector<Vector2f> mExtraTexcoords;
    
    /// List of vertex's extra normals
    std::vector<Vector3f> mExtraNormals;
    
    std::map<QString,SubMesh*> mSubMeshesByName;
    
    // store the faces per sub mesh
    FaceList mEveryFaces;
};

typedef SharedPointer<Mesh> MeshPtr;
typedef ConstSharedPointer<Mesh> ConstMeshPtr;



/** A SubMesh is basically a list of faces (polygons).
*/
class SubMesh
{
friend class Mesh;
friend class Mesh::FaceHandle;
friend class Mesh::ConstFaceHandle;
public:

    ~SubMesh();

    /// Name of the sub mesh.
    const QString& getName(void) const {return mName;}
    void rename(const QString& name) {mName = name;}
    
    /** Returns true if all faces have the same material.
    */
    bool hasUniqueMaterial(void) const {return mFMaterialIds.empty();}
    
    /** Returns the default Material's id of the sub mesh.
    */
    int getMaterialId(void) const {return mDefaultMaterialId;}
    
    /** Set the default material's id and set it to all faces.
    */
    void setMaterialId(int matId);
    
    inline uint getNofFaces(void) const {return mNofFaces;}
    
    inline Mesh::ConstFaceHandle getFace(uint fid) const
    {
        if (mFSizes.empty())
        {
            // means a constant number of vertices per face
            return Mesh::ConstFaceHandle(this,fid,mConstNofVertices*fid,mConstNofVertices);
        }
        return Mesh::ConstFaceHandle(this,fid,mFOffsets[fid],mFSizes[fid]);
    }
    inline Mesh::FaceHandle editFace(uint fid)
    {
        if (mFSizes.empty())
        {
            // means a constant number of vertices per face
            return Mesh::FaceHandle(this,fid,mConstNofVertices*fid,mConstNofVertices);
        }
        return Mesh::FaceHandle(this,fid,mFOffsets[fid],mFSizes[fid]);
    }
    
    /** Return the number of vertices per face if it is constant, and -1 otherwise.
    */
    inline int getConstNofVerticesPerFace(void) const {return mConstNofVertices;}
    
    /** Add a face to the SubMesh.
        \param nofVertices number of vertices of the face. By default we assume a triangle.
        \param att specify the attribute with a different index than the main vertex index. By default we assume an Indexed Face Set.
        \param matId index of the material of the face.
    */
    Mesh::FaceHandle createFace(uint nofVertices=3, Mesh::Options att=Mesh::None, int matId=-1);
    
    /** Return true if the sub mesh is a regular indexed face set.
        I.e., if all faces have the same number vertices, all vertices don't have any extra normal or extra texcoords,
        and all faces have the same material.
    */
    bool isRegularIFS(void) const;
    
    /** \name MultiSet compatibility
    */
    //@{
    
    /** \internal Added for MultiSet compatibility.
    */
    inline Mesh::ConstFaceHandle at(uint fid) const { return getFace(fid); }
    
    /** \internal Added for MultiSet compatibility.
    */
    inline Mesh::FaceHandle at(uint fid) { return editFace(fid); }
    
    /** \internal Added for MultiSet compatibility.
    */
    inline uint size(void) const { return getNofFaces(); }
    
    /** \internal This method can only reduce the number of faces by deleting the last ones.
        In other words, if n>size(), then this method as no effect.
        This method is useful to delete a set of faces after pushing them at the end of the stream (\see swapFaces)
    */
    void resize(uint n);
    
    /** Swap two faces. Useful to push deleted faces at the end of the stream.
    */
    void swapFaces(uint i0, uint i1);
    //@}
    
protected:

    typedef std::vector<Mesh::VertexId> VertexIdArray;
    typedef std::vector<Mesh::VNormalId> VNormalIdArray;
    typedef std::vector<Mesh::VTexcoordId> VTexcoordIdArray;
    
    template <class ArrayT>
    void _swapAux(uint nbv0, uint nbv1, uint o0, uint o1, ArrayT& array);

    SubMesh(const QString& name, Mesh* pOwner);
    
    uint mNofFaces;
    
    /// The list of face's sizes
    std::vector<uint> mFSizes;
    int mConstNofVertices;
    
    /// The list of face's offsets
    std::vector<uint> mFOffsets;
    
    /// The list of face's material indices.
    std::vector<int> mFMaterialIds;
    
    /// Pool of position indices
    VertexIdArray mVertexIds;
    
    /// Pool of texcoord indices
    VTexcoordIdArray* mVTexcoordIds;
    
    /// Pool of normal indicies
    VNormalIdArray* mVNormalIds;
    
    QString mName;
    int mDefaultMaterialId;
    Mesh* mpOwner;
};

//-------------------------------------------------------------------------------
// inline implementation
//-------------------------------------------------------------------------------


inline Vector3f& Mesh::normal(VNormalId i)
{
    if (i.isExtra())
        return mExtraNormals[i];
    else
        return mVertices.at(i).normal();
}
inline const Vector3f& Mesh::normal(VNormalId i) const
{
    if (i.isExtra())
        return mExtraNormals[i];
    else
        return mVertices.at(i).normal();
}


inline Mesh::FaceHandle::FaceHandle(SubMesh* pOwner, uint faceId, uint offset, uint nofVertices)
    : mpOwner(pOwner), mFaceId(faceId), mOffset(offset), mNofVertices(nofVertices)
{}

inline uint Mesh::FaceHandle::nofVertices(void) const {return mNofVertices;}

inline int Mesh::FaceHandle::materialId(void) const
{
    if (mpOwner->mFMaterialIds.empty())
        return mpOwner->mDefaultMaterialId;
    return mpOwner->mFMaterialIds[mFaceId];
}

inline Mesh::VertexId& Mesh::FaceHandle::vertexId(uint i)
{
    assert(i<nofVertices());
    uint id = mOffset + i;
    assert(id<mpOwner->mVertexIds.size());
    return mpOwner->mVertexIds[id];
}

inline Mesh::VTexcoordId& Mesh::FaceHandle::vTexcoordId(uint i)
{
    assert(i<nofVertices());
    uint id = mOffset + i;
    assert(id<mpOwner->mVTexcoordIds->size());
    return mpOwner->mVTexcoordIds->at(id);
}

inline Mesh::VNormalId& Mesh::FaceHandle::vNormalId(uint i)
{
    assert(i<nofVertices());
    uint id = mOffset + i;
    assert(id<mpOwner->mVNormalIds->size());
    return mpOwner->mVNormalIds->at(id);
}



inline Mesh::ConstFaceHandle::ConstFaceHandle(const SubMesh* pOwner, uint faceId, uint offset, uint nofVertices)
    : mpOwner(pOwner), mFaceId(faceId), mOffset(offset), mNofVertices(nofVertices)
{}

inline uint Mesh::ConstFaceHandle::nofVertices(void) const {return mNofVertices;}

inline int Mesh::ConstFaceHandle::materialId(void) const
{
    if (mpOwner->mFMaterialIds.empty())
        return mpOwner->mDefaultMaterialId;
    return mpOwner->mFMaterialIds[mFaceId];
}

inline Mesh::VertexId Mesh::ConstFaceHandle::vertexId(uint i) const
{
    assert(i<nofVertices());
    uint id = mOffset + i;
    assert(id<mpOwner->mVertexIds.size());
    return mpOwner->mVertexIds[id];
}

inline Mesh::VTexcoordId Mesh::ConstFaceHandle::vTexcoordId(uint i) const
{
    assert(i<nofVertices());
    uint id = mOffset + i;
    assert(id<mpOwner->mVTexcoordIds->size());
    return mpOwner->mVTexcoordIds->at(id);
}

inline Mesh::VNormalId Mesh::ConstFaceHandle::vNormalId(uint i) const
{
    assert(i<nofVertices());
    uint id = mOffset + i;
    assert(id<mpOwner->mVNormalIds->size());
    return mpOwner->mVNormalIds->at(id);
}


}

#endif
