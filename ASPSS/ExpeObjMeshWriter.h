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



#ifndef _ExpeObjMeshWriter_h_
#define _ExpeObjMeshWriter_h_

#include "ExpePrerequisites.h"
#include "ExpeMeshIO.h"

namespace Expe
{

class ObjMeshWriter : public MeshWriter
{
public:

    ObjMeshWriter(QIODevice* pDevice);
    
    virtual ~ObjMeshWriter();

protected:
    
    virtual void writeMesh(const Mesh* pMesh, Options& options = Options::EMPTY);
    
protected:

};

}

#endif

