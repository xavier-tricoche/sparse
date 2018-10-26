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



#ifndef _ExpeMatrix4_
#define _ExpeMatrix4_

#include "ExpePrerequisites.h"

#include "ExpeVector3.h"
#include "ExpeVector4.h"
#include "ExpeMatrix3.h"

namespace Expe
{
    /** Class encapsulating a standard 4x4 homogenous matrix.

        \par
            Internally, the matrix is stored as a vector of column vector in order to
            match OpenGL and to perform fast SSE matrix operations. However,
            the constructor and the access method \see get take arguments in
            the conventionnal row-column order.
        \par
            The generic form M * V which shows the layout of the matrix
            entries is shown below:
            <pre>
                / _m[0][0]  _m[1][0]  _m[2][0]  _m[3][0] \   /x\
                | _m[0][1]  _m[1][1]  _m[2][1]  _m[3][1] | * |y|
                | _m[0][2]  _m[1][2]  _m[2][2]  _m[3][2] |   |z|
                \ _m[0][3]  _m[1][3]  _m[2][3]  _m[3][3] /   \1/
            </pre>
        \par
            <pre>
                [ m[0]  m[4]  m[ 8]  m[12] ]
                | m[1]  m[5]  m[ 9]  m[13] |
                | m[2]  m[6]  m[10]  m[14] |
                [ m[3]  m[7]  m[11]  m[15] ]
            </pre>
    */
    class Matrix4
    {
    protected:
    public:
        /// The matrix entries, indexed by [row][col].
        union {
            #ifdef EXPE_USE_SSE
            v4sf cols[4];
            #endif
            Real _m[4][4];
            Real m[16];
        };
    public:
        /** Default constructor.
            @note
                It does <b>NOT</b> initialize the matrix for efficiency.
        */
        inline Matrix4()
        {
        }

        /** Here m_ij is such that i denote the row and j denote the column
        */
        inline Matrix4(
            Real m00, Real m01, Real m02, Real m03,
            Real m10, Real m11, Real m12, Real m13,
            Real m20, Real m21, Real m22, Real m23,
            Real m30, Real m31, Real m32, Real m33 )
        {
            m[0]  = m00; m[4]  = m01; m[8]  = m02; m[12] = m03;
            m[1]  = m10; m[5]  = m11; m[9]  = m12; m[13] = m13;
            m[2]  = m20; m[6]  = m21; m[10] = m22; m[14] = m23;
            m[3]  = m30; m[7]  = m31; m[11] = m32; m[15] = m33;
        }

        #ifdef EXPE_USE_SSE
        /** SSE matrix copy (faster ?)
        */
        inline Matrix4( const Matrix4& m2 )
        {
            cols[0] = m2.cols[0];
            cols[1] = m2.cols[1];
            cols[2] = m2.cols[2];
            cols[3] = m2.cols[3];
        }

        /** SSE matrix copy (faster ?)
        */
        inline void operator = ( const Matrix4& m2 )
        {
            cols[0] = m2.cols[0];
            cols[1] = m2.cols[1];
            cols[2] = m2.cols[2];
            cols[3] = m2.cols[3];
        }
        #endif

        #ifdef EXPE_USE_SSE
        inline const v4sf& getColumns(uint i) const
        {
            assert(i<4);
            return cols[i];
        }
        #endif

        inline Real* data(void)
        {
            return m;
        }

        inline const Real* data(void) const
        {
            return m;
        }

        /** i denote the row and j denote the column
        */
        inline Real get(uint i, uint j) const
        {
            return _m[j][i];
        }

        inline Real& get(uint i, uint j)
        {
            return _m[j][i];
        }
        inline Real operator()(uint i, uint j) const
        {
            return _m[j][i];
        }

        inline Real& operator()(uint i, uint j)
        {
            return _m[j][i];
        }

