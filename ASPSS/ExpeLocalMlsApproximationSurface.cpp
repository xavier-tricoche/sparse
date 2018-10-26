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



#include "ExpeLocalMlsApproximationSurface.h"

#include "ExpeNeighborhood.h"
#include "ExpeWeightingFunction.h"
//#include "ExpeLazzyUi.h"

namespace Expe
{

/*ELU_DECLARE(LocalMlsApproximationSurface,
    ELU_PROPERTY("StartProjWithClosest",ELU_OPT("description",ELU_STR("Start the projection with the closest sample instead of the initial point.")))
    ELU_PROPERTY("ProjectionMode",ELU_OPT("description",ELU_STR("Different type of projections can be applied.")))
    ELU_PROPERTY("DomainMinNofSamples",ELU_OPT("description",ELU_STR("Minimum number of samples used to define the restricted definition domain."))
        ELU_OPT("min",1) ELU_OPT("step",1))
    ELU_PROPERTY("CheckRestrictedDomain",ELU_OPT("description",ELU_STR("Enable/disable the automatic restricted domain definition at the end of the projection procedure.")))
    ELU_PROPERTY("DomainRadiusScale",ELU_OPT("description",ELU_STR("Scale factor of the sample radii used to define the restricted definition domain."))
        ELU_OPT("min",0.) ELU_OPT("step",0.1))
    ELU_PROPERTY("DomainNormalScale",ELU_OPT("description",ELU_STR("Scale factor along the normal direction used to define the restricted definition domain."))
        ELU_OPT("min",0.) ELU_OPT("max",1.) ELU_OPT("step",0.1))
);*/

LocalMlsApproximationSurface::LocalMlsApproximationSurface(ConstPointSetPtr pPoints)
    : MlsSurface(pPoints)
{
    mDomainNormalScale = 0.9;
    mDomainRadiusScale = 1.5;
    mDomainMinNofSamples = 4;
    mStartProjWithClosest = false;
    mCheckRestrictedDomain = true;
    mProjectionMode = PM_Basic;
}

LocalMlsApproximationSurface::~LocalMlsApproximationSurface()
{
}

void LocalMlsApproximationSurface::initSession(void)
{
}

Real LocalMlsApproximationSurface::potentiel(const Vector3& position) const
{
    // find the neighors and compute their weights
    mNeighborhood->computeNeighborhood(position, mWeightingFunction);
    if (mNeighborhood->getNofFoundNeighbors()==0)
        return 1e9;
    
    // compute the local approximation
    if (!const_cast<LocalMlsApproximationSurface*>(this)->fit(position))
        return 1e9;
    mCachedPosition = position;
    
    // and compute the scalar field
    return const_cast<LocalMlsApproximationSurface*>(this)->evalPotentiel(position);
}

Vector3 LocalMlsApproximationSurface::gradient(const Vector3& position) const
{
    if (mCachedPosition!=position)
    {
        mNeighborhood->computeNeighborhood(position, mWeightingFunction);
        const_cast<LocalMlsApproximationSurface*>(this)->fit(position);
        mCachedPosition = position;
    }
    
	return const_cast<LocalMlsApproximationSurface*>(this)->evalMlsGradient(position);
}

bool LocalMlsApproximationSurface::project(Vector3& position, Vector3& normal, Color& color) const
{
    uint iterationCount = 0;
    Vector3 source = position;
    float delta;
    float epsilon = mProjectionAccuracy * mObjectScale;
    
    if (mStartProjWithClosest)
    {
        mNeighborhood->computeNeighborhood(position, mWeightingFunction);
        uint nb = mNeighborhood->getNofFoundNeighbors();
        if (nb==0)
            return false;
        uint closestId = 0;
        for (uint i=1 ; i<nb ; ++i)
        {
            if (mNeighborhood->getNeighborSquaredDistance(i)<mNeighborhood->getNeighborSquaredDistance(closestId))
                closestId = i;
        }
        position = mNeighborhood->getNeighbor(closestId).position();
        mNeighborhood->computeNeighborhood(position, mWeightingFunction);
        if (!const_cast<LocalMlsApproximationSurface*>(this)->fit(position))
            return false;
        mCachedPosition = position;
        if (!const_cast<LocalMlsApproximationSurface*>(this)->doProjection(position, normal, delta))
            return false;
        position = position.midPoint(source);
    }
    
    if (mProjectionMode==PM_Basic)
    {
        do {
            mNeighborhood->computeNeighborhood(position, mWeightingFunction);
            if (!const_cast<LocalMlsApproximationSurface*>(this)->fit(position))
                return false;
            mCachedPosition = position;

            if (!const_cast<LocalMlsApproximationSurface*>(this)->doProjection(position,normal,delta))
                return false;
        } while ( delta<epsilon && ++iterationCount<mMaxNofProjectionIterations);
    }
    else
    {
        Vector3 oldPosition;
        do {
            mNeighborhood->computeNeighborhood(position, mWeightingFunction);
            if (!const_cast<LocalMlsApproximationSurface*>(this)->fit(position))
                return false;
            mCachedPosition = position;

            oldPosition = position;
            position = source;
            const_cast<LocalMlsApproximationSurface*>(this)->doProjection(position,normal,delta);
            delta = oldPosition.distanceTo(position);
        } while ( delta<epsilon && ++iterationCount<mMaxNofProjectionIterations);
    }
    
    if (delta>epsilon)
        return false;
    
    if (mCheckRestrictedDomain)
    {
        // we don't need to recompute the neighborhood here
        mCachedPosition = position;
        // check if the point is into the surface definition domain
        if (!isValid(position))
            return false;
    }
    
    return true;
}

bool LocalMlsApproximationSurface::isValid(const Vector3& position) const
{
    if (mCachedPosition!=position)
    {
        mNeighborhood->computeNeighborhood(position, mWeightingFunction);
    }
    
    uint nb = mNeighborhood->getNofFoundNeighbors();
    if (nb<mDomainMinNofSamples)
    {
        return false;
    }
    
    uint i=0;
    bool out = true;
	bool hasNormal = mpInputPoints->hasAttribute((UberVectorBaseT<_PointSetBuiltinData>::Attribute)(PointSet::Attribute_normal));
    if (mDomainNormalScale==1.f || (!hasNormal))
    {
        while (out && i<nb)
        {
            Real rs2 = mNeighborhood->getNeighbor(i).radius() * mDomainRadiusScale;
            rs2 = rs2*rs2;
            out = mNeighborhood->getNeighborSquaredDistance(i) > rs2;
            ++i;
        }
    }
    else
    {
        Real s = 1./(mDomainNormalScale*mDomainNormalScale) - 1.f;
        while (out && i<nb)
        {
            Real rs2 = mNeighborhood->getNeighbor(i).radius() * mDomainRadiusScale;
            rs2 = rs2*rs2;
            Real dn = mNeighborhood->getNeighbor(i).normal().dot(position-mNeighborhood->getNeighbor(i).position());
            out = (mNeighborhood->getNeighborSquaredDistance(i) + s*dn*dn ) > rs2;
            ++i;
        }
    }
    return !out;
}

void LocalMlsApproximationSurface::flushStatistics(void)
{
}


}

//#include "ExpeLocalMlsApproximationSurface.moc"

