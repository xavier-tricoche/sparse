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



#include "ExpeNormalConstrainedSphereFitter.h"

#include "ExpeNeighborhood.h"

namespace Expe
{

NormalConstrainedSphereFitter::NormalConstrainedSphereFitter()
    : mNormalParameter(1e6), mNormalization(false)
{
}

NormalConstrainedSphereFitter::~NormalConstrainedSphereFitter()
{
}

bool NormalConstrainedSphereFitter::fit(const Neighborhood* pNeighborhood)
{
    LocalFloat beta;
    if (mNormalization)
        beta = mNormalParameter*pNeighborhood->getCurrentFilterRadius();
    else
        beta = mNormalParameter;
    beta = beta * beta;
    
    uint nofSamples = pNeighborhood->getNofFoundNeighbors();
    if (nofSamples==0)
    {
        return false;
    }
    else if (nofSamples==1)
    {
        u13() = pNeighborhood->getNeighbor(0).normal();
        u[0] = -pNeighborhood->getNeighbor(0).position().dot(u13());
        u[4] = 0.;
        return true;
    }

    /** We directly compute the symetric covariance matrix taking advantage of the structure of the design matrix A:

        A'A = m     s(xi)     s(yi)      s(zi)   s(|pi|^2)
              "  s(xi^2)+m  s(xi.yi)  s(xi.zi)   s(xi.|pi|^2) + 2s(xi)
              "        "   s(yi^2)+m  s(yi.zi)   s(yi.|pi|^2) + 2s(yi)
              "        "       "      s(zi^2)+m  s(zi.|pi|^2) + 2s(zi)
              "        "       "          "      s(|pi|^4) + 4(s(xi^2)+s(yi^2)+s(zi^2))

        and A'b = 0 s(nxi) s(nyi) s(nzi) 2.(s(xi.nxi)+s(yi.nyi)+s(zi.nzi))
        
        where s(xi) means \sum_i {x_i} and m is the number of samples.
        
        Note that the weights are missing in this formula (just replace xi by wi.xi and m by s(wi) ...
    */
    
    /*  Maybe we could also compute the QR factorisation of A that directly gives us the cholesky decomposition of A'A.
        On the other hand, the QR factorisation is probably more expensive as long as the number of samples increases.
    */
    
    // clear the data
    for(uint i=0 ; i<5 ; ++i)
    {
        mVecB[i] = 0.;
        for(uint j=0 ; j<5 ; ++j)
            mCovMat[i][j] = 0.;
    }
    
    // fill the covariance matrix and value vector
    for (uint i=0; i<nofSamples; i++)
    {
        PointSet::ConstPointHandle nei = pNeighborhood->getNeighbor(i);
        
        LocalVector3 p = vector_cast<LocalVector3>(nei.position());
        LocalVector3 n = vector_cast<LocalVector3>(nei.normal());
        LocalFloat w = pNeighborhood->getNeighborWeight(i);
        LocalFloat l2 = p.squaredLength();
        LocalVector3 wp = w * p;
        LocalFloat wl2 = w*l2;
        
        mCovMat[0][0] += w;
        
        mCovMat[1][1] += wp.x*p.x;
        mCovMat[2][2] += wp.y*p.y;
        mCovMat[3][3] += wp.z*p.z;
        mCovMat[4][4] += wl2*l2;
        
        mCovMat[0][1] += wp.x;
        mCovMat[0][2] += wp.y;
        mCovMat[0][3] += wp.z;
        mCovMat[0][4] += wl2;
        
        mCovMat[1][2] += wp.x*p.y;
        mCovMat[1][3] += wp.x*p.z;
        mCovMat[2][3] += wp.y*p.z;
        
        mCovMat[1][4] += wp.x*l2;
        mCovMat[2][4] += wp.y*l2;
        mCovMat[3][4] += wp.z*l2;
        
        mVecB[1] += w*n.x;
        mVecB[2] += w*n.y;
        mVecB[3] += w*n.z;
        mVecB[4] += w*p.dot(n);
    }
    
    // finish the work
    mCovMat[1][4] += beta*2.*mCovMat[0][1];
    mCovMat[2][4] += beta*2.*mCovMat[0][2];
    mCovMat[3][4] += beta*2.*mCovMat[0][3];
    mCovMat[4][4] += beta*4.*(mCovMat[1][1]+mCovMat[2][2]+mCovMat[3][3]);
    
    mCovMat[1][1] += beta*mCovMat[0][0];
    mCovMat[2][2] += beta*mCovMat[0][0];
    mCovMat[3][3] += beta*mCovMat[0][0];
    
    mVecB[1] *= beta;
    mVecB[2] *= beta;
    mVecB[3] *= beta;
    mVecB[4] *= beta*2.;
    
    // Note that only the upper triangle part of mCovMat is valid.
    // Now we solve mCovMat * x = mVecB with mCovMat symmetric !
    // => we use a fast cholesky decomposition without square root.
    
    // Cholesky without square root : mCovMat = U'DU,
    // Note that at the end D == mCovMat
    for (uint i=0 ; i<N ; ++i)
    {
        // Commented because it is not use
        // U[i][i] = 1;
        for (uint j=i+1 ; j<N ; ++j)
        {
            mMatU[i][j] = mCovMat[i][j]/mCovMat[i][i];
            for (uint k=j ; k<N ; ++k)
                mCovMat[j][k] -= mMatU[i][j]*mCovMat[i][k];
        }
    }
    
    // alias to save memory allocation.
    LocalFloat* vecX = mVecB;
    
    // forward substitution : x <- inv(U') * x
    for (uint i=1 ; i<5 ; ++i)
    {
        LocalFloat& tmp = vecX[i];
        for (uint j=0 ; j<i ; ++j)
            tmp -= mMatU[j][i] * vecX[j];
    }
    
    // x <- inv(D) * x  (remind that D == matcov)
    for (uint i=0 ; i<5 ; ++i)
        vecX[i] /= mCovMat[i][i];
    
    // backsubstitution : x <- inv(U) * x
    for (int i=5-2 ; i>=0 ; --i)
    {
        LocalFloat& tmp = vecX[i];
        for (uint j = i+1 ; j < 5 ; j++)
            tmp -= mMatU[i][j] * vecX[j];
    }
    
    // copy
    for (uint i=0 ; i<5 ; ++i)
        u[i] = vecX[i];
    
    this->endEdit();

    return true;
}

Vector3 NormalConstrainedSphereFitter::mlsGradient(const Neighborhood* pNeighborhood, const Vector3& position) const
{
    Vector3 grad;
    
    LocalVector3 x = vector_cast<LocalVector3>(position);
    
    LocalFloat beta;
    if (mNormalization)
        beta = mNormalParameter*pNeighborhood->getCurrentFilterRadius();
    else
        beta = mNormalParameter;
    beta *= beta;
    
    // size of the problem
    const uint N = 5;
    // offset used to handle a half matrix
    uint nofSamples = pNeighborhood->getNofFoundNeighbors();
        
    LocalFloat matA[3][5][5];
    LocalFloat vecB[3][5];
    // clear the data
    for(uint k=0 ; k<3 ; ++k)
    {
        for(uint i=0 ; i<N ; ++i)
        {
            vecB[k][i] = 0.;
            for(uint j=0 ; j<N ; ++j)
                matA[k][i][j] = 0.;
        }
    }
    
    // fill the derivative matrices
    for (uint i=0; i<nofSamples; i++)
    {
        PointSet::ConstPointHandle nei = pNeighborhood->getNeighbor(i);
        LocalVector3 p = vector_cast<LocalVector3>(nei.position());
        LocalVector3 n = vector_cast<LocalVector3>(nei.normal());
        
        LocalVector3 dw = 2.*(x-p)*pNeighborhood->getNeighborDerivativeWeight(i);
        
        LocalFloat l2 = p.squaredLength();
        
        for (uint k=0 ; k<3 ; ++k)
        {
            LocalVector3 wp = dw[k]*p;
            LocalFloat wl2 = dw[k]*l2;
        
            matA[k][0][0] += dw[k];
            
            matA[k][1][1] += wp.x*p.x;
            matA[k][2][2] += wp.y*p.y;
            matA[k][3][3] += wp.z*p.z;
            matA[k][4][4] += wl2*l2;
            
            matA[k][0][1] += wp.x;
            matA[k][0][2] += wp.y;
            matA[k][0][3] += wp.z;
            matA[k][0][4] += wl2;
            
            matA[k][1][2] += wp.x*p.y;
            matA[k][1][3] += wp.x*p.z;
            matA[k][2][3] += wp.y*p.z;
            
            matA[k][1][4] += wp.x*l2;
            matA[k][2][4] += wp.y*l2;
            matA[k][3][4] += wp.z*l2;
            
            vecB[k][1] += dw[k]*n.x;
            vecB[k][2] += dw[k]*n.y;
            vecB[k][3] += dw[k]*n.z;
            vecB[k][4] += dw[k]*p.dot(n);
        }
    }
    
    for (uint k=0 ; k<3 ; ++k)
    {
        // finish the work
        matA[k][1][4] += beta*2.*matA[k][0][1];
        matA[k][2][4] += beta*2.*matA[k][0][2];
        matA[k][3][4] += beta*2.*matA[k][0][3];
        matA[k][4][4] += beta*4.*(matA[k][1][1]+matA[k][2][2]+matA[k][3][3]);
        
        matA[k][1][1] += beta*matA[k][0][0];
        matA[k][2][2] += beta*matA[k][0][0];
        matA[k][3][3] += beta*matA[k][0][0];
        
        // copy the upper triangular to the lower triangular
        matA[k][1][0] = matA[k][0][1];
        matA[k][2][0] = matA[k][0][2];
        matA[k][3][0] = matA[k][0][3];
        matA[k][4][0] = matA[k][0][4];
        matA[k][2][1] = matA[k][1][2];
        matA[k][3][1] = matA[k][1][3];
        matA[k][4][1] = matA[k][1][4];
        matA[k][3][2] = matA[k][2][3];
        matA[k][4][2] = matA[k][2][4];
        matA[k][4][3] = matA[k][3][4];
        
        vecB[k][1] *= beta;
        vecB[k][2] *= beta;
        vecB[k][3] *= beta;
        vecB[k][4] *= beta*2.;
    }
    
    // alias
    const LocalFloat* vecX = mVecB;
    
    for (uint k=0 ; k<3 ; ++k)
    {
        LocalFloat aux[5];
        
        // compute aux_k = dA_k * vecX
        for(uint i=0 ; i<N ; ++i)
        {
            aux[i] = 0.;
            for(uint j=0 ; j<N ; ++j)
                aux[i] += matA[k][i][j] * vecX[j];
        }
        
        // aux = -aux + db_k
        for(uint i=0 ; i<N ; ++i)
        {
            aux[i] = vecB[k][i] - aux[i];
        }
        
        // aux = inv(A)*aux
    
        // forward substitution : aux <- inv(U') * aux
        for (uint i=1 ; i<N ; ++i)
        {
            LocalFloat& tmp = aux[i];
            for (uint j=0 ; j<i ; ++j)
                tmp -= mMatU[j][i] * aux[j];
        }
        
        // aux <- inv(diag(cov)) * aux
        for (uint i=0 ; i<N ; ++i)
            aux[i] /= mCovMat[i][i];
        
        // backsubstitution : aux <- inv(U) * aux
        for (int i=N-2 ; i>=0 ; --i)
        {
            LocalFloat& tmp = aux[i];
            for (uint j = i+1 ; j < N ; j++)
                tmp -= mMatU[i][j] * aux[j];
        }
        
        //grad[k] = aux[0] + Vector3(&aux[1]).dotProduct(source) + aux[4]*source.squaredLength() + vecX[1+k] + 2.*source[k]*vecX[4];
        grad[k] = aux[0] + aux[1]*x.x+aux[2]*x.y+aux[3]*x.z + aux[4]*x.squaredLength() + vecX[1+k] + 2.*x[k]*vecX[4];
    }
    
    return grad;
}

}


 
