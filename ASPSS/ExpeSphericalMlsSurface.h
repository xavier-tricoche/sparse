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



#ifndef _ExpeSphericalMlsSurface_h_
#define _ExpeSphericalMlsSurface_h_

#include "ExpeLocalMlsApproximationSurface.h"
#include "ExpeAlgebraicSphere.h"

namespace Expe
{

/** Template class to easily build a MLS surface definition based on algebraic spherical fit.
    You basically only have to provide the fitter class.
    \param AlgebraicSphereFitterT the spherical fitter class
    This class assumes that the fitter is normal aware, then the default minimal number of samples to perform the fit is set to 3 and the algebraic fitted sphere is assumed to be correctly oriented.
    If it is not the case then the use the specialized class NormalFreeSphericalMlsSurfaceT instead.
*/
template <class AlgebraicSphereFitterT> class SphericalMlsSurfaceT : public LocalMlsApproximationSurface
{
public:

    SphericalMlsSurfaceT(ConstPointSetPtr pPoints) : LocalMlsApproximationSurface(pPoints) {}
    
    virtual ~SphericalMlsSurfaceT() {}

protected:

    virtual bool fit(const Vector3& position);

    virtual Real evalPotentiel(const Vector3& position);
    
    virtual Vector3 evalMlsGradient(const Vector3& position);
    
    virtual bool doProjection(Vector3& position, Vector3& normal, Real& delta);

public:

    AlgebraicSphereFitterT mAlgebraicSphere;

};

/** Template class to easily build a MLS surface definition based on algebraic spherical fit without normal.
    \param AlgebraicSphereFitterT the spherical fitter class
    The differences with the base template class SphericalMlsSurfaceT are the following:
    -If the input point clouds have normal information, then the fit operation automatically adjusts the sign of the sphere in order to provide a signed scalar field. Otherwise, the scalar field is not signed.
    -The default minimal number of samples to perform the fit is set to 5 here instead of 3.
*/
template <class AlgebraicSphereFitterT> class SphericalMlsSurfaceWithoutNormalT : public SphericalMlsSurfaceT<AlgebraicSphereFitterT>
{
public:

    SphericalMlsSurfaceWithoutNormalT(ConstPointSetPtr pPoints);
    
    virtual ~SphericalMlsSurfaceWithoutNormalT() {}

protected:

    /** Overloaded to adjust the orientation of the fitted sphere if the point set does provide normals.
    */
    virtual bool fit(const Vector3& position);
    
    /** Overloaded to adjust the orientation of the MLS gradient wrt the gradient of the sphere if the point set does not provide normals.
    */
    virtual Vector3 evalMlsGradient(const Vector3& position);
    
    /** Overloaded to remove the sign of the scalar field if the point set does not provide normals.
    */
    virtual Real evalPotentiel(const Vector3& position);

protected:

    bool mInputHasNormals;

};


#include "ExpeSphericalMlsSurface.inl"

}

#endif

