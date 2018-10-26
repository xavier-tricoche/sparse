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



#include "ExpeOffMeshReader.h"
#include "ExpeMesh.h"
#include "ExpeMeshNormalEvaluator.h"
#include "ExpeCliProgressBar.h"


namespace Expe
{

OffMeshReader::OffMeshReader(QIODevice* pDevice)
    : MeshReader(pDevice)
{
}

OffMeshReader::~OffMeshReader()
{
}

void OffMeshReader::readMesh(Mesh* pMesh, Options& options)
{
    if ( (!mpDevice->isOpen()) && (!mpDevice->open(QFile::ReadOnly)) )
        return;
    
    QTextStream stream(const_cast<QIODevice*>(mpDevice));
    
    bool swapFaces = options.has("swap") && options["swap"].toBool();

    // check the header file
    QString header = stream.readLine();
    if (!header.contains("OFF",Qt::CaseInsensitive))
    {
        LOG_ERROR("OffMeshReader:: wrong header = \"" << header << "\"");
        mpDevice->close();
        return;
    }
    
    int nofVertices, nofFaces, inull;
    int nb, id0, id1, id2;
    Vector3 v;

    stream >> nofVertices >> nofFaces >> inull;

    std::cout << "OffMeshReader::nofVertices = " << nofVertices << std::endl;
    pMesh->editVertices().reserve(nofVertices);
    
    CliProgressBarT<uint> progressBar(0,nofVertices+nofFaces-1);
    for (int i=0 ; i<nofVertices ; ++i)
    {
        stream >> v.x >> v.y >> v.z;

        pMesh->addVertex(v);
        progressBar.update(i);
    }

    SubMesh* pSubMesh = pMesh->createSubMesh();
    
    for (int i=0 ; i<nofFaces ; ++i)
    {
        stream >> nb >> id0 >> id1 >> id2;
        assert(nb==3 && "OFF file format, faces other than triangles are currently not supported");

        Mesh::FaceHandle face = pSubMesh->createFace(3, Mesh::None);
        if (swapFaces)
        {
            face.vertexId(0) = id0;
            face.vertexId(1) = id2;
            face.vertexId(2) = id1;
        }
        else
        {
            face.vertexId(0) = id0;
            face.vertexId(1) = id1;
            face.vertexId(2) = id2;
        }
        progressBar.update(nofVertices+i);
    }

    MeshNormalEvaluator normalEvaluator(pMesh);
    if (!normalEvaluator.apply())
        LOG_ERROR("OffMeshReader: error computing the normals.");
}


}
