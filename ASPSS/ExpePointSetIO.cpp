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



#include "ExpePointSetIO.h"
#include "ExpeSmartCast.h"
#include "ExpeLogManager.h"
#include "ExpeGeometryAutoReshape.h"

namespace Expe
{

PointSetReader::PointSetReader(QIODevice* pDevice)
    : Reader(pDevice)
{
}

PointSetReader::~PointSetReader()
{
}

SerializableObject* PointSetReader::create(void)
{
    return new PointSet();
}

void PointSetReader::read(SerializableObject* pDest, Options& options)
{
    PointSet* pPointSet = dynamic_cast<PointSet*>(pDest);
    assert(pPointSet==pDest);
    readPointSet(pPointSet, options);
    
    if (options.has("reshape") && options["reshape"].toBool())
    {
        Real size = options.has("reshape::size") ? smart_cast<Real>(options["reshape::size"]) : 1.;
        Vector3 offset = options.has("reshape::offset") ? smart_cast<Vector3>(options["reshape::offset"]) : Vector3::ZERO;
        LOG_DEBUG_MSG(5,"PointSetReader: reshape with size=" << size << " offset=" << offset);
        GeometryAutoReshape::reshape(pPointSet, size, offset);
        LOG_DEBUG(9,"Reshape OK");
    }
    
    if ( (!(options.has("inVideoMemory") && (!options["inVideoMemory"].toBool())) ) )
    {
//         pPointSet->finalize();
        #pragma message ( "warning fix PointSet::finalize" )
        LOG_DEBUG(9,"buildVBO OK");
    }
}

PointSetWriter::PointSetWriter(QIODevice* pDevice)
    : Writer(pDevice)
{
}

PointSetWriter::~PointSetWriter()
{
}

void PointSetWriter::write(const SerializableObject* pDest, Options& options)
{
    const PointSet* pPointSet = dynamic_cast<const PointSet*>(pDest);
    assert(pPointSet==pDest);
    writePointSet(pPointSet,options);
}

}

