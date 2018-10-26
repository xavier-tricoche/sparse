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



#include "ExpeBasicMesh2PointSet.h"

#include "ExpeStaticInitializer.h"
#include "ExpeMeshNormalEvaluator.h"

namespace Expe
{
/*
EXPE_INIT_CODE(BasicMesh2PointSet) {
    GeometryOperatorManager::InstanceSafe().registerFactory(new GeometryOperatorFactoryCreatorMatch1<BasicMesh2PointSet,Mesh>());
}*/

BasicMesh2PointSet::BasicMesh2PointSet(GeometryObject* pInputMesh)
    : GeometryOperator(pInputMesh)
{
    mIsInputAsOutputDoable = false;
    mInputAsOutput = false;
}

BasicMesh2PointSet::~BasicMesh2PointSet(void)
{
}

bool BasicMesh2PointSet::apply(void)
{
    Mesh* pMesh = dynamic_cast<Mesh*>(mpInput);
    if (!pMesh)
        return false;
    
    if (!pMesh->isRegularIFS())
    {
        pMesh = pMesh->createRegularIFS();
        /*if (pMesh->normals.empty())
        {
            MeshNormalEvaluator normalEvaluator(pMesh);
            normalEvaluator.apply();
        }*/
    }
    
    // create a point set with the default format
    PointSet* pPoints = new PointSet();
    
    uint nofVertices = pMesh->getNofVertices();
    pPoints->resize(nofVertices);
    std::vector<int> counts(nofVertices);
    for (uint i=0 ; i<nofVertices ; ++i)
    {
        pPoints->at(i).position() = pMesh->vertex(i).position();
        pPoints->at(i).normal() = pMesh->vertex(i).normal();
        pPoints->at(i).color() = Color::WHITE.toRGBA();
        pPoints->at(i).radius() = 0.;
        counts[i] = 0;
    }

    // compute the radii
    for (uint j=0 ; j<pMesh->getNofSubMeshes() ; ++j)
    {
        SubMesh* pSubMesh = pMesh->editSubMesh(j);
        for (uint i=0 ; i<pSubMesh->getNofFaces() ; ++i)
        {
            Mesh::FaceHandle face = pSubMesh->editFace(i);
            
            Vector3f cog = (  pMesh->vertex(face.vertexId(0)).position()
                            + pMesh->vertex(face.vertexId(1)).position()
                            + pMesh->vertex(face.vertexId(2)).position()) / 3.;
    
            for (uint k=0 ; k<3 ; ++k)
                pPoints->at(face.vertexId(k)).radius() = Math::Max(pPoints->at(face.vertexId(k)).radius(),
                    pPoints->at(face.vertexId(k)).position().squaredDistanceTo(cog));
        }
    }
    
    for (uint i=0 ; i<nofVertices ; ++i)
        pPoints->at(i).radius() = Math::Sqrt(pPoints->at(i).radius()) * 1.2;

    mpOutput = pPoints;
    
    return true;
}

}

//#include "ExpeBasicMesh2PointSet.moc"
