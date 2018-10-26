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



#include "ExpeMesh.h"

#include "ExpeMath.h"
#include <QString>

namespace Expe
{


//--------------------------------------------------------------------------------
// SubMesh implementation
//--------------------------------------------------------------------------------

SubMesh::SubMesh(const QString& name, Mesh* pOwner)
    : mNofFaces(0), mConstNofVertices(-1), mName(name), mDefaultMaterialId(-1), mpOwner(pOwner)
{
    typedef VNormalIdArray* toto;
    // the convertion to a uint is used to avoids a stupid warning
    mVNormalIds = reinterpret_cast<VNormalIdArray*>(ArithPtr(&mVertexIds));
    mVTexcoordIds = reinterpret_cast<VTexcoordIdArray*>(ArithPtr(&mVertexIds));
}
    
SubMesh::~SubMesh()
{
    if (Pointer(mVTexcoordIds)!=Pointer(&mVertexIds))
        DESTROY_PTR(mVTexcoordIds);
    
    if (Pointer(mVNormalIds)!=Pointer(&mVertexIds))
        DESTROY_PTR(mVNormalIds);
}

void SubMesh::setMaterialId(int matId)
{
    mDefaultMaterialId = matId;
    mFMaterialIds.clear();
}

Mesh::FaceHandle SubMesh::createFace(uint nofVertices, Mesh::Options att, int matId/*=-1*/)
{
//     assert(matId<0 || mpOwner->getMaterial(matId)!=0 && "The specified material's id is not valid");
    
    if (mNofFaces==0)
    {
        // first face, let us assume a constant number of vertexInstances per face
        mConstNofVertices = nofVertices;
    }
    else if (!mFSizes.empty())
    {
        mFSizes.push_back(nofVertices);
        mFOffsets.push_back(mVertexIds.size());
    }
    else if (mConstNofVertices!=int(nofVertices))
    {
        mFSizes.resize(mNofFaces);
        mFOffsets.resize(mNofFaces);
        for (uint i=0 ; i<mNofFaces ; ++i)
        {
            mFSizes[i] = mConstNofVertices;
            mFOffsets[i] = mConstNofVertices * i;
        }
        mConstNofVertices = -1;
        
        mFSizes.push_back(nofVertices);
        mFOffsets.push_back(mVertexIds.size());
    }
    
    if (!mFMaterialIds.empty())
    {
        if (matId<0)
            mFMaterialIds.push_back(mDefaultMaterialId);
        else
            mFMaterialIds.push_back(matId);
    }
    else if (matId>=0)
    {
        if (mDefaultMaterialId<0)
        {
            // first valid material id
            mDefaultMaterialId = matId;
        }
        else if (matId!=mDefaultMaterialId)
        {
            // first different material id
            mFMaterialIds.resize(mNofFaces);
            for (uint i=0 ; i<mNofFaces ; ++i)
            {
                mFMaterialIds[i] = mDefaultMaterialId;
            }
            mFMaterialIds.push_back(matId);
        }
    }
    
    mNofFaces++;
    
    mVertexIds.resize(mVertexIds.size()+nofVertices);
    
    if (att&Mesh::Texcoord && Pointer(mVTexcoordIds)==Pointer(&mVertexIds))
    {
        mVTexcoordIds = new VTexcoordIdArray();
    }
    
    if (Pointer(mVTexcoordIds)!=Pointer(&mVertexIds))
    {
        int oldSize = mVTexcoordIds->size();
        mVTexcoordIds->resize(mVTexcoordIds->size()+nofVertices);
        for (uint i=oldSize ; i<mVTexcoordIds->size() ; ++i)
            mVTexcoordIds->at(i).invalidate();
    }
    
    if (att&Mesh::Normal && Pointer(mVNormalIds)==Pointer(&mVertexIds))
    {
        mVNormalIds = new VNormalIdArray();
    }
    
    if (Pointer(mVNormalIds)!=Pointer(&mVertexIds))
    {
        int oldSize = mVNormalIds->size();
        mVNormalIds->resize(mVNormalIds->size()+nofVertices);
        for (uint i=oldSize ; i<mVNormalIds->size() ; ++i)
            mVNormalIds->at(i).invalidate();
    }

    return editFace(mNofFaces-1);
}

bool SubMesh::isRegularIFS(void) const
{
    return mFSizes.empty() && mFMaterialIds.empty()
        && Pointer(mVNormalIds)==Pointer(&mVertexIds)
        && Pointer(mVTexcoordIds)==Pointer(&mVertexIds);
}


void SubMesh::resize(uint n)
{
    if (n<mNofFaces)
    {
        if (mFSizes.empty())
        {
            // constant number of vertices per face
            mVertexIds.resize(n*mConstNofVertices);
        
            if (Pointer(mVTexcoordIds) != Pointer(&mVertexIds))
                mVTexcoordIds->resize(n*mConstNofVertices);
            
            if (Pointer(mVNormalIds) != Pointer(&mVertexIds))
                mVNormalIds->resize(n*mConstNofVertices);
        }
        else
        {
            // variable number of vertices per face
            mVertexIds.resize(mFOffsets[n]);
        
            if (Pointer(mVTexcoordIds) != Pointer(&mVertexIds))
                mVTexcoordIds->resize(mFOffsets[n]);
            
            if (Pointer(mVNormalIds) != Pointer(&mVertexIds))
                mVNormalIds->resize(mFOffsets[n]);
            
            mFOffsets.resize(n);
            mFSizes.resize(n);
        }
        
        if (!mFMaterialIds.empty())
            mFMaterialIds.resize(n);
        
        mNofFaces = n;
    }
}


// FIXME this function is a bit tricky and has never been tested ! so expect some bugs here ;)
// the purpose is to swap the vertex indices of two face having a different number of vertices
// that involves to shift the inner part...
template <class ArrayT>
void SubMesh::_swapAux(uint nbv0, uint nbv1, uint o0, uint o1, ArrayT& array)
{
    if (nbv0>nbv1)
    {
        uint overlapsize = nbv1-nbv0;
        uint destO = o1-overlapsize;
        // save the overlap parts
        ArrayT backup(overlapsize);
        for (uint k=0 ; k<overlapsize ; ++k)
            backup[k] = array[destO+k];
        // swap elements
        for (uint k=0 ; k<nbv0 ; ++k)
        {
            array[destO+k] = o0+k < destO ? array[o0+k] : backup[destO-(o0+k)];
            if (k<nbv1)
                array[o0+k] = array[o1+k];
        }
        if (destO>o0+nbv0)
        {
            // move the inner part
            for (uint i=o0+nbv0 ; i<destO ; ++i)
                array[i-overlapsize] = array[i];
            // insert the backup
            for (uint k=0 ; k<overlapsize ; ++k)
                array[destO-overlapsize+k] = backup[k];
        }
    }
    else if (nbv0<nbv1)
    {
        uint overlapsize = nbv0-nbv1;
        // save the overlap parts
        ArrayT backup(overlapsize);
        for (uint k=0 ; k<overlapsize ; ++k)
            backup[k] = array[o0+nbv0+k];
        // swap elements
        for (int k=nbv1-1 ; k>0 ; --k)
        {
            array[o0+k] = o1+k>=o0+nbv0+overlapsize ? array[o1+k] : backup[o1+k-(o0+nbv0)];
            if (uint(k)>=overlapsize)
                array[o1+k] = array[o0+k-overlapsize];
        }
        if (o1>=o0+nbv1+overlapsize)
        {
            // move the inner part
            for (uint i=o1+overlapsize-1 ; i>=o0+nbv1+overlapsize ; --i)
                array[i] = array[i-overlapsize];
            // insert the backup
            for (uint k=0 ; k<overlapsize ; ++k)
                array[o0+nbv1+k] = backup[k];
        }
    }
}


void SubMesh::swapFaces(uint i0, uint i1)
{
    uint nbv0, nbv1;
    uint o0, o1;
    if (!mFSizes.empty())
    {
        nbv0 = mFSizes[i0];
        nbv1 = mFSizes[i1];
        o0 = mFOffsets[i0];
        o1 = mFOffsets[i1];
    }
    else
    {
        nbv0 = nbv1 = mConstNofVertices;
        o0 = mConstNofVertices*i0;
        o1 = mConstNofVertices*i1;
    }
    
    if (nbv0==nbv1)
    {
        // easy case
        for (uint k=0 ; k<nbv0 ; ++k)
        {
            std::swap(mVertexIds[o0+k], mVertexIds[o1+k]);
            
            if (Pointer(mVTexcoordIds) != Pointer(&mVertexIds))
                std::swap(mVTexcoordIds[o0+k], mVTexcoordIds[o1+k]);
            
            if (Pointer(mVNormalIds) != Pointer(&mVertexIds))
                std::swap(mVNormalIds[o0+k], mVNormalIds[o1+k]);
        }
        
        if (!mFMaterialIds.empty())
            std::swap(mFMaterialIds[i0], mFMaterialIds[i1]);
    }
    else
    {
        _swapAux(nbv0, nbv1, o0, o1, mVertexIds);
        if (Pointer(mVTexcoordIds) !=Pointer(&mVertexIds))
            _swapAux(nbv0, nbv1, o0, o1, *mVTexcoordIds);
        if (Pointer(mVNormalIds) != Pointer(&mVertexIds))
            _swapAux(nbv0, nbv1, o0, o1, *mVNormalIds);

        if (!mFMaterialIds.empty())
            std::swap(mFMaterialIds[i0], mFMaterialIds[i1]);
        
        std::swap(mFSizes[i0], mFSizes[i1]);
        int o = nbv1-nbv0;
        for (uint i=i0+1 ; i<=i1 ; ++i)
            mFOffsets[i]+=o;
    }
}

//--------------------------------------------------------------------------------
// Mesh implementation
//--------------------------------------------------------------------------------

Mesh::Mesh(const VertexList::Format& vertexFormat /*= VertexList::Attribute_position*/)
    : mVertices(vertexFormat)
{
}

Mesh::~Mesh()
{
    for (uint i=0 ; i<getNofSubMeshes() ; ++i)
        delete editSubMesh(i);
}

SubMesh* Mesh::createSubMesh(const QString& aName /*= ""*/)
{
    QString name = aName;
    SubMesh* pSubMesh = 0;
    if (name=="")
    {
        name = QString("submesh_%1").arg(getNofSubMeshes());
    }
    else
    {
        pSubMesh = editSubMesh(name);
        if (pSubMesh)
            return pSubMesh;
    }
    
    pSubMesh = new SubMesh(name,this);
    mSubMeshesByName[name] = pSubMesh;
    mEveryFaces.add(pSubMesh);
    
    return pSubMesh;
}

const SubMesh* Mesh::getSubMesh(const QString& name) const
{
    std::map<QString,SubMesh*>::const_iterator it = mSubMeshesByName.find(name);
    if (it==mSubMeshesByName.end())
        return 0;
    return it->second;
}
SubMesh* Mesh::editSubMesh(const QString& name)
{
    return const_cast<SubMesh*>(getSubMesh(name));
}


AxisAlignedBox Mesh::computeAABB(void) const
{
    return mVertices.computeAABB();
}

bool Mesh::isRegularIFS(void) const
{
    for (uint smi=0 ; smi<getNofSubMeshes() ; ++smi)
    {
        const SubMesh* pSubMesh = getSubMesh(smi);
        if (!pSubMesh->isRegularIFS())
            return false;
    }
    return true;
}

Mesh* Mesh::clone(void) const
{
    Mesh* pClone = new Mesh(this->mVertices.getFormat());
    
    // clone the vertexInstances
    pClone->mVertices.reserve(mVertices.size());
    for(uint i=0 ; i<mVertices.size() ; ++i)
    {
        pClone->mVertices.push_back(mVertices[i]);
    }
    pClone->mExtraNormals = this->mExtraNormals;
    pClone->mExtraTexcoords = this->mExtraTexcoords;

    for (uint smi=0 ; smi<getNofSubMeshes() ; ++smi)
    {
        const SubMesh* pSrcSubMesh = getSubMesh(smi);
        SubMesh* pDstSubMesh = pClone->createSubMesh(pSrcSubMesh->getName());
        
        pDstSubMesh->mNofFaces = pSrcSubMesh->mNofFaces;
        pDstSubMesh->mFSizes = pSrcSubMesh->mFSizes;
        pDstSubMesh->mConstNofVertices = pSrcSubMesh->mConstNofVertices;
        pDstSubMesh->mFOffsets = pSrcSubMesh->mFOffsets;
        pDstSubMesh->mFMaterialIds = pSrcSubMesh->mFMaterialIds;
        pDstSubMesh->mVertexIds = pSrcSubMesh->mVertexIds;
        if (Pointer(pSrcSubMesh->mVTexcoordIds)!=Pointer(&pSrcSubMesh->mVertexIds))
        {
            pDstSubMesh->mVTexcoordIds = new SubMesh::VTexcoordIdArray();
            *pDstSubMesh->mVTexcoordIds = *pSrcSubMesh->mVTexcoordIds;
        }
        if (Pointer(pSrcSubMesh->mVNormalIds)!=Pointer(&pSrcSubMesh->mVertexIds))
        {
            pDstSubMesh->mVNormalIds = new SubMesh::VNormalIdArray();
            *pDstSubMesh->mVNormalIds = *pSrcSubMesh->mVNormalIds;
        }
        pDstSubMesh->mDefaultMaterialId = pSrcSubMesh->mDefaultMaterialId;
    }
    
    return pClone;
}

Mesh* Mesh::createRegularIFS(Mesh::Options options) const
{
    Mesh* pClone = new Mesh();
    

    // store for each vertex position the indices of the new vertexInstances.
    // a vertex having multiple different normals or texcoords will appear multiple times.
    // the key value is a mix of the normal and texcoord indices.
    std::vector< std::map<long long int,uint> > vertexInstances;
    vertexInstances.resize(getNofVertices());
    
    for (uint smi=0 ; smi<getNofSubMeshes() ; ++smi)
    {
        const SubMesh* pSrcSubMesh = getSubMesh(smi);
        int matId = pSrcSubMesh->getMaterialId();
        SubMesh* pDstSubMesh;
        if (matId<0)
        {
            matId = pSrcSubMesh->getFace(0).materialId();
            QString name = (pSrcSubMesh->getName() + "/%1").arg(matId);
            pDstSubMesh = pClone->createSubMesh(name);
            pDstSubMesh->setMaterialId(matId);
        }
        else
        {
            pDstSubMesh = pClone->createSubMesh(pSrcSubMesh->getName());
            pDstSubMesh->setMaterialId(matId);
        }
        
        // this map is used to classify the local sub meshes per material id
        std::map<int,SubMesh*> localSubMesh;
        localSubMesh[matId] = pDstSubMesh;
        
        for (uint fid = 0 ; fid<pSrcSubMesh->getNofFaces() ; ++fid)
        {
            ConstFaceHandle sface = pSrcSubMesh->getFace(fid);
            
            if (sface.materialId()!=matId)
            {
                // material change
                matId = sface.materialId();
                std::map<int,SubMesh*>::iterator smeshit = localSubMesh.find(matId);
                if (smeshit==localSubMesh.end())
                {
                    // a local sub mesh with this material does not exist yet.
                    // let's create it:
                    QString name = (pSrcSubMesh->getName() + "/%1").arg(matId);
                    pDstSubMesh = pClone->createSubMesh(name);
                    localSubMesh[matId] = pDstSubMesh;
                }
                else
                {
                    pDstSubMesh = smeshit->second;
                }
            }

            int dstNofVertices = 3;
            if ( (!options&Mesh::Triangulate) && (pSrcSubMesh->getConstNofVerticesPerFace()>0) )
                dstNofVertices = pSrcSubMesh->getConstNofVerticesPerFace();
            
            int ids[3];
            ids[0] = 0;
            ids[1] = 1;
            ids[2] = 2;
            
            uint nofSubPolygons = 1 + (sface.nofVertices()-dstNofVertices);
            for (uint polyId=0 ; polyId<nofSubPolygons ; ++polyId)
            {
                FaceHandle dface = pDstSubMesh->createFace(dstNofVertices,Mesh::None,matId);
                for (uint k=0 ; k<3 ; ++k)
                {
                    // construction of the unique key value of the vertex:
                    long long int key = 0;
                    if (options&Mesh::Texcoord)
                        key = (long long int)(sface.vTexcoordId(ids[k]));
                    if (options&Mesh::Normal)
                        key = key | ((long long int)(sface.vNormalId(ids[k])) << 32 );
                    
                    assert(uint(sface.vertexId(k))<mVertices.size());
                    //assert(sface.vTexcoordId(k)<int(texcoords.size()));
                    //assert(sface.vNormalId(k)<int(normals.size()));
                    
                    std::map<long long int,uint>::const_iterator vit = vertexInstances[sface.vertexId(ids[k])].find(key);
                    if (vit!=vertexInstances[sface.vertexId(ids[k])].end())
                    {
                        // the unique vertex already exist, let's reuse it
                        dface.vertexId(k) = vit->second;
                    }
                    else
                    {
                        // a new vertex has to be created
                        vertexInstances[sface.vertexId(ids[k])][key] = pClone->getNofVertices();
                        dface.vertexId(k) = pClone->getNofVertices();
                        VertexHandle v = pClone->addVertex(this->vertex(sface.vertexId(ids[k])).position());
                        
                        /* FIXME createIndexedFaceSet: texcoords
                        if ( (options&Mesh::Texcoord) && (!this->texcoords.empty()) )
                        {
                            if (sface.vTexcoordId(ids[k])>=0)
                                pClone->texcoords.push_back(this->texcoords[sface.vTexcoordId(ids[k])]);
                            else
                                pClone->texcoords.push_back(Vector2f::ZERO);
                        }
                        */
                        // FIXME createIndexedFaceSet: normals
                        if ( (options&Mesh::Normal) && (!this->mExtraNormals.empty()) )
                        {
                            if (sface.vNormalId(ids[k])>=0)
                                //pClone->normals.push_back(this->normals[sface.vNormalId(ids[k])]);
                                v.normal() = this->mExtraNormals[sface.vNormalId(ids[k])];
                            else
                                //pClone->normals.push_back(Vector3f::ZERO);
                                v.normal() = Vector3f::ZERO;
                        }
                    }
                }
                ids[1] = ids[2];
                ids[2]++;
            }
        }
    }
    
    return pClone;
}

}

//#include "ExpeMesh.moc"
