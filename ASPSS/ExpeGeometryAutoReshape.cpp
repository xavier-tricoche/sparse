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



#include "ExpeGeometryAutoReshape.h"

#include "ExpeStaticInitializer.h"
#include "ExpeMatrix3.h"

namespace Expe
{

//EXPE_INIT_CODE(GeometryAutoReshape) {
  //  GeometryOperatorManager::InstanceSafe().registerFactory(new GeometryOperatorFactoryCreatorMatch2<GeometryAutoReshape,PointSet,Mesh>());
//}

GeometryAutoReshape::GeometryAutoReshape(GeometryObject* pInput)
    : GeometryOperator(pInput)
{
    mIsInputAsOutputDoable = true;
    mInputAsOutput = true;
    
    mTargetSize = 100.;
    mRelativeOffset = Vector3::ZERO;
}

GeometryAutoReshape::~GeometryAutoReshape(void)
{
}

void setParameters(Real size,
        const Vector3& relativeOffset = Vector3::ZERO,
        const Quaternion& rotation  = Quaternion())
{
    
}

void GeometryAutoReshape::computeReshapeParameters(const AxisAlignedBox& aabb,
        Real size, const Vector3& relativeOffset, const Quaternion& rotation,
        Vector3& trans, Real& scale, Matrix3& mrot)
{
    Vector3 diff = aabb.max() - aabb.min();
    scale = diff.maxComponent();
    scale = size/scale;
    trans = aabb.min() + 0.5*diff - relativeOffset*diff;
    rotation.toRotationMatrix(mrot);

}

void GeometryAutoReshape::reshape(Mesh* pMesh, Real size, const Vector3& relativeOffset, const Quaternion& rotation)
{
    AxisAlignedBox aabb = pMesh->computeAABB();
    Real scale;
    Vector3 trans;
    Matrix3 mrot;
    computeReshapeParameters(aabb, size, relativeOffset, rotation, trans, scale, mrot);

    uint nofVertices = pMesh->getNofVertices();
    for (uint i=0 ; i<nofVertices ; ++i)
    {
        Vector3& p = pMesh->vertex(i).position();
        p -= trans;
        p *= scale;
        p = mrot * p;
        
		if (pMesh->editVertices().hasAttribute((UberVectorBaseT<_PointSetBuiltinData>::Attribute)(Mesh::VertexList::Attribute_normal)))
            pMesh->vertex(i).normal() = mrot * pMesh->vertex(i).normal();
    }
    
    // extra normals
    uint nofNormals = pMesh->editExtraNormals().size();
    for (uint i=0 ; i<nofNormals ; ++i)
        pMesh->editExtraNormals().at(i) = mrot * pMesh->editExtraNormals().at(i);
}

void GeometryAutoReshape::reshape(PointSet* pPoints, Real size, const Vector3& relativeOffset, const Quaternion& rotation)
{
    AxisAlignedBox aabb = pPoints->computeAABB();
    Real scale;
    Vector3 trans;
    Matrix3 mrot;
    computeReshapeParameters(aabb, size, relativeOffset, rotation, trans, scale, mrot);

    uint nb = pPoints->size();
    for (uint i=0 ; i<nb ; ++i)
    {
        Vector3& p = pPoints->at(i).position();
        p -= trans;
        p *= scale;
        p = mrot * p;
        
		if (pPoints->hasAttribute((UberVectorBaseT<_PointSetBuiltinData>::Attribute)(PointSet::Attribute_radius)))
            pPoints->at(i).radius() *= scale;
        
		if (pPoints->hasAttribute((UberVectorBaseT<_PointSetBuiltinData>::Attribute)(PointSet::Attribute_normal)))
            pPoints->at(i).normal() = mrot * pPoints->at(i).normal();
    }
}


bool GeometryAutoReshape::apply(void)
{
    Mesh* pMesh = dynamic_cast<Mesh*>(mpInput);
    if (pMesh)
    {
        if (!mInputAsOutput)
            pMesh = pMesh->clone();
        reshape(pMesh, mTargetSize, mRelativeOffset, mRotation);
        return true;
    }
    
    PointSet* pPoints = dynamic_cast<PointSet*>(mpInput);
    if (pMesh)
    {
        if (!mInputAsOutput)
            pPoints = pPoints->clone();
        reshape(pPoints, mTargetSize, mRelativeOffset, mRotation);
        return true;
    }
    
    return false;
}

}

//#include "ExpeGeometryAutoReshape.moc"
