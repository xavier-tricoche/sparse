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



#ifndef _ExpeGolubSphericalMlsSurface_h_
#define _ExpeGolubSphericalMlsSurface_h_

#include "ExpeSphericalMlsSurface.h"
#include "ExpeGolubSphereFitter.h"

namespace Expe
{

/** Normal free MLS surface definition based on spherical fit with the Pratt's constraint. This variant is recommended as a surface representation. Though the fitting does not need any normal information, this implementation is able to use them in order to provide a signed distance field that is mandatory for a lot of applications.
*/

class GolubSphericalMlsSurface : public SphericalMlsSurfaceWithoutNormalT<GolubSphereFitter>
{
Q_OBJECT
Q_CLASSINFO("Type", "APSS/Golub")
Q_CLASSINFO("Description", "Normal free MLS surface definition based on spherical fit with the Pratt's constraint. This variant is recommended as a surface representation. Though the fitting does not need any normal information, this implementation is able to use them in order to provide a signed distance field that is mandatory for a lot of applications.");

public:

    GolubSphericalMlsSurface(ConstPointSetPtr pPoints);
    
protected:

    /** Overloaded because we don't know how to analytically compute the MLS gradient.
    */
    virtual Vector3 evalMlsGradient(const Vector3& position);
};


}

#endif

 
