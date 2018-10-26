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



#include "ExpeEigenSphereFitter.h"

#include "ExpeNeighborhood.h"
#include "ExpeLinearAlgebra.h"
#include <gsl/gsl_linalg.h>

namespace Expe
{

EigenSphereFitter::EigenSphereFitter()
    :  mNormalization(false)
{
    mSvdMatV = gsl_matrix_alloc(N,N);
    mSvdVecS = gsl_vector_alloc(N);
    mSvdVecWork = gsl_vector_alloc(N);
}

EigenSphereFitter::~EigenSphereFitter()
{
    gsl_matrix_free(mSvdMatV);
    gsl_vector_free(mSvdVecS);
    gsl_vector_free(mSvdVecWork);
}

bool EigenSphereFitter::fit(const Neighborhood* pNeighborhood)
{
    // the constraint matrix
    static double invC[] = {
        0, 0, 0, 0, -0.5,
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
        -0.5, 0, 0, 0, 0};
    
    uint nofSamples = pNeighborhood->getNofFoundNeighbors();
    if (nofSamples<5)
    {
        return false;
    }
    
    gsl_matrix_view invC_view = gsl_matrix_view_array (invC, N, N);
    gsl_matrix_view matrixA_view = gsl_matrix_view_array ((double*)mCovMat, N, N);
    
    // directly fill the covariance matrix A'A
    for(uint i=0 ; i<N ; ++i)
        for(uint j=0 ; j<N ; ++j)
            mCovMat[i][j] = 0.;
    
    double vec[N];
    for (uint k=0; k<nofSamples; k++)
    {
        const Vector3& pt = pNeighborhood->getNeighbor(k).position();
        double w = pNeighborhood->getNeighborWeight(k);
        
        vec[0] = 1.;
        vec[1] = pt.x;
        vec[2] = pt.y;
        vec[3] = pt.z;
        vec[4] = pt.squaredLength();
        
        for(uint i=0 ; i<N ; ++i)
            for(uint j=0 ; j<N ; ++j)
                mCovMat[i][j] += vec[i]*vec[j]*w;
    }

    // Compute the determinant of the covariance matrix
    #ifndef EXPE_HAVE_OCTAVE
    #pragma message ( "#warning The octave library is not enabled => the result of the sphere fitting may be unstable for ill-conditionned covariance matrix." )
    #endif
    double det = ela::det(&matrixA_view.matrix);
    if (det==0)
    {
        // this means either an exact fit is possible or that the problem is underconstrained
        // then algebraic coeffs == cofactors of A'A
        double cofactors[N];
        double norm = 0.;
        double sign = (N%2) ? -1. : 1.;
        gsl_matrix* subMat = gsl_matrix_alloc(N-1,N-1);
        for(uint k=0 ; k<N ; ++k)
        {
            // fill the sub matrix
            uint j1 = 0;
            for(uint j=0 ; j<N ; ++j)
            {
                if(j!=k)
                {
                    for(uint i=0 ; i<N-1 ; ++i)
                    {
                        gsl_matrix_set(subMat, i, j1, gsl_matrix_get(&matrixA_view.matrix, i, j));
                    }
                    j1++;
                }
            }
            cofactors[k] = sign*ela::det(subMat);
            norm += cofactors[k]*cofactors[k];
//             det += cofactors[k] * gsl_matrix_get(&matrixA_view.matrix, N-1, k);
            sign = -sign;
        }
        gsl_matrix_free(subMat);
        
        norm = Math::Sqrt(norm);
        if (norm<1e-4)
            return false;
        
        // copy the result
        for(uint k=0 ; k<N ; ++k)
        {
            u[k] = cofactors[k];
        }
        
//         {
//             this->endEdit();
//             Vector3 mean = Vector3::ZERO;
//             Real sumW = 0.;
//             for (uint k=0; k<nofSamples; k++)
//             {
//                 const Vector3& pt = pNeighborhood->getNeighbor(k).position();
//                 Real w = pNeighborhood->getNeighborWeight(k);
//                 mean += w*pt;
//                 sumW += w;
//             }
//             mean += sumW;
//             Real d = this->euclideanDistance(mean);
//             //if (!Math::isFinite(d))
//             if (!(d>-1e4 && d<1e4))
//             {
//                 std::cout << "det == 0" << d << " " << this->project(mean) << " sumW=" << sumW << "\n";
//                 for (uint k=0 ; k<N ; ++k)
//                     std::cout << u[k] << " ";
//                 std::cout << "\n";
//             }
//         }
    }
    else
    {
        // use the cholesky decomposition to solve the generalized eigen problem
        // as a simple eigen problem
        gsl_matrix *eigen_vectors = gsl_matrix_alloc(N,N);
        gsl_vector *eigen_values = gsl_vector_alloc(N);
        if(!ela::eigen_generalized_sym(&invC_view.matrix, &matrixA_view.matrix, eigen_values, eigen_vectors, ela::GEF_ABx_lx))
        {
            std::cerr << "ela::eigen_generalized_sym failed:\n"; 
            ela::print_matrix(&matrixA_view.matrix);
            std::cerr << "\tnofSamples = " << nofSamples << "\n";
            std::cerr << "\tdet(AA) = " << det << "\n\n";
            return false;
        }

        // search the lowest positive eigen value
        int lowestId = -1;
        for (uint i=0 ; i<N ; ++i)
        {
            double ev = gsl_vector_get(eigen_values, i);
            if(fabs(ev)<1e-9)
                ev = 0;
            if ((ev>0) && (lowestId==-1 || ev<gsl_vector_get(eigen_values, lowestId)))
            {
                lowestId = i;
            }
        }
        
        // keep the value of the eigen value for the gradient calculation
        mLambda = gsl_vector_get(eigen_values, lowestId);
        
        for (uint i=0 ; i<N ; ++i)
            mVecU[i] = gsl_matrix_get(eigen_vectors, i, lowestId);
        
        double norm = mVecU[1]*mVecU[1]+mVecU[2]*mVecU[2]+mVecU[3]*mVecU[3] - 4.*mVecU[0]*mVecU[N-1];
//         std::cout << "norm = " << norm << "\n";
        norm = 1./sqrtf(norm);
        for (uint i=0 ; i<N ; ++i)
            mVecU[i] *= norm;

        gsl_matrix_free(eigen_vectors);
        gsl_vector_free(eigen_values);
        
        // copy
        for (uint i=0 ; i<N ; ++i)
            u[i] = mVecU[i];
    }
    
    this->endEdit();

    return true;
}



Vector3 EigenSphereFitter::mlsGradient(const Neighborhood* pNeighborhood, const Vector3& source) const
{
    double matC[] = {
        0, 0, 0, 0,-2,
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
       -2, 0, 0, 0, 0};

    // offset used to handle a half matrix
    uint nofSamples = pNeighborhood->getNofFoundNeighbors();

    Vector3 grad;
    // for each coordinate
    for (uint k=0 ; k<3 ; ++k)
    {
        for(uint i=0 ; i<N ; ++i)
            for(uint j=0 ; j<N ; ++j)
                mMatdA[i][j] = 0.;
        
        double vec[N];
        for (uint i=0; i<nofSamples; i++)
        {
            Vector3 p = pNeighborhood->getNeighbor(i).position();
            double dwk = 2.*(source[k]-p[k])*pNeighborhood->getNeighborDerivativeWeight(i);
            
            vec[0] = 1.;
            vec[1] = p.x;
            vec[2] = p.y;
            vec[3] = p.z;
            vec[4] = p.squaredLength();
            
            for(uint i1=0 ; i1<N ; ++i1)
                for(uint j1=0 ; j1<N ; ++j1)
                    mMatdA[i1][j1] += vec[i1]*vec[j1]*dwk;
        }

        // compute dlambda = x^T dA x
        double aux[N];
        double dlambda = 0.;
        for (uint i=0 ; i<N ; ++i)
        {
            aux[i] = 0.;
            for (uint j=0 ; j<N ; ++j)
                aux[i] += mMatdA[i][j] * mVecU[j];
        }
        for (uint i=0 ; i<N ; ++i)
            dlambda += aux[i]*mVecU[i];
        
        // mMatdA <- dA - dlambda * C
        // mMatA <- A - lambda * C
//         for (uint i=1 ; i<N-1 ; ++i)
//         {
//             mMatdA[i][i] -= dlambda;
//             mMatA[i][i]  -= mLambda;
//         }
//         mMatdA[0][N-1] -= 2.*dlambda;
//         mMatA[0][N-1] -= 2.*mLambda;
//         mMatdA[N-1][0] -= 2.*dlambda;
//         mMatA[N-1][0] -= 2.*mLambda;

        double dlcda[N][N]; // = dlambda * C - dA
        double alc[N][N]; // = A - lambda * C
        for (uint i=0 ; i<N ; ++i)
        {
            for (uint j=0 ; j<N ; ++j)
            {
                dlcda[i][j] = dlambda*matC[i*N+j] - mMatdA[i][j];
                alc[i][j] = mCovMat[i][j] - mLambda*matC[i*N+j];
            }
        }
        
        // vecB = dlcda * x (=aux)
        for (uint i=0 ; i<N ; ++i)
        {
            aux[i] = 0.;
            for (uint j=0 ; j<N ; ++j)
                aux[i] += dlcda[i][j] * mVecU[j];
        }
        
        // dX/de_k =  inv(alc) * dlcda * x = inv(alc) * vecB
        // but since alc is singular we use a SVD as follow:
        
        double dU[N]; // du/dx_k =  inv(alc) * dlcda * u
        gsl_vector_view dU_view = gsl_vector_view_array((double*)dU, N);
        gsl_vector_view vecB_view = gsl_vector_view_array((double*)aux, N);
        gsl_matrix_view alc_view = gsl_matrix_view_array((double*)alc, N, N);
        
        gsl_linalg_SV_decomp(&alc_view.matrix, mSvdMatV, mSvdVecS, mSvdVecWork);
        gsl_linalg_SV_solve (&alc_view.matrix, mSvdMatV, mSvdVecS, &vecB_view.vector, &dU_view.vector);
        
//         for (uint i=1 ; i<N-1 ; ++i)
//             dU[i] = -dU[i];


//         EigenvectorDerivativeFramework<5> solver;
//         double dU[N];
//         solver.solve(mMatA, mMatdA, mLambda, mVecX, dU);

        grad[k] = dU[0] + dU[1]*source[0]+dU[2]*source[1]+dU[3]*source[2] + dU[4]*source.squaredLength()
            + mVecU[1+k] + 2.*source[k]*mVecU[4];
    }

    return grad;
}


}


 
