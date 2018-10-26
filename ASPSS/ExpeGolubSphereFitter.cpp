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



#include "ExpeGolubSphereFitter.h"

#include "ExpeNeighborhood.h"
#include "ExpeLinearAlgebra.h"
#include <gsl/gsl_linalg.h>

namespace Expe
{

GolubSphereFitter::GolubSphereFitter()
    :  mNormalization(false)
{
    mMatV = gsl_matrix_alloc(4,4);
    mVecS = gsl_vector_alloc(4);
    mVecWork = gsl_vector_alloc(4);
    mVariance = 0;
}

GolubSphereFitter::~GolubSphereFitter()
{
    gsl_matrix_free(mMatV);
    gsl_vector_free(mVecS);
    gsl_vector_free(mVecWork);
}

bool GolubSphereFitter::fit(const Neighborhood* pNeighborhood)
{
    uint nofSamples = pNeighborhood->getNofFoundNeighbors();
    if (nofSamples<5)
    {
        return false;
    }
    
    // Step 1: compute the mean
    Vector3d mean(Vector3d::ZERO);
    double wSum = 0.;
    for(uint i=0 ; i<nofSamples ; ++i)
    {
        double w = pNeighborhood->getNeighborWeight(i);
        mean += w*vector_cast<Vector3d>(pNeighborhood->getNeighbor(i).position());
        wSum += w;
    }
    if(wSum==0.)
        return false;
    mean /= wSum;
    
    // compute the variance
    double variance=0.;
    for(uint i=0 ; i<nofSamples ; ++i)
    {
        variance += pNeighborhood->getNeighborWeight(i)
            * (vector_cast<Vector3d>(pNeighborhood->getNeighbor(i).position())-mean).squaredLength();
    }
    variance /= wSum;
    
    // Step 2: compute the SVD of Q"A2
    gsl_matrix *matrixQA2 = gsl_matrix_alloc(nofSamples,4);
    for (uint i=0; i<nofSamples; ++i)
    {
        double w = Math::Sqrt(pNeighborhood->getNeighborWeight(i));
        Vector3d diff = vector_cast<Vector3d>(pNeighborhood->getNeighbor(i).position()) - mean;
        gsl_matrix_set(matrixQA2, i, 0, diff.x*w);
        gsl_matrix_set(matrixQA2, i, 1, diff.y*w);
        gsl_matrix_set(matrixQA2, i, 2, diff.z*w);
        gsl_matrix_set(matrixQA2, i, 3, (diff.squaredLength()-variance)*w);
    }
    
    gsl_linalg_SV_decomp(matrixQA2, mMatV, mVecS, mVecWork);
    gsl_matrix_free(matrixQA2);
    
    for (uint i=1 ; i<N ; ++i)
        u[i] = gsl_matrix_get(mMatV, i-1, N-2);
    u[0] = -variance*u[4];
    this->translate(vector_cast<Vector3>(mean));
    this->endEdit();
    
    mVariance = variance;

    return true;
}


}
