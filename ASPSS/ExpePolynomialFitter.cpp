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

#include "ExpePolynomialFitter.h"

namespace Expe
{

PolynomialFitter::PolynomialFitter(uint degree)
    : mDegree(degree)
{
    mNofCoeffs = (mDegree+1)*(mDegree+2)/2;
    mCoeffVector = gsl_vector_alloc(mNofCoeffs);
    mCovarianceMatrix = gsl_matrix_alloc(mNofCoeffs, mNofCoeffs);
}

PolynomialFitter::~PolynomialFitter()
{
    gsl_matrix_free(mCovarianceMatrix);
    gsl_vector_free(mCoeffVector);
}
/*
bool PolynomialFitter::fit(const WheightedNeihgborhood& neighborhood)
{
    Vector3 center = mFitter->getCenter();
    
    Vector3 zAxis =   -center;
    zAxis.normalize();
    Vector3 uAxis = zAxis.crossProduct(Vector3::UNIT_X) + zAxis.crossProduct(Vector3::UNIT_Y);//zAxis.perpendicular();
    uAxis.normalize();
    Vector3 vAxis = zAxis.crossProduct(uAxis);
    vAxis.normalize();
    
    
    uint nofNeighbors = mNeighborhood->getNofFoundNeighbors();
    
    // check the number of found neighbors (we need at least nofCoeffs neighbors)
    if (nofNeighbors < nofCoeffs)
    {
        std::cout << "doPolynomialProjection FAILED (" << nofNeighbors << ")\n";
        return false;
    }

    // allocation of least square fitting data
    gsl_matrix* designMatrix = gsl_matrix_alloc(nofNeighbors, nofCoeffs);
    gsl_vector* valueVector = gsl_vector_alloc(nofNeighbors);
    gsl_vector* weightVector = gsl_vector_alloc(nofNeighbors);
    
    for (uint i=0; i<nofNeighbors; i++)
    {
        Vector3 pt = neighborhood.getNeighbor(i).position();
        Vector3 localPt = pt-position;
        Real f = localPt.dotProduct(planeNormal);
        Real u = localPt.dotProduct(uAxis);
        Real v = localPt.dotProduct(vAxis);
        
        Real u2, v2, u3, v3, u4, v4;

        gsl_matrix_set(designMatrix, i, 0, 1);
        if(mDegree>=1) {
            gsl_matrix_set(designMatrix, i, 1, u);
            gsl_matrix_set(designMatrix, i, 2, v);
        }
        if(mDegree>=2) {
            u2 = u*u;
            v2 = v*v;
            gsl_matrix_set(designMatrix, i, 3, u2);
            gsl_matrix_set(designMatrix, i, 4, u*v);
            gsl_matrix_set(designMatrix, i, 5, v2);
        }
        if(mDegree>=3) {
            u3 = u*u2;
            v3 = v*v2;
            gsl_matrix_set(designMatrix, i, 6, u3);
            gsl_matrix_set(designMatrix, i, 7, u2*v);
            gsl_matrix_set(designMatrix, i, 8, u*v2);
            gsl_matrix_set(designMatrix, i, 9, v3);
        }
        if(mDegree>=4) {
            u4 = u2*u2;
            v4 = v2*v2;
            gsl_matrix_set(designMatrix, i, 10, u4);
            gsl_matrix_set(designMatrix, i, 11, u3*v);
            gsl_matrix_set(designMatrix, i, 12, u2*v2);
            gsl_matrix_set(designMatrix, i, 13, u*v3);
            gsl_matrix_set(designMatrix, i, 14, v4);
        }
        if(mDegree>=5) {
            LOG_ERROR("degree>=5 are not implemented yet");
            gsl_matrix_free(designMatrix);
            gsl_matrix_free(matCov);
            gsl_vector_free(values);
            gsl_vector_free(weights);
            gsl_vector_free(coeffs);
            return false;
        }
        
        gsl_vector_set (valueVector, i, f);
        gsl_vector_set (weightVector, i, neighborhood.getNeighborWeight(i));
    }
    double chisquare;
    gsl_matrix_const_view designMatrix_view = gsl_matrix_const_submatrix(designMatrix, 0,0, nofNeighbors, mNofCoeffs);
    gsl_vector_const_view values_view = gsl_vector_const_subvector(values, 0, nofNeighbors);
    gsl_vector_const_view weights_view = gsl_vector_const_subvector(weights, 0, nofNeighbors);

    gsl_multifit_linear_workspace *workspace = gsl_multifit_linear_alloc (nofNeighbors, mNofCoeffs);
    gsl_multifit_wlinear(
        &designMatrix_view.matrix,
        &weights_view.vector,
        &values_view.vector,
        mCoeffVector, mCovarianceMatrix, &chisquare, workspace);

    gsl_multifit_linear_free(workspace);
    
    gsl_matrix_free(mDesignMatrix);
    gsl_vector_free(mValueVector);
    gsl_vector_free(mWeightVector);
}

*/

}