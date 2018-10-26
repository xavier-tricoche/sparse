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



#include "ExpeOffMeshWriter.h"
#include "ExpeMesh.h"


namespace Expe
{

OffMeshWriter::OffMeshWriter(QIODevice* pDevice)
    : MeshWriter(pDevice)
{
}

OffMeshWriter::~OffMeshWriter()
{
}

void OffMeshWriter::writeMesh(const Mesh* apMesh, Options& options)
{
    if ( (!mpDevice->isOpen()) && (!mpDevice->open(QFile::WriteOnly)) )
        return;
    
    QTextStream stream(mpDevice);
    
    bool swapFaces = options.has("swap") && options["swap"].toBool();
    
    const Mesh* pMesh = apMesh;
    if (!pMesh->isRegularIFS())
        pMesh = pMesh->createRegularIFS();
    
    stream << "OFF\n" << pMesh->getNofVertices() << " " << pMesh->getSubMesh(0)->getNofFaces() << " 0\n";

    for (uint i=0 ; i<pMesh->getNofVertices() ; ++i)
    {
        stream << pMesh->vertex(i).position().x
        << " " << pMesh->vertex(i).position().y
        << " " << pMesh->vertex(i).position().z << "\n";
    }

    for(uint i=0 ; i<pMesh->getSubMesh(0)->getNofFaces() ; ++i)
    {
        Mesh::ConstFaceHandle f = pMesh->getSubMesh(0)->getFace(i);
        if (swapFaces)
            stream << "3 " << f.vertexId(2) << " " << f.vertexId(1) << " " << f.vertexId(0) << "\n";
        else
            stream << "3 " << f.vertexId(0) << " " << f.vertexId(1) << " " << f.vertexId(2) << "\n";
    }
    
    if (pMesh!=apMesh)
        delete pMesh;
}


}
