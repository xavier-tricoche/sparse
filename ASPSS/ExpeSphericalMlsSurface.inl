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





template <class AlgebraicSphereFitterT>
bool SphericalMlsSurfaceT<AlgebraicSphereFitterT>::fit(const Vector3& position)
{
    if (mNeighborhood->getNofFoundNeighbors()<4)
        return false;
    return mAlgebraicSphere.fit(mNeighborhood);
}

template <class AlgebraicSphereFitterT>
Real SphericalMlsSurfaceT<AlgebraicSphereFitterT>::evalPotentiel(const Vector3& position)
{
    return mAlgebraicSphere.euclideanDistance(position);
}

template <class AlgebraicSphereFitterT>
Vector3 SphericalMlsSurfaceT<AlgebraicSphereFitterT>::evalMlsGradient(const Vector3& position)
{
    return mAlgebraicSphere.mlsGradient(mNeighborhood,position);
}

template <class AlgebraicSphereFitterT>
bool SphericalMlsSurfaceT<AlgebraicSphereFitterT>::doProjection(Vector3& position, Vector3& normal, Real& delta)
{
    // re-implement the projection here in order to save compuation of the approximated normal and delta value.
    if (mAlgebraicSphere.state()==ASS_SPHERE)
    {
        normal = position-mAlgebraicSphere.asSphere().center();
        delta = normal.length();
        normal /= delta;
        delta = mAlgebraicSphere.asSphere().radius() - delta;
        position = normal * mAlgebraicSphere.asSphere().radius() + mAlgebraicSphere.asSphere().center();
        if (mAlgebraicSphere.u[4]<0.)
            normal = -normal;
        return true;
    }
    
    if (mAlgebraicSphere.state()==ASS_PLANE)
    {
        normal = mAlgebraicSphere.asPlane().normal();
        delta = mAlgebraicSphere.asPlane().distanceTo(position);
        position = position - delta*normal;
        return true;
    }

    // else, tedious case, fall back to an iterative method
    Vector3 grad;
    normal = mAlgebraicSphere.u13()+2.*mAlgebraicSphere.u[4]*position;
    Real ilg = 1./normal.length();
    normal *= ilg;
    Real ad = mAlgebraicSphere.u[0] + mAlgebraicSphere.u13().dot(position) + mAlgebraicSphere.u[4] * position.squaredLength();
    delta = -ad*Math::Min<Real>(ilg,1.);
    position = position + normal*delta;
    for (int i=0 ; i<5 ; ++i)
    {
        grad = mAlgebraicSphere.u13()+2.*mAlgebraicSphere.u[4]*position;
        ilg = 1./grad.length();
        delta = -(  mAlgebraicSphere.u[0] + mAlgebraicSphere.u13().dot(position)
                + mAlgebraicSphere.u[4] * position.squaredLength())*Math::Min<Real>(ilg,1.);
        position += normal*delta;
    }
    return true;
}






template <class AlgebraicSphereFitterT>
SphericalMlsSurfaceWithoutNormalT<AlgebraicSphereFitterT>::SphericalMlsSurfaceWithoutNormalT(ConstPointSetPtr pPoints)
    : SphericalMlsSurfaceT<AlgebraicSphereFitterT>(pPoints)
{
    mInputHasNormals = pPoints->hasAttribute((UberVectorBaseT<_PointSetBuiltinData>::Attribute)PointSet::Attribute_normal);
}

template <class AlgebraicSphereFitterT>
bool SphericalMlsSurfaceWithoutNormalT<AlgebraicSphereFitterT>::fit(const Vector3& position)
{
    if (this->mNeighborhood->getNofFoundNeighbors()<5)
        return false;
    
    bool ok = this->mAlgebraicSphere.fit(this->mNeighborhood);
    if (ok && mInputHasNormals)
    {
        // set the sphere orientation to match the normal of the closest sample
        uint closestId = 0;
        for (uint i=1 ; i<this->mNeighborhood->getNofFoundNeighbors() ; ++i)
        {
            if (this->mNeighborhood->getNeighborSquaredDistance(i) < this->mNeighborhood->getNeighborSquaredDistance(closestId))
                closestId = i;
        }
        
        if (this->mNeighborhood->getNeighbor(closestId).normal().
            dot(this->mAlgebraicSphere.gradient(this->mNeighborhood->getNeighbor(closestId).position()))<0.)
        {
            for (uint k=0 ; k<5 ; ++k)
                this->mAlgebraicSphere.u[k] = -this->mAlgebraicSphere.u[k];
            this->mAlgebraicSphere.endEdit();
        }
    }
    return ok;
}

template <class AlgebraicSphereFitterT>
Real SphericalMlsSurfaceWithoutNormalT<AlgebraicSphereFitterT>::evalPotentiel(const Vector3& position)
{
    Real rawDist = this->mAlgebraicSphere.euclideanDistance(position);
    return mInputHasNormals ? rawDist : Math::Abs(rawDist);
}

template <class AlgebraicSphereFitterT>
Vector3 SphericalMlsSurfaceWithoutNormalT<AlgebraicSphereFitterT>::evalMlsGradient(const Vector3& position)
{
    Vector3 grad = this->mAlgebraicSphere.mlsGradient(this->mNeighborhood,position);
    if ( mInputHasNormals && (this->mAlgebraicSphere.gradient(position).dot(grad) < 0.) ) 
        grad = -grad;
    return grad;
}