        /** Matrix-Matrix multiplication
        */
        inline Matrix4 operator * ( const Matrix4 &m2 ) const
        {
            Matrix4 r;
            // first column
            r._m[0][0] = _m[0][0]*m2._m[0][0] + _m[1][0]*m2._m[0][1] + _m[2][0]*m2._m[0][2] + _m[3][0]*m2._m[0][3];
            r._m[0][1] = _m[0][1]*m2._m[0][0] + _m[1][1]*m2._m[0][1] + _m[2][1]*m2._m[0][2] + _m[3][1]*m2._m[0][3];
            r._m[0][2] = _m[0][2]*m2._m[0][0] + _m[1][2]*m2._m[0][1] + _m[2][2]*m2._m[0][2] + _m[3][2]*m2._m[0][3];
            r._m[0][3] = _m[0][3]*m2._m[0][0] + _m[1][3]*m2._m[0][1] + _m[2][3]*m2._m[0][2] + _m[3][3]*m2._m[0][3];

            // second column
            r._m[1][0] = _m[0][0]*m2._m[1][0] + _m[1][0]*m2._m[1][1] + _m[2][0]*m2._m[1][2] + _m[3][0]*m2._m[1][3];
            r._m[1][1] = _m[0][1]*m2._m[1][0] + _m[1][1]*m2._m[1][1] + _m[2][1]*m2._m[1][2] + _m[3][1]*m2._m[1][3];
            r._m[1][2] = _m[0][2]*m2._m[1][0] + _m[1][2]*m2._m[1][1] + _m[2][2]*m2._m[1][2] + _m[3][2]*m2._m[1][3];
            r._m[1][3] = _m[0][3]*m2._m[1][0] + _m[1][3]*m2._m[1][1] + _m[2][3]*m2._m[1][2] + _m[3][3]*m2._m[1][3];

            // third column
            r._m[2][0] = _m[0][0]*m2._m[2][0] + _m[1][0]*m2._m[2][1] + _m[2][0]*m2._m[2][2] + _m[3][0]*m2._m[2][3];
            r._m[2][1] = _m[0][1]*m2._m[2][0] + _m[1][1]*m2._m[2][1] + _m[2][1]*m2._m[2][2] + _m[3][1]*m2._m[2][3];
            r._m[2][2] = _m[0][2]*m2._m[2][0] + _m[1][2]*m2._m[2][1] + _m[2][2]*m2._m[2][2] + _m[3][2]*m2._m[2][3];
            r._m[2][3] = _m[0][3]*m2._m[2][0] + _m[1][3]*m2._m[2][1] + _m[2][3]*m2._m[2][2] + _m[3][3]*m2._m[2][3];

            // 4th column
            r._m[3][0] = _m[0][0]*m2._m[3][0] + _m[1][0]*m2._m[3][1] + _m[2][0]*m2._m[3][2] + _m[3][0]*m2._m[3][3];
            r._m[3][1] = _m[0][1]*m2._m[3][0] + _m[1][1]*m2._m[3][1] + _m[2][1]*m2._m[3][2] + _m[3][1]*m2._m[3][3];
            r._m[3][2] = _m[0][2]*m2._m[3][0] + _m[1][2]*m2._m[3][1] + _m[2][2]*m2._m[3][2] + _m[3][2]*m2._m[3][3];
            r._m[3][3] = _m[0][3]*m2._m[3][0] + _m[1][3]*m2._m[3][1] + _m[2][3]*m2._m[3][2] + _m[3][3]*m2._m[3][3];

            return r;
        }

        /** Vector transformation using '*'.
            @remarks
                Transforms the given 3-D vector by the matrix, projecting the
                result back into <i>w</i> = 1.
            @note
                This means that the initial <i>w</i> is considered to be 1.0,
                and then all the tree elements of the resulting 3-D vector are
                divided by the resulting <i>w</i>.
        */
        inline Vector3 operator * ( const Vector3 &v ) const
        {
            Vector3 r;

            Real fInvW = 1.0 / ( _m[0][3] * v.x + _m[1][3] * v.y+ _m[2][3] * v.z + _m[3][3] );

            r.x = ( _m[0][0] * v.x + _m[1][0] * v.y + _m[2][0] * v.z + _m[3][0] ) * fInvW;
            r.y = ( _m[0][1] * v.x + _m[1][1] * v.y + _m[2][1] * v.z + _m[3][1] ) * fInvW;
            r.z = ( _m[0][2] * v.x + _m[1][2] * v.y + _m[2][2] * v.z + _m[3][2] ) * fInvW;

            return r;
        }

