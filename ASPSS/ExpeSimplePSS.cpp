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



#include "ExpeSimplePSS.h"
#include "ExpeStaticInitializer.h"

namespace Expe
{

//EXPE_INIT_CODE(SimplePSS) {
//    MlsSurfaceManager::InstanceSafe().registerFactory(new MlsSurfaceFactoryCreator<SimplePSS>());
//}

SimplePSS::SimplePSS(ConstPointSetPtr pPoints)
 : LocalMlsApproximationSurface(pPoints)
{
}

SimplePSS::~SimplePSS()
{
}

bool SimplePSS::fit(const Vector3& position)
{
    uint nofSamples = mNeighborhood->getNofFoundNeighbors();
    if (nofSamples<2)
        return false;
    
    Vector3d mean = Vector3d::ZERO;
    Vector3d normal = Vector3d::ZERO;
    double sumW = 0.;
    for (uint i=0; i<nofSamples; i++)
    {
        double w = mNeighborhood->getNeighborWeight(i);
        PointSet::ConstPointHandle nei = mNeighborhood->getNeighbor(i);
        mean += w * vector_cast<Vector3d>(nei.position());
        normal += w * vector_cast<Vector3d>(nei.normal());
        sumW += w;
    }
    
    if(sumW==0.)
    {
        LOG_ERROR("SimplePSS: the sum of weight is null.");
        return false;
    }
    mean /= sumW;
    normal.normalize();
    mOffset = -mean.dot(normal);
    mNormal = vector_cast<Vector3>(normal);
    
    return true;
}

Real SimplePSS::evalPotentiel(const Vector3& position)
{
    return mNormal.dot(position) + mOffset;
}

Vector3 SimplePSS::evalMlsGradient(const Vector3& position)
{
//     return mNormal;
    
    Vector3 grad;
    Vector3d source = vector_cast<Vector3d>(position);
    
    // size of the problem
    uint nofSamples = mNeighborhood->getNofFoundNeighbors();
    
    Vector3d mean = Vector3d::ZERO;
    Vector3d avgN = Vector3d::ZERO;
    double wSum = 0.;
    for(uint i=0 ; i<nofSamples ; ++i)
    {
        double w = mNeighborhood->getNeighborWeight(i);
        mean += w * vector_cast<Vector3d>(mNeighborhood->getNeighbor(i).position());
        avgN += w * vector_cast<Vector3d>(mNeighborhood->getNeighbor(i).normal());
        wSum += w;
    }
    mean /= wSum;
    avgN /= wSum;

    for (uint k=0 ; k<3 ; ++k)
    {
        // compute dMean and dN
        Vector3d dMean(Vector3d::ZERO);
        Vector3d dN(Vector3d::ZERO);
        double dwkSum = 0.;
        for (uint i=0; i<nofSamples; i++)
        {
            Vector3d p = vector_cast<Vector3d>(mNeighborhood->getNeighbor(i).position());
            Vector3d n = vector_cast<Vector3d>(mNeighborhood->getNeighbor(i).normal());
            double dwk = 2.*(source[k]-p[k])*mNeighborhood->getNeighborDerivativeWeight(i);
            dwkSum += dwk;
            dMean += dwk*p;
            dN += dwk*n;
        }
        dMean = (dMean-mean*dwkSum)/wSum;
        dN = (dN-avgN*dwkSum)/wSum;
        //dN[k] -= 1.;
        
        dMean[k] -= 1.;
        grad[k] = dN.dot(source-mean) + vector_cast<Vector3d>(mNormal).dot(-dMean);
    }
    
    return grad;
}

bool SimplePSS::doProjection(Vector3& position, Vector3& normal, Real& delta)
{
    delta = mNormal.dot(position) + mOffset;
    position = position - delta * mNormal;
    normal = mNormal;
    return true;
}

}

//#include "ExpeSimplePSS.moc"


