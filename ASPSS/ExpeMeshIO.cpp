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



#include "ExpeMeshIO.h"
#include "ExpeSmartCast.h"
#include "ExpeLogManager.h"
#include "ExpeGeometryAutoReshape.h"

namespace Expe
{

MeshReader::MeshReader(QIODevice* pDevice)
    : Reader(pDevice)
{
}

MeshReader::~MeshReader()
{
}

SerializableObject* MeshReader::create(void)
{
    return new Mesh();
}

void MeshReader::read(SerializableObject* pDest, Options& options)
{
    Mesh* pMesh = dynamic_cast<Mesh*>(pDest);
    assert(pMesh==pDest);
    readMesh(pMesh, options);
    
    if (options.has("reshape") && options["reshape"].toBool())
    {
        Real size = options.has("reshape::size") ? smart_cast<Real>(options["reshape::size"]) : 1.;
        Vector3 offset = options.has("reshape::offset") ? smart_cast<Vector3>(options["reshape::offset"]) : Vector3::ZERO;
        LOG_DEBUG_MSG(5,"MeshReader: reshape with size=" << size << " offset=" << offset);
        GeometryAutoReshape::reshape(pMesh, size, offset);
        LOG_DEBUG(9,"Reshape OK");
    }
    
    if ( (!(options.has("inVideoMemory") && (!options["inVideoMemory"].toBool())) ) )
    {
//         pMesh->finalize();
        #pragma message ( "#warning fix Mesh::finalize" );
        LOG_DEBUG(9,"buildVBO OK");
    }
}

MeshWriter::MeshWriter(QIODevice* pDevice)
    : Writer(pDevice)
{
}

MeshWriter::~MeshWriter()
{
}

void MeshWriter::write(const SerializableObject* pDest, Options& options)
{
    const Mesh* pMesh = dynamic_cast<const Mesh*>(pDest);
    assert(pMesh==pDest);
    writeMesh(pMesh,options);
}

}