        /// \todo : bug SSE !
        inline Vector4 operator * ( const Vector4 &v ) const
        {
            Vector4 res, tmp1, tmp2;


//             #if (EXPE_USE_SSE==1)
//             asm (
//                 "movss  %[x], %[dst] \n\t"
//                 "movss  %[y], %[tmp1] \n\t"
//                 "shufps $0, %[dst], %[dst] \n\t"
//                 "shufps $0, %[tmp1], %[tmp1] \n\t"
//                 "mulps %[col0], %[dst] \n\t"
//                 "movss  %[z], %[tmp2] \n\t"
//                 "mulps %[col1], %[tmp1] \n\t"
//                 "shufps $0, %[tmp2], %[tmp2] \n\t"
//                 "addps %[tmp1], %[dst] \n\t"
//                 "mulps %[col2], %[tmp2] \n\t"
//                 "addps %[col3], %[dst] \n\t"
//                 "addps %[tmp2], %[dst] \n\t"
//                 : [dst] "=x,x" (res.vec)
//                 : [x] "x,m" (v.x),
//                 [y] "x,m" (v.y),
//                 [z] "x,m" (v.z),
//                 [w] "x,m" (v.w),
//                 [tmp1] "x,x" (tmp1.vec),
//                 [tmp2] "x,x" (tmp2.vec),
//                 [col0] "x,m" (cols[0]),
//                 [col1] "x,m" (cols[1]),
//                 [col2] "x,m" (cols[2]),
//                 [col3] "x,m" (cols[3])
//             );
//             #elif (EXPE_USE_SSE>=2)
//             asm(
//                 "pshufd $0, %[x], %[dst] \n\t"
//                 "pshufd $0, %[y], %[tmp1] \n\t"
//                 "mulps %[col0], %[dst] \n\t"
//                 "pshufd $0, %[z], %[tmp2] \n\t"
//                 "mulps %[col1], %[tmp1] \n\t"
//                 "mulps %[col2], %[tmp2] \n\t"
//                 "addps %[tmp1], %[dst] \n\t"
//                 "addps %[col3], %[tmp2] \n\t"
//                 "addps %[tmp2], %[dst] \n\t"
//                 : [dst] "=x,x" (res.vec)
//                 : [x] "x,m" (v.x),
//                 [y] "x,m" (v.y),
//                 [z] "x,m" (v.z),
//                 [tmp1] "x,x" (tmp1.vec),
//                 [tmp2] "x,x" (tmp2.vec),
//                 [col0] "x,m" (cols[0]),
//                 [col1] "x,m" (cols[1]),
//                 [col2] "x,m" (cols[2]),
//                 [col3] "x,m" (cols[3])
//             );
//             #else
                res.x = ( _m[0][0] * v.x + _m[1][0] * v.y + _m[2][0] * v.z + _m[3][0] ) ;
                res.y = ( _m[0][1] * v.x + _m[1][1] * v.y + _m[2][1] * v.z + _m[3][1] ) ;
                res.z = ( _m[0][2] * v.x + _m[1][2] * v.y + _m[2][2] * v.z + _m[3][2] ) ;
                res.w = ( _m[0][3] * v.x + _m[1][3] * v.y + _m[2][3] * v.z + _m[3][3] ) ;
//             #endif
            return res;
        }

