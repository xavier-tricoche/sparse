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



#ifndef _ExpeGeometryAutoReshape_h_
#define _ExpeGeometryAutoReshape_h_


#include "ExpeGeometryOperator.h"
#include "ExpeMesh.h"

namespace Expe
{

class GeometryAutoReshape : public GeometryOperator
{
//Q_OBJECT

public:

    GeometryAutoReshape(GeometryObject* pInput);
    
    virtual ~GeometryAutoReshape(void);
    
    void setParameters(Real size,
        const Vector3& relativeOffset = Vector3::ZERO,
        const Quaternion& rotation  = Quaternion());
    
    virtual bool apply(void);
    
    static void reshape(Mesh* pMesh, Real size=1.,
        const Vector3& relativeOffset = Vector3::ZERO,
        const Quaternion& rotation  = Quaternion());
    
    static void reshape(PointSet* pPoints, Real size=1.,
        const Vector3& relativeOffset = Vector3::ZERO,
        const Quaternion& rotation  = Quaternion());
    
    QUICK_MEMBER(Real,TargetSize);
    QUICK_MEMBER(Vector3,RelativeOffset);
    QUICK_MEMBER(Quaternion,Rotation);
    
protected:

    static void computeReshapeParameters(const AxisAlignedBox& aabb,
        Real size, const Vector3& relativeOffset, const Quaternion& rotation,
        Vector3& trans, Real& scale, Matrix3& mrot);

};

}

#endif

