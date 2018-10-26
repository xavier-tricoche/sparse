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



#include "ExpeImplicitSurface.h"
#include "ExpeVector3.h"

namespace Expe
{

ImplicitSurface::ImplicitSurface(void)
{}

ImplicitSurface::~ImplicitSurface()
{}

void ImplicitSurface::flushStatistics(void)
{
}

Real ImplicitSurface::potentiel(const Vector3& position) const
{
    return 1e99;
}

Vector3 ImplicitSurface::gradient(const Vector3& position) const
{
    return numericalGradient(position,0.02);
}

Vector3 ImplicitSurface::numericalGradient(const Vector3& position, Real delta) const
{
    Vector3 V1, V2;
    Vector3 dp;
    
    V1.x=position.x+delta;
    V1.y=position.y;
    V1.z=position.z;
    V2.x=position.x-delta;
    V2.y=position.y;
    V2.z=position.z;
    dp.x = (this->potentiel(V1)-this->potentiel(V2))/(2.*delta);
    
    V1.x=position.x;
    V1.y=position.y+delta;
    V1.z=position.z;
    V2.x=position.x;
    V2.y=position.y-delta;
    V2.z=position.z;
    dp.y = (this->potentiel(V1)-this->potentiel(V2))/(2.*delta);

    V1.x=position.x;
    V1.y=position.y;
    V1.z=position.z+delta;
    V2.x=position.x;
    V2.y=position.y;
    V2.z=position.z-delta;
    dp.z = (this->potentiel(V1)-this->potentiel(V2))/(2.*delta);

    return dp;
}

bool ImplicitSurface::isValid(const Vector3& position) const
{
    return true;
}

}

//#include "ExpeImplicitSurface.moc"