        /** Matrix-Matrix addition.
        */
        inline Matrix4 operator + ( const Matrix4 &m2 ) const
        {
            Matrix4 r;

            #ifdef EXPE_USE_SSE
            r.cols[0] = __builtin_ia32_addps(cols[0], m2.cols[0]);
            r.cols[1] = __builtin_ia32_addps(cols[1], m2.cols[1]);
            r.cols[2] = __builtin_ia32_addps(cols[2], m2.cols[2]);
            r.cols[3] = __builtin_ia32_addps(cols[3], m2.cols[3]);
            /*asm (
                "movaps %[col00], %[res0] \n\t"
                "movaps %[col01], %[res1] \n\t"
                "movaps %[col02], %[res2] \n\t"
                "movaps %[col03], %[res3] \n\t"
                "addps  %[col10], %[res0] \n\t"
                "addps  %[col11], %[res1] \n\t"
                "addps  %[col12], %[res2] \n\t"
                "addps  %[col13], %[res3] \n\t"

                :   [res0] "x" (r.cols[0]),
                    [res1] "x" (r.cols[1]),
                    [res2] "x" (r.cols[2]),
                    [res3] "x" (r.cols[3]),

                :   [col00] "x,m" (cols[0]),
                    [col01] "x,m" (cols[1]),
                    [col02] "x,m" (cols[2]),
                    [col03] "x,m" (cols[3]),

                    [col10] "x,m" (m2.cols[0]),
                    [col11] "x,m" (m2.cols[1]),
                    [col12] "x,m" (m2.cols[2]),
                    [col13] "x,m" (m2.cols[3])
            );*/
            #else
            r._m[0][0] = _m[0][0] + m2._m[0][0];
            r._m[0][1] = _m[0][1] + m2._m[0][1];
            r._m[0][2] = _m[0][2] + m2._m[0][2];
            r._m[0][3] = _m[0][3] + m2._m[0][3];

            r._m[1][0] = _m[1][0] + m2._m[1][0];
            r._m[1][1] = _m[1][1] + m2._m[1][1];
            r._m[1][2] = _m[1][2] + m2._m[1][2];
            r._m[1][3] = _m[1][3] + m2._m[1][3];

            r._m[2][0] = _m[2][0] + m2._m[2][0];
            r._m[2][1] = _m[2][1] + m2._m[2][1];
            r._m[2][2] = _m[2][2] + m2._m[2][2];
            r._m[2][3] = _m[2][3] + m2._m[2][3];

            r._m[3][0] = _m[3][0] + m2._m[3][0];
            r._m[3][1] = _m[3][1] + m2._m[3][1];
            r._m[3][2] = _m[3][2] + m2._m[3][2];
            r._m[3][3] = _m[3][3] + m2._m[3][3];
            #endif
            return r;
        }

        /** Matrix-Matrix subtraction.
        */
        inline Matrix4 operator - ( const Matrix4 &m2 ) const
        {
            Matrix4 r;
            #ifdef EXPE_USE_SSE
            r.cols[0] = __builtin_ia32_subps(cols[0], m2.cols[0]);
            r.cols[1] = __builtin_ia32_subps(cols[1], m2.cols[1]);
            r.cols[2] = __builtin_ia32_subps(cols[2], m2.cols[2]);
            r.cols[3] = __builtin_ia32_subps(cols[3], m2.cols[3]);
            /*asm (
                "movaps %[col00], %[res0] \n\t"
                "movaps %[col01], %[res1] \n\t"
                "movaps %[col02], %[res2] \n\t"
                "movaps %[col03], %[res3] \n\t"
                "subps  %[col10], %[res0] \n\t"
                "subps  %[col11], %[res1] \n\t"
                "subps  %[col12], %[res2] \n\t"
                "subps  %[col13], %[res3] \n\t"

                :   [res0] "x" (r.cols[0]),
                    [res1] "x" (r.cols[1]),
                    [res2] "x" (r.cols[2]),
                    [res3] "x" (r.cols[3]),

                :   [col00] "x,m" (cols[0]),
                    [col01] "x,m" (cols[1]),
                    [col02] "x,m" (cols[2]),
                    [col03] "x,m" (cols[3]),

                    [col10] "x,m" (m2.cols[0]),
                    [col11] "x,m" (m2.cols[1]),
                    [col12] "x,m" (m2.cols[2]),
                    [col13] "x,m" (m2.cols[3])
            );*/
            #else
            r._m[0][0] = _m[0][0] - m2._m[0][0];
            r._m[0][1] = _m[0][1] - m2._m[0][1];
            r._m[0][2] = _m[0][2] - m2._m[0][2];
            r._m[0][3] = _m[0][3] - m2._m[0][3];

            r._m[1][0] = _m[1][0] - m2._m[1][0];
            r._m[1][1] = _m[1][1] - m2._m[1][1];
            r._m[1][2] = _m[1][2] - m2._m[1][2];
            r._m[1][3] = _m[1][3] - m2._m[1][3];

            r._m[2][0] = _m[2][0] - m2._m[2][0];
            r._m[2][1] = _m[2][1] - m2._m[2][1];
            r._m[2][2] = _m[2][2] - m2._m[2][2];
            r._m[2][3] = _m[2][3] - m2._m[2][3];

            r._m[3][0] = _m[3][0] - m2._m[3][0];
            r._m[3][1] = _m[3][1] - m2._m[3][1];
            r._m[3][2] = _m[3][2] - m2._m[3][2];
            r._m[3][3] = _m[3][3] - m2._m[3][3];
            #endif
            return r;
        }

