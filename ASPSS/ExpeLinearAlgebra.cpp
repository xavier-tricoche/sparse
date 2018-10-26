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



#include "ExpeLinearAlgebra.h"

#include "ExpeLogManager.h"

#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_linalg.h>

#ifdef EXPE_HAVE_OCTAVE
    #include <octave/octave/config.h>
    #include <octave/octave/EIG.h>
    #include <octave/dbleDET.h>
#endif

namespace Expe
{

namespace ela
{

#ifdef EXPE_HAVE_OCTAVE
int eigen_by_octave(gsl_matrix* matA, gsl_vector* evals, gsl_matrix* evecs)
{
    size_t pbSize = matA->size1;
    
    // prepare the matrix for octave
    Matrix octaveMatA(pbSize, pbSize);
    for (uint i=0 ; i<pbSize ; ++i)
    for (uint j=0 ; j<pbSize ; ++j)
        octaveMatA.elem(i,j) = gsl_matrix_get(matA,i,j);
    
    int info;
    EIG octaveEigen(octaveMatA, info, true);
    
    // get the eigen values and vectors
    for (uint i=0 ; i<pbSize ; ++i)
    for (uint j=0 ; j<pbSize ; ++j)
    {
        if (i==j)
        {
            if(octaveEigen.eigenvalues().elem(i).imag()==0)
            {
                gsl_vector_set(evals, i, octaveEigen.eigenvalues().elem(i).real());
            }
            else
            {
                std::cout << "eigenvalues.imag != 0\n";
                gsl_vector_set(evals, i, -1e-9);
            }
        }
        
        gsl_matrix_set(evecs, i, j, octaveEigen.eigenvectors().elem(i,j).real());
        
        if(octaveEigen.eigenvectors().elem(i,j).imag()!=0)
            std::cout << "eigenvectors.imag != 0\n";
    }
    
//     if(info!=0)
//     {
//         std::cout <<  "eigen_by_octave: info = " << info << "\n";
//         std::cout << "  " << octaveEigen.eigenvalues() << "\n" << octaveEigen.eigenvectors() << "\n\n";
//     }
    
    return (info==0 ? 1 : 0);
}
#endif

double det(gsl_matrix* m)
{
    #ifndef EXPE_HAVE_OCTAVE
    gsl_permutation* p = gsl_permutation_alloc(m->size1);
    int i;
    gsl_matrix* LU = gsl_matrix_alloc(m->size1,m->size2);
    gsl_matrix_memcpy(LU,m);
    gsl_linalg_LU_decomp (LU, p, &i);
    double d = gsl_linalg_LU_det(LU,i);
    gsl_matrix_free(LU);
    gsl_permutation_free(p);
    return d;
    #else
    // prepare the matrix for octave
    Matrix octaveMatA(m->size1, m->size2);
    for (uint i=0 ; i<m->size1 ; ++i)
        for (uint j=0 ; j<m->size2 ; ++j)
            octaveMatA.elem(i,j) = gsl_matrix_get(m,i,j);
        
    int info;
    double rcond = 0.0;
    DET det = octaveMatA.determinant(info, rcond);
    volatile double xrcond = rcond;
    xrcond += 1.0;
    return ((info == -1 || xrcond == 1.0) ? 0.0 : det.value ());
    #endif
}

int eigen_generalized_sym(gsl_matrix* matA, gsl_matrix* matB, gsl_vector* evals, gsl_matrix* evecs, GenEigenForm form)
{
    size_t pbSize = matA->size1;
    assert(matA->size2==pbSize);
    assert(matB->size1==pbSize);
    assert(matB->size2==pbSize);
    assert(evecs->size1==pbSize);
    assert(evecs->size2==pbSize);
    assert(evals->size==pbSize);
    
    // Cholesky of matB -> U'U
    gsl_matrix *matU = gsl_matrix_alloc(pbSize,pbSize);
    gsl_matrix_memcpy(matU, matB);
    if(gsl_linalg_cholesky_decomp(matU))
        return 0;
    
    // clean the strict lower triangular part
    for (uint m=1 ; m<pbSize ; ++m)
        for (uint n=0 ; n<m ; ++n)
            gsl_matrix_set(matU, m, n, 0);
    
    gsl_matrix *matC = gsl_matrix_alloc(pbSize,pbSize);
    gsl_matrix *matAux = gsl_matrix_alloc(pbSize,pbSize);
    gsl_eigen_symmv_workspace *workspace = gsl_eigen_symmv_alloc(pbSize);
    
    if(form==GEF_ABx_lx)
    {
        // compute C = UAU'
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, matU, matA, 0.0, matAux);
        gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, matAux, matU, 0.0, matC);
        
        // eigen system
        if (gsl_eigen_symmv(matC, evals, evecs, workspace))
        {
            gsl_matrix_free(matU);
            gsl_matrix_free(matC);
            gsl_eigen_symmv_free(workspace);
            gsl_matrix_free(matAux);
            return 0;
        }
    
        // transform the eigen vectors: evecs = inv(U) * evecs
        gsl_blas_dtrsm(CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit, 1.0, matU, evecs);
    }
    else if(form==GEF_BAx_lx)
    {
        // compute C = UAU'
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, matU, matA, 0.0, matAux);
        gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, matAux, matU, 0.0, matC);
        
        // eigen system
        if (gsl_eigen_symmv(matC, evals, evecs, workspace))
        {
            gsl_matrix_free(matU);
            gsl_matrix_free(matC);
            gsl_eigen_symmv_free(workspace);
            gsl_matrix_free(matAux);
            return 0;
        }
    
        // transform the eigen vectors: evecs = U' * evecs
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, matU, evecs, 0.0, evecs);
    }
    else if(form==GEF_Ax_lBx)
    {
        // compute C = inv(U').A.inv(U)
        gsl_matrix_memcpy(matC, matA);
        gsl_blas_dtrsm(CblasLeft, CblasUpper, CblasTrans, CblasNonUnit, 1.0, matU, matC);
        gsl_blas_dtrsm(CblasRight, CblasUpper, CblasNoTrans, CblasNonUnit, 1.0, matU, matC);
        
        // eigen system
        if (gsl_eigen_symmv(matC, evals, evecs, workspace))
        {
            gsl_matrix_free(matU);
            gsl_matrix_free(matC);
            gsl_eigen_symmv_free(workspace);
            gsl_matrix_free(matAux);
            return 0;
        }
    
        // transform the eigen vectors: evecs = inv(U) * evecs
        gsl_blas_dtrsm(CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit, 1.0, matU, evecs);
    }
    else
    {
        LOG_ERROR("eigen_generalized_sym: invalid form");
        gsl_matrix_free(matU);
        gsl_matrix_free(matC);
        gsl_eigen_symmv_free(workspace);
        gsl_matrix_free(matAux);
        return 0;
    }
    
    gsl_matrix_free(matU);
    gsl_matrix_free(matC);
    gsl_matrix_free(matAux);
    gsl_eigen_symmv_free(workspace);
    
    return 1;
}

void print_matrix(gsl_matrix* m)
{
    for(uint i=0 ; i<m->size1 ; ++i)
    {
        for(uint j=0 ; j<m->size2 ; ++j)
        {
            std::cout << gsl_matrix_get(m, i, j);
            if(j+1!=m->size2)
                std::cout << ",\t";
        }
        std::cout << ";\n";
    }
    std::cout << "\n";
}

}

}

