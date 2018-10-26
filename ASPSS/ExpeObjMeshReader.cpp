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



#include "ExpeObjMeshReader.h"
#include "Mesh/ExpeMesh.h"
#include "ExpeMeshNormalEvaluator.h"
#include "ExpeCliProgressBar.h"
#include "ObjFormat.h"
#include <QFile>

namespace Expe
{

ObjMeshReader::ObjMeshReader(QIODevice* pDevice)
    : MeshReader(pDevice)
{
}

ObjMeshReader::~ObjMeshReader()
{
}



void ObjMeshReader::readMesh(Mesh* pMesh, Options& options)
{
    LOG_DEBUG_MSG(5,"ObjMeshReader: start reading...");
    
    QFile* pFile = dynamic_cast<QFile*>(mpDevice);
    if (!pFile)
    {
        LOG_ERROR("ObjMeshReader: can only read from file");
        return;
    }
    
    ObjMesh* pRawObjMesh = ObjMesh::LoadFromFile(pFile->fileName().toStdString());
    
    if (!pRawObjMesh)
    {
        LOG_ERROR("ObjMeshReader: error load file " << pFile->fileName() << ".");
        return;
    }
    
    ObjMesh* pObjMesh = pRawObjMesh->createIndexedFaceSet(Obj::Options(Obj::AllAttribs|Obj::Triangulate));
    delete pRawObjMesh;
    pRawObjMesh = 0;
    
    // copy material
    /*
    for (uint i=0 ; i<pObjMesh->getNofMaterials() ; ++i)
    {
        const ObjMaterial* pObjMat = pObjMesh->getMaterial(i);
        Material *pMat = MaterialManager::Instance().createMaterial(pObjMat->getName());
        
        if (pObjMat->hasProperty("Kd"))
        {
            Vector3 a(pObjMat->getKa());
            Vector3 d(pObjMat->getKd());
            Vector3 ad = a+d;
            Real ambientcoeff = ad.dotProduct(a)/ad.dotProduct(ad);
            pMat->setBaseColor(Color(pObjMat->getKd())*(1./(1.-ambientcoeff)));
            pMat->setAmbient(ambientcoeff);
        }
        
        if (Vector3(pObjMat->getKs())!=Vector3::ZERO)
        {
            pMat->setSpecularColor(Color(pObjMat->getKs()));
            pMat->setShininess(pObjMat->getNs());
        }
        
        if (pObjMat->getMapKd()!="")
        {
            Sampler2D* pTexture = Sampler2D::BuildFromFile(pObjMesh->getTexturePath() + pObjMat->getMapKd(), "DiffuseMap");
            if (pTexture)
            {
                pMat->addTexture(pTexture);
                pMat->setAlphaThresold(0.5);
            }
            else
            {
                std::cerr << "Unable to create texture " << (pObjMesh->getTexturePath() + pObjMat->getMapKd()) << "\n";
            }
        }
    }
    */
    
    // copy vertices
    for (uint i=0 ; i<pObjMesh->positions.size() ; ++i)
    {
        Mesh::VertexHandle pt = pMesh->addVertex(Vector3(pObjMesh->positions[i]));
        if (!pObjMesh->texcoords.empty())
            pt.texcoord() = Vector2(pObjMesh->texcoords[i]);
        
//         if (!pObjMesh->texcoords.empty())
//             pt.color() = Color(pObjMesh->texcoords[i].s,pObjMesh->texcoords[i].t,0.5).toRGBA();
            
        if (!pObjMesh->normals.empty())
        {
            pt.normal() = Vector3(pObjMesh->normals[i]);
        }
    }
    
    // copy faces
    for (uint smi=0 ; smi<pObjMesh->getNofSubMeshes() ; ++smi)
    {
        const ObjSubMesh* pSrcSubMesh = pObjMesh->getSubMesh(smi);
        
        pMesh->createSubMesh(QString(pSrcSubMesh->getName()));
        SubMesh* pSubMesh = pMesh->editSubMesh(pMesh->getNofSubMeshes()-1);
        /*
        const ObjMaterial* pObjMat = pObjMesh->getMaterial(pSrcSubMesh->getMaterialId());
        if (pObjMat)
        {
            String matName = pObjMat->getName();
            pSubMesh->setMaterial(MaterialManager::Instance().getMaterial(matName));
            if (MaterialManager::Instance().getMaterial(matName)==0)
                std::cerr << "Unable to find material \"" << matName << "\"\n";
        }
        */
        
        for (uint fid = 0 ; fid<pSrcSubMesh->getNofFaces() ; ++fid)
        {
            ObjConstFaceHandle srcFace = pSrcSubMesh->getFace(fid);
            Mesh::FaceHandle face = pSubMesh->createFace(3, Mesh::None);
            for (uint k=0 ; k<3 ; ++k)
                face.vertexId(k) = srcFace.vPositionId(k);
        }
    }

    if (pObjMesh->normals.empty())
    {
        MeshNormalEvaluator normalEvaluator(pMesh);
        if (!normalEvaluator.apply())
            LOG_ERROR("ObjMeshReader: error computing the normals.");
    }

    delete pObjMesh;
}


}