        /** Tests 2 matrices for equality.
        */
        inline bool operator == ( const Matrix4& m2 ) const
        {
            if(
                _m[0][0]!=m2._m[0][0] || _m[0][1]!=m2._m[0][1] || _m[0][2]!=m2._m[0][2] || _m[0][3]!= m2._m[0][3] ||
                _m[1][0]!=m2._m[1][0] || _m[1][1]!=m2._m[1][1] || _m[1][2]!=m2._m[1][2] || _m[1][3]!= m2._m[1][3] ||
                _m[2][0]!=m2._m[2][0] || _m[2][1]!=m2._m[2][1] || _m[2][2]!=m2._m[2][2] || _m[2][3]!= m2._m[2][3] ||
                _m[3][0]!=m2._m[3][0] || _m[3][1]!=m2._m[3][1] || _m[3][2]!=m2._m[3][2] || _m[3][3]!= m2._m[3][3] )
                return false;
            return true;
        }

        /** Tests 2 matrices for inequality.
        */
        inline bool operator != ( Matrix4& m2 ) const
        {
            if(
                _m[0][0] != m2._m[0][0] || _m[0][1] != m2._m[0][1] || _m[0][2] != m2._m[0][2] || _m[0][3] != m2._m[0][3] ||
                _m[1][0] != m2._m[1][0] || _m[1][1] != m2._m[1][1] || _m[1][2] != m2._m[1][2] || _m[1][3] != m2._m[1][3] ||
                _m[2][0] != m2._m[2][0] || _m[2][1] != m2._m[2][1] || _m[2][2] != m2._m[2][2] || _m[2][3] != m2._m[2][3] ||
                _m[3][0] != m2._m[3][0] || _m[3][1] != m2._m[3][1] || _m[3][2] != m2._m[3][2] || _m[3][3] != m2._m[3][3] )
                return true;
            return false;
        }

        /** Assignment from 3x3 matrix.
        */
        inline void operator = ( const Matrix3& mat3 )
        {
            _m[0][0] = mat3.m[0][0]; _m[1][0] = mat3.m[0][1]; _m[2][0] = mat3.m[0][2];
            _m[0][1] = mat3.m[1][0]; _m[1][1] = mat3.m[1][1]; _m[2][1] = mat3.m[1][2];
            _m[0][2] = mat3.m[2][0]; _m[1][2] = mat3.m[2][1]; _m[2][2] = mat3.m[2][2];
        }

        inline Matrix4 transpose(void) const
        {
            return Matrix4(_m[0][0], _m[0][1], _m[0][2], _m[0][3],
                           _m[1][0], _m[1][1], _m[1][2], _m[1][3],
                           _m[2][0], _m[2][1], _m[2][2], _m[2][3],
                           _m[3][0], _m[3][1], _m[3][2], _m[3][3]);
        }

        /// \name Translation transformation
        //@{
        /** Sets the translation part of the matrix.
        */
        inline void setTranslation( const Vector3& v )
        {
            _m[3][0] = v.x;
            _m[3][1] = v.y;
            _m[3][2] = v.z;
        }

