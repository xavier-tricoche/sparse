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



#include "ExpeIMLS.h"

#include "ExpeNeighborhood.h"
#include "ExpeWeightingFunction.h"

namespace Expe
{

IMLS::IMLS(ConstPointSetPtr pPoints)
    : MlsSurface(pPoints)
{
}

IMLS::~IMLS()
{
}

void IMLS::initSession(void)
{
}

Real IMLS::potentiel(const Vector3& position) const
{
    // find the neighors and compute their weights
    mNeighborhood->computeNeighborhood(position, mWeightingFunction);
    
    uint nofSamples = mNeighborhood->getNofFoundNeighbors();
    if (nofSamples<3)
        return 1e9;

    double potentiel = 0.;
    double sumW = 0.;
    for (uint i=0; i<nofSamples; i++)
    {
        double w = mNeighborhood->getNeighborWeight(i);
        PointSet::ConstPointHandle nei = mNeighborhood->getNeighbor(i);
        Vector3 diff = position-nei.position();
        potentiel += w * double(diff.dot(nei.normal()));
        sumW += w;
    }
    
    if(sumW==0.)
    {
        LOG_ERROR("IMLS: the sum of weight is null.");
        return 1e6;
    }
    potentiel /= sumW;
    
    return potentiel;
}

Vector3 IMLS::gradient(const Vector3& position) const
{
    if (mCachedPosition!=position)
    {
        mNeighborhood->computeNeighborhood(position, mWeightingFunction);
        
        computePotentielAndGradient(position);
    }
    
    return mCachedGradient;
}

bool IMLS::project(Vector3& position, Vector3& normal, Color& color) const
{
    uint iterationCount = 0;
    Vector3 source = position;
    float delta;
    float epsilon = mProjectionAccuracy * mObjectScale;
    do {
        mNeighborhood->computeNeighborhood(position, mWeightingFunction);
        if (!computePotentielAndGradient(position))
            return false;

        normal = mCachedGradient.normalized();
        delta = mCachedPotentiel;
        position = position - normal*delta;
    } while ( delta<epsilon && ++iterationCount<mMaxNofProjectionIterations);
    
    return true;
}

bool IMLS::isValid(const Vector3& position) const
{
    // TODO IMLS::isValid
    return true;
}

bool IMLS::computePotentielAndGradient(const Vector3& position) const
{
    uint nofSamples = mNeighborhood->getNofFoundNeighbors();
    if (nofSamples<1)
    {
        mCachedGradient = Vector3::ZERO;
        mCachedPosition = position;
        mCachedPotentiel = 1e9;
        return false;
    }
    
    // eval the gradient
    Vector3d source = vector_cast<Vector3d>(position);
    Vector3d grad = Vector3d::ZERO;
    Vector3d sumDw = Vector3d::ZERO;
    Vector3d sumDwDist = Vector3d::ZERO;
    Vector3d sumN = Vector3d::ZERO;
    double potentiel = 0.;
    double sumW = 0.;
    for (uint i=0; i<nofSamples; i++)
    {
        PointSet::ConstPointHandle nei = mNeighborhood->getNeighbor(i);
        Vector3d diff = source-vector_cast<Vector3d>(nei.position());
        Vector3d dw = 2.*diff*double(mNeighborhood->getNeighborDerivativeWeight(i));
        Vector3d normal = vector_cast<Vector3d>(nei.normal());
        double w = double(mNeighborhood->getNeighborWeight(i));
        
        double ddotn = diff.dot(normal);

        sumDw += dw;
        sumDwDist += dw*ddotn;
        sumN += w * normal;
        potentiel += w * ddotn;
        sumW += w;
    }
    potentiel /= sumW;
    mCachedGradient = vector_cast<Vector3>((1./sumW) * (-sumDw*potentiel + sumDwDist + sumN));
    mCachedPotentiel = potentiel;
    mCachedPosition = position;
    return true;
}


}

//#include "ExpeIMLS.moc"

