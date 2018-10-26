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



namespace Expe
{

template <typename ScalarT>
inline ScalarT AlgebraicSphereT<ScalarT>::iEuclideanDistance(const Vector3T& x) const
{
    ScalarT d = 0.;
    Vector3T grad;
    Vector3T dir = u13()+2.*u[4]*x;
    ScalarT ilg = 1./dir.length();
    dir *= ilg;
    ScalarT ad = u[0] + u13().dot(x) + u[4] * x.squaredLength();
    ScalarT delta = -ad*Math::Min<ScalarT>(ilg,1.);
    Vector3T p = x + dir*delta;
    d += delta;
    for (int i=0 ; i<5 ; ++i)
    {
        grad = u13()+2.*u[4]*p;
        ilg = 1./grad.length();
        delta = -(u[0] + u13().dot(p) + u[4] * p.squaredLength())*Math::Min<ScalarT>(ilg,1.);
        p += dir*delta;
        d += delta;
    }
    return -d;
}

template <typename ScalarT>
inline typename AlgebraicSphereT<ScalarT>::Vector3T AlgebraicSphereT<ScalarT>::iProject(const Vector3T& x) const
{
    Vector3T grad;
    Vector3T dir = u13()+2.*u[4]*x;
    ScalarT ilg = 1./dir.length();
    dir *= ilg;
    ScalarT ad = u[0] + u13().dot(x) + u[4] * x.squaredLength();
    ScalarT delta = -ad*Math::Min<ScalarT>(ilg,1.);
    Vector3T p = x + dir*delta;
    for (int i=0 ; i<5 ; ++i)
    {
        grad = u13()+2.*u[4]*p;
        ilg = 1./grad.length();
        delta = -(u[0] + u13().dot(p) + u[4] * p.squaredLength())*Math::Min<ScalarT>(ilg,1.);
        p += dir*delta;
    }
    return p;
}


}