        /** Builds a translation matrix
        */
        inline void makeTranslation( const Vector3& v )
        {
            _m[0][0] = 1.0; _m[1][0] = 0.0; _m[2][0] = 0.0; _m[3][0] = v.x;
            _m[0][1] = 0.0; _m[1][1] = 1.0; _m[2][1] = 0.0; _m[3][1] = v.y;
            _m[0][2] = 0.0; _m[1][2] = 0.0; _m[2][2] = 1.0; _m[3][2] = v.z;
            _m[0][3] = 0.0; _m[1][3] = 0.0; _m[2][3] = 0.0; _m[3][3] = 1.0;
        }

        /** Gets the translation part.
        */
        inline Vector3 getTranslation(void) const
        {
            return Vector3(_m[3][0], _m[3][1], _m[3][2]);
        }
        //@}

        /// \name Scale transformation
        //@{
        /** Sets the scale part of the matrix.
        */
        inline void setScale( const Vector3& v )
        {
            _m[0][0] = v.x;
            _m[1][1] = v.y;
            _m[2][2] = v.z;
        }

        inline void scaleBy(const Vector3& s )
        {
            reinterpret_cast<Vector3&>(_m[0][0]) *= s.x;
            reinterpret_cast<Vector3&>(_m[0][1]) *= s.y;
            reinterpret_cast<Vector3&>(_m[0][2]) *= s.z;
        }

        /** Gets the scale of a scale matrix.
        */
        inline Vector3 getScale(void) const
        {
            return Vector3(
                reinterpret_cast<const Vector3&>(_m[0][0]).length(),
                reinterpret_cast<const Vector3&>(_m[1][0]).length(),
                reinterpret_cast<const Vector3&>(_m[2][0]).length());
        }
        //@}
        
        /// \name Rotation transformation
        //@{
        /** Specify the rotation part of the matrix
        */
        inline void setRotation(const Matrix3& r)
        {
            _m[0][0] = r[0][0]; _m[1][0] = r[0][1]; _m[2][0] = r[0][2];
            _m[0][1] = r[1][0]; _m[1][1] = r[1][1]; _m[2][1] = r[1][2];
            _m[0][2] = r[2][0]; _m[1][2] = r[2][1]; _m[2][2] = r[2][2];
        }

        /** Create a rotation matrix from a 3x3 matrix
        */
        inline void makeRotation(const Matrix3& r)
        {
            _m[0][0] = r[0][0];  _m[1][0] = r[0][1];  _m[2][0] = r[0][2];  _m[3][0] = 0.0;
            _m[0][1] = r[1][0];  _m[1][1] = r[1][1];  _m[2][1] = r[1][2];  _m[3][1] = 0.0;
            _m[0][2] = r[2][0];  _m[1][2] = r[2][1];  _m[2][2] = r[2][2];  _m[3][2] = 0.0;
            _m[0][3] = 0.0;      _m[1][3] = 0.0;      _m[2][3] = 0.0;      _m[3][3] = 1.0;
        }

        /** Extracts the rotation / scaling part of the Matrix as a 3x3 matrix.
        */
        inline Matrix3 extract3x3Matrix(void) const
        {
            Matrix3 m3x3;
            m3x3.m[0][0] = _m[0][0];
            m3x3.m[0][1] = _m[1][0];
            m3x3.m[0][2] = _m[2][0];
            m3x3.m[1][0] = _m[0][1];
            m3x3.m[1][1] = _m[1][1];
            m3x3.m[1][2] = _m[2][1];
            m3x3.m[2][0] = _m[0][2];
            m3x3.m[2][1] = _m[1][2];
            m3x3.m[2][2] = _m[2][2];
            return m3x3;

        }
        //@}

        static const Matrix4 ZERO;
        static const Matrix4 IDENTITY;

        inline Matrix4 operator*(Real scalar)
        {
            return Matrix4(
                scalar*_m[0][0], scalar*_m[1][0], scalar*_m[2][0], scalar*_m[3][0],
                scalar*_m[0][1], scalar*_m[1][1], scalar*_m[2][1], scalar*_m[3][1],
                scalar*_m[0][2], scalar*_m[1][2], scalar*_m[2][2], scalar*_m[3][2],
                scalar*_m[0][3], scalar*_m[1][3], scalar*_m[2][3], scalar*_m[3][3]);
        }

