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



#ifndef _ExpeAlgebraicSphere_h_
#define _ExpeAlgebraicSphere_h_

#include "ExpePrerequisites.h"
#include "ExpeVector3.h"
#include "ExpeSphere.h"
#include "ExpePlane.h"

namespace Expe
{

enum AlgebraicSphereState {ASS_UNDETERMINED=0,ASS_PLANE=1,ASS_SPHERE=2};

/** Represent a sphere in its algebraic form.
*/

template <typename ScalarT>
class AlgebraicSphereT
{
public:
    typedef typename GetVector<3,ScalarT>::Type Vector3T;
    typedef Expe::SphereT<ScalarT> SphereT;
    typedef Expe::PlaneT<ScalarT> PlaneT;
public:

    AlgebraicSphereT();

    ~AlgebraicSphereT();

    /** the set of coefficients
    */
    ScalarT u[5];

    inline Vector3T& u13(void) {return reinterpret_cast<Vector3T&>(u[1]);}
    inline const Vector3T& u13(void) const {return reinterpret_cast<const Vector3T&>(u[1]);}

    void fromCenterRadius(const Vector3T& center, ScalarT radius);

    /** Normalizes the algebraic distance such that it is as close as possible
        to the Euclidean distance close to the surface of the sphere.
        In other words, this makes the length of the gradient to be 1
        AT the surface of the sphere.
    */
    void prattNormalize(void);

    /** Translates the sphere by the vector t.
    */
    inline void translate(const Vector3T& t);

    /** This function must be called before each edit session...
    */
    inline void beginEdit(void) { mState=ASS_UNDETERMINED; }

    /** This function must be called after each edit session before calling
        the other functions like state(), euclideanDistance(), project(), etc...
    */
    inline void endEdit(void);

    /** \warning the function endEdit() must be called before.
    */
    inline AlgebraicSphereState state(void) const {return mState;}

    /** \warning the function endEdit() must be called before.
    */
    inline const SphereT& asSphere(void) const;

    /** \warning the function endEdit() must be called before.
    */
    inline const PlaneT& asPlane(void) const;

    /** Computes the algebraic distance between a point x and the sphere surface.
    */
    inline ScalarT algebraicDistance(const Vector3T& x) const {
        return u[0] + u13().dot(x) + u[4] * x.squaredLength();
    }

    /** Computes the euclidean distance between a point x and the sphere surface.
        \warning the function endEdit() must be called before.
    */
    inline ScalarT euclideanDistance(const Vector3T& x) const;

    inline Vector3T gradient(const Vector3T& x) const {
        return u13()+2.*u[4]*x;
    }

    /** Projects a point onto the surface of the sphere.
        \warning the function endEdit() must be called before.
    */
    inline Vector3T project(const Vector3T& x) const;


    ScalarT iEuclideanDistance(const Vector3T& x) const;
    Vector3T iProject(const Vector3T& x) const;

public:

    inline SphereT& _asSphere(void);
    inline PlaneT& _asPlane(void);

    static_assert(sizeof(PlaneT)==sizeof(SphereT), "Size of Plane and Sphere types differ");
    byte mSphereAndPlaneHolder[sizeof(SphereT)];
    AlgebraicSphereState mState;

};

#include "ExpeAlgebraicSphere.inl"

//extern template class AlgebraicSphereT<float>;
//extern template class AlgebraicSphereT<double>;

typedef AlgebraicSphereT<Real> AlgebraicSphere;
typedef AlgebraicSphereT<float> AlgebraicSpheref;
typedef AlgebraicSphereT<double> AlgebraicSphered;

}

#endif
