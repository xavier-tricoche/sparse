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



template <typename ScalarT>
AlgebraicSphereT<ScalarT>::AlgebraicSphereT()
    : mState(ASS_UNDETERMINED)
{
}

template <typename ScalarT>
AlgebraicSphereT<ScalarT>::~AlgebraicSphereT()
{
}

template <typename ScalarT>
void AlgebraicSphereT<ScalarT>::fromCenterRadius(const Vector3T& center, ScalarT radius)
{
    u13() = -2.*center;
    u[0] = center.squaredLength() - radius*radius;
    u[4] = 1.;
}

template <typename ScalarT>
void AlgebraicSphereT<ScalarT>::prattNormalize(void)
{
    ScalarT f = 1./sqrt(u13().squaredLength() - 4.*u[0]*u[4]);
    for (uint k=0 ; k<5 ; ++k)
        u[k] *= f;
}

template <typename ScalarT>
inline void AlgebraicSphereT<ScalarT>::translate(const Vector3T& t)
{
    u[0] = u[0] - u13().dot(t) + u[4] * t.squaredLength();
    u13() = u13() - 2.*u[4]*t;
}

template <typename ScalarT>
void AlgebraicSphereT<ScalarT>::endEdit(void)
{
    if (Math::Abs(u[4])>1e-9)
    {
        mState = ASS_SPHERE;
        ScalarT b = 1./u[4];
        _asSphere().setCenter(-0.5*b*u13());
        _asSphere().setRadius( Math::Sqrt( asSphere().center().squaredLength() - b*u[0] ) );
    }
    else if (u[4]==0.)
    {
        mState = ASS_PLANE;
        ScalarT s = u13().invLength();
        _asPlane().setNormal(u13()*s);
        _asPlane().setOffset(u[0]*s);
    }
    else
    {
        mState = ASS_UNDETERMINED;
    }
}


// template <typename ScalarT>
// AlgebraicSphereT<ScalarT>::AlgebraicSphereState AlgebraicSphereT<ScalarT>::state(void) const
// {
//     return mState;
// }

template <typename ScalarT>
const typename AlgebraicSphereT<ScalarT>::SphereT& AlgebraicSphereT<ScalarT>::asSphere(void) const
{
    return *reinterpret_cast<const SphereT*>(mSphereAndPlaneHolder);
}

template <typename ScalarT>
typename AlgebraicSphereT<ScalarT>::SphereT& AlgebraicSphereT<ScalarT>::_asSphere(void)
{
    return *reinterpret_cast<SphereT*>(mSphereAndPlaneHolder);
}

template <typename ScalarT>
const typename AlgebraicSphereT<ScalarT>::PlaneT& AlgebraicSphereT<ScalarT>::asPlane(void) const
{
    return *reinterpret_cast<const PlaneT*>(mSphereAndPlaneHolder);
}

template <typename ScalarT>
typename AlgebraicSphereT<ScalarT>::PlaneT& AlgebraicSphereT<ScalarT>::_asPlane(void)
{
    return *reinterpret_cast<PlaneT*>(mSphereAndPlaneHolder);
}

template <typename ScalarT>
inline ScalarT AlgebraicSphereT<ScalarT>::euclideanDistance(const Vector3T& x) const
{
    if (mState==ASS_SPHERE)
    {
        ScalarT aux = asSphere().distanceTo(x);
        if (u[4]<0.)
            aux = -aux;
        return aux;
    }
    
    if (mState==ASS_PLANE)
        return asPlane().distanceTo(x);
    
    // else, tedious case, fall back to an iterative method:
    return iEuclideanDistance(x);
}

template <typename ScalarT>
inline typename AlgebraicSphereT<ScalarT>::Vector3T AlgebraicSphereT<ScalarT>::project(const Vector3T& x) const
{
    if (mState==ASS_SPHERE)
        return asSphere().project(x);
    
    if (mState==ASS_PLANE)
        return asPlane().project(x);

    // else, tedious case, fall back to an iterative method
    return iProject(x);
}
