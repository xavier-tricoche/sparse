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



#ifndef _ExpeMatrix3_h_
#define _ExpeMatrix3_h_

#include "ExpePrerequisites.h"

#include "ExpeVector3.h"

// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.magic-software.com

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

namespace Expe
{
    /** A 3x3 matrix which can represent rotations around axes.
        @note
            <b>All the code is adapted from the Wild Magic 0.2 Matrix
            library (http://www.magic-software.com).
        @par
            The coordinate system is assumed to be <b>right-handed</b>.
    */
    class Matrix3
    {
    public:
        // construction
        Matrix3 ();
        Matrix3 (const Real arr[3][3]);
        Matrix3 (const Matrix3& rkMatrix);
        Matrix3 (Real fEntry00, Real fEntry01, Real fEntry02,
                    Real fEntry10, Real fEntry11, Real fEntry12,
                    Real fEntry20, Real fEntry21, Real fEntry22);
                    
        inline Real* data(void)
        {
            return m[0];
        }
        
        inline const Real* data(void) const
        {
            return m[0];
        }

        // member access, allows use of construct mat[r][c]
        Real* operator[] (int iRow) const;
        operator Real* ();
        Vector3 getColumn (int iCol) const;
        void setColumn(int iCol, const Vector3& vec);
        void fromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

        // assignment and comparison
        Matrix3& operator= (const Matrix3& rkMatrix);
        bool operator== (const Matrix3& rkMatrix) const;
        bool operator!= (const Matrix3& rkMatrix) const;

        // arithmetic operations
        Matrix3 operator+ (const Matrix3& rkMatrix) const;
        Matrix3 operator- (const Matrix3& rkMatrix) const;
        Matrix3 operator* (const Matrix3& rkMatrix) const;
        Matrix3 operator- () const;

        // matrix * vector [3x3 * 3x1 = 3x1]
        Vector3 operator* (const Vector3& rkVector) const;

        // vector * matrix [1x3 * 3x3 = 1x3]
        friend Vector3 operator* (const Vector3& rkVector,
            const Matrix3& rkMatrix);

        // matrix * scalar
        Matrix3 operator* (Real fScalar) const;

        // scalar * matrix
        friend Matrix3 operator* (Real fScalar, const Matrix3& rkMatrix);

        // utilities
        Matrix3 transpose () const;
        bool inverse (Matrix3& rkInverse, Real fTolerance = 1e-06) const;
        Matrix3 inverse (Real fTolerance = 1e-06) const;
        Real determinant () const;

        // singular value decomposition
        void singularValueDecomposition (Matrix3& rkL, Vector3& rkS,
            Matrix3& rkR) const;
        void singularValueComposition (const Matrix3& rkL,
            const Vector3& rkS, const Matrix3& rkR);

        // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
        void orthonormalize ();

        // orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
        void QDUDecomposition (Matrix3& rkQ, Vector3& rkD,
            Vector3& rkU) const;

        Real spectralNorm () const;

        // matrix must be orthonormal
        void toAxisAngle (Vector3& rkAxis, Real& rfRadians) const;
        void fromAxisAngle (const Vector3& rkAxis, Real fRadians);

        // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
        // where yaw is rotation about the Up vector, pitch is rotation about the
        // Right axis, and roll is rotation about the Direction axis.
        bool toEulerAnglesXYZ (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
        bool toEulerAnglesXZY (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
        bool toEulerAnglesYXZ (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
        bool toEulerAnglesYZX (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
        bool toEulerAnglesZXY (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
        bool toEulerAnglesZYX (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
        void fromEulerAnglesXYZ (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesXZY (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesYXZ (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesYZX (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesZXY (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesZYX (float fYAngle, float fPAngle, float fRAngle);

        // eigensolver, matrix must be symmetric
        void eigenSolveSymmetric (Real afEigenvalue[3],
            Vector3 akEigenvector[3]) const;

        static void TensorProduct (const Vector3& rkU, const Vector3& rkV,
            Matrix3& rkProduct);

        static const Real EPSILON;
        static const Matrix3 ZERO;
        static const Matrix3 IDENTITY;

    protected:
        // support for eigensolver
        void tridiagonal (Real afDiag[3], Real afSubDiag[3]);
        bool QLAlgorithm (Real afDiag[3], Real afSubDiag[3]);

        // support for singular value decomposition
        static const Real ms_fSvdEpsilon;
        static const int ms_iSvdMaxIterations;
        static void bidiagonalize (Matrix3& kA, Matrix3& kL,
            Matrix3& kR);
        static void golubKahanStep (Matrix3& kA, Matrix3& kL,
            Matrix3& kR);

        // support for spectral norm
        static Real MaxCubicRoot (Real afCoeff[3]);

        Real m[3][3];

        // for faster access
        friend class Matrix4;
    };
}

#include <qmetatype.h>
Q_DECLARE_METATYPE(Expe::Matrix3);

#endif
