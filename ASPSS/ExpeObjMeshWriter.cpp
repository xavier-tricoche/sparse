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



#include "ExpeObjMeshWriter.h"
#include "ExpeMesh.h"


namespace Expe
{

ObjMeshWriter::ObjMeshWriter(QIODevice* pDevice)
    : MeshWriter(pDevice)
{
}

ObjMeshWriter::~ObjMeshWriter()
{
}

void ObjMeshWriter::writeMesh(const Mesh* apMesh, Options& options)
{
    if ( (!mpDevice->isOpen()) && (!mpDevice->open(QFile::WriteOnly)) )
        return;
    
    QTextStream stream(mpDevice);
    
    bool swapFaces = options.has("swap") && options["swap"].toBool();
    
    const Mesh* pMesh = apMesh;
    if (!pMesh->isRegularIFS())
        pMesh = pMesh->createRegularIFS();
    
    stream << "# number vertices " << pMesh->getNofVertices() << "\n";
    stream << "# number of faces " << pMesh->getSubMesh(0)->getNofFaces() << "\n";
    
    stream << "g\n";

    for (Mesh::VertexList::const_iterator point_iter = pMesh->getVertices().begin() ; point_iter!=pMesh->getVertices().end() ; ++point_iter)
    {
        stream << "v " << point_iter->position().x << " " << point_iter->position().y << " " << point_iter->position().z << "\n";
    }
    
    bool exportNormals = (options.has("export_normals") && options["export_normals"].toBool());
    if (exportNormals)
    {
        for (Mesh::VertexList::const_iterator point_iter = pMesh->getVertices().begin() ; point_iter!=pMesh->getVertices().end() ; ++point_iter)
        {
            stream << "vn " << point_iter->normal().x << " " << point_iter->normal().y << " " << point_iter->normal().z << "\n";
        }
    }
    for(uint i=0 ; i<pMesh->getSubMesh(0)->getNofFaces() ; ++i)
    {
        Mesh::ConstFaceHandle f = pMesh->getSubMesh(0)->getFace(i);

        if (exportNormals)
        {
            if (swapFaces)
            {
                stream << "f "
                    << f.vertexId(2)+1 << "//" << f.vertexId(2)+1 << " "
                    << f.vertexId(1)+1 << "//" << f.vertexId(1)+1 << " "
                    << f.vertexId(0)+1 << "//" << f.vertexId(0)+1 << "\n";
            }
            else
            {
                stream << "f "
                    << f.vertexId(0)+1 << "//" << f.vertexId(0)+1 << " "
                    << f.vertexId(1)+1 << "//" << f.vertexId(1)+1 << " "
                    << f.vertexId(2)+1 << "//" << f.vertexId(2)+1 << "\n";
            }
        }
        else
        {
            if (swapFaces)
            {
                stream << "f "
                    << f.vertexId(2)+1 << " "
                    << f.vertexId(1)+1 << " "
                    << f.vertexId(0)+1 << "\n";
            }
            else
            {
                stream << "f "
                    << f.vertexId(0)+1 << " "
                    << f.vertexId(1)+1 << " "
                    << f.vertexId(2)+1 << "\n";
            }
        }
    }
    
    if (pMesh!=apMesh)
        delete pMesh;
}


}
