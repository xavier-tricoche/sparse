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



#include "ExpeMeshNormalEvaluator.h"

#include "ExpeStaticInitializer.h"

namespace Expe
{

//EXPE_INIT_CODE(MeshNormalEvaluator) {
//    GeometryOperatorManager::InstanceSafe().registerFactory(new GeometryOperatorFactoryCreatorMatch1<MeshNormalEvaluator,Mesh>());
//}

MeshNormalEvaluator::MeshNormalEvaluator(GeometryObject* pInputMesh)
    : GeometryOperator(pInputMesh)
{
    mIsInputAsOutputDoable = true;
    mInputAsOutput = true;
}

MeshNormalEvaluator::~MeshNormalEvaluator(void)
{
}

bool MeshNormalEvaluator::apply(void)
{
    Mesh* pMesh = dynamic_cast<Mesh*>(mpInput);
    if (!pMesh)
        return false;
    
    if (!mInputAsOutput)
        pMesh = pMesh->clone();
    mpOutput = pMesh;
    
    if (pMesh->isRegularIFS())
    {
        // set the normals to zero
        uint nofVertices = pMesh->getNofVertices();
        for (uint i=0 ; i<nofVertices ; ++i)
            pMesh->vertex(i).normal() = Vector3f::ZERO;
        
        // accumulate the normals of each faces
        for (uint j=0 ; j<pMesh->getNofSubMeshes() ; ++j)
        {
            SubMesh* pSubMesh = pMesh->editSubMesh(j);
            for (uint i=0 ; i<pSubMesh->getNofFaces() ; ++i)
            {
                Mesh::FaceHandle face = pSubMesh->editFace(i);
                
                Vector3f normal =  (pMesh->vertex(face.vertexId(1)).position()-pMesh->vertex(face.vertexId(0)).position())
                            .cross(pMesh->vertex(face.vertexId(2)).position()-pMesh->vertex(face.vertexId(0)).position()).normalized();
        
                // wheight the normal by the angle
                Vector3f v01 = (pMesh->vertex(face.vertexId(1)).position()-pMesh->vertex(face.vertexId(0)).position()).normalized();
                Vector3f v12 = (pMesh->vertex(face.vertexId(2)).position()-pMesh->vertex(face.vertexId(1)).position()).normalized();
                Vector3f v20 = (pMesh->vertex(face.vertexId(0)).position()-pMesh->vertex(face.vertexId(2)).position()).normalized();
        
                float a0, a1, a2;
                a0 = Math::ACos(-v20.dot(v01));
                a1 = Math::ACos(-v01.dot(v12));
                a2 = Math::ACos(-v12.dot(v20));
        
                /*pMesh->normals[face.vertexId(0)] += a0 * normal;
                pMesh->normals[face.vertexId(1)] += a1 * normal;
                pMesh->normals[face.vertexId(2)] += a2 * normal;*/
                
                pMesh->vertex(face.vertexId(0)).normal() += a0 * normal;
                pMesh->vertex(face.vertexId(1)).normal() += a1 * normal;
                pMesh->vertex(face.vertexId(2)).normal() += a2 * normal;
            }
        }
        
        // normalize the normals
        for (uint i=0 ; i<nofVertices ; ++i)
            pMesh->vertex(i).normal().normalize();
    }
    else
        return false;

    return true;
}

}

//#include "ExpeMeshNormalEvaluator.moc"