        /** Function for writing to a stream.
        */
        inline friend std::ostream& operator <<
            ( std::ostream& o, const Matrix4& m )
        {
            o << "Matrix4(";
            for (int i = 0; i < 4; ++i)
            {
                o << " row" << i << "{";
                for(int j = 0; j < 4; ++j)
                {
                    o << m.get(i,j) << " ";
                }
                o << "}";
            }
            o << ")";
            return o;
        }

        Matrix4 adjoint() const;
        Real determinant() const;
        Matrix4 inverse() const;

    };

    inline Vector4 mul(const Matrix4& mat, const Vector4& v)
    {
        Vector4 res, tmp1, tmp2;
        #if defined(EXPE_USE_SSE) && (EXPE_USE_SSE==1)
        asm (
            "movss  %[x], %[dst] \n\t"
            "movss  %[y], %[tmp1] \n\t"
            "shufps $0, %[dst], %[dst] \n\t"
            "shufps $0, %[tmp1], %[tmp1] \n\t"
            "mulps %[col0], %[dst] \n\t"
            "movss  %[z], %[tmp2] \n\t"
            "mulps %[col1], %[tmp1] \n\t"
            "shufps $0, %[tmp2], %[tmp2] \n\t"
            "addps %[tmp1], %[dst] \n\t"
            "mulps %[col2], %[tmp2] \n\t"
            "addps %[col3], %[dst] \n\t"
            "addps %[tmp2], %[dst] \n\t"
            : [dst] "=x,x" (res.vec)
            : [x] "x,m" (v.x),
              [y] "x,m" (v.y),
              [z] "x,m" (v.z),
              [w] "x,m" (v.w),
              [tmp1] "x,x" (tmp1.vec),
              [tmp2] "x,x" (tmp2.vec),
              [col0] "x,m" (mat.cols[0]),
              [col1] "x,m" (mat.cols[1]),
              [col2] "x,m" (mat.cols[2]),
              [col3] "x,m" (mat.cols[3])
        );
        #elif defined(EXPE_USE_SSE) && (EXPE_USE_SSE>=2)
        asm(
            "pshufd $0, %[x], %[dst] \n\t"
            "pshufd $0, %[y], %[tmp1] \n\t"
            "mulps %[col0], %[dst] \n\t"
            "pshufd $0, %[z], %[tmp2] \n\t"
            "mulps %[col1], %[tmp1] \n\t"
            "mulps %[col2], %[tmp2] \n\t"
            "addps %[tmp1], %[dst] \n\t"
            "addps %[col3], %[tmp2] \n\t"
            "addps %[tmp2], %[dst] \n\t"
            : [dst] "=x,x" (res.vec)
            : [x] "x,m" (v.x),
              [y] "x,m" (v.y),
              [z] "x,m" (v.z),
              [tmp1] "x,x" (tmp1.vec),
              [tmp2] "x,x" (tmp2.vec),
              [col0] "x,m" (mat.cols[0]),
              [col1] "x,m" (mat.cols[1]),
              [col2] "x,m" (mat.cols[2]),
              [col3] "x,m" (mat.cols[3])
        );
        #else
            res.x = ( mat.get(0,0) * v.x + mat.get(0,1) * v.y + mat.get(0,2) * v.z + mat.get(0,3) ) ;
            res.y = ( mat.get(1,0) * v.x + mat.get(1,1) * v.y + mat.get(1,2) * v.z + mat.get(1,3) ) ;
            res.z = ( mat.get(2,0) * v.x + mat.get(2,1) * v.y + mat.get(2,2) * v.z + mat.get(2,3) ) ;
            res.w = ( mat.get(3,0) * v.x + mat.get(3,1) * v.y + mat.get(3,2) * v.z + mat.get(3,3) ) ;
        #endif
        return res;
    }
}

#include <qmetatype.h>
Q_DECLARE_METATYPE(Expe::Matrix4);

#endif
