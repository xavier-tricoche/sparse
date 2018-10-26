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



#ifndef _ExpeSimplePSS_h_
#define _ExpeSimplePSS_h_

#include "ExpeLocalMlsApproximationSurface.h"
#include "ExpeNormalConstrainedSphereFitter.h"

namespace Expe
{

/** 
*/

class SimplePSS : public LocalMlsApproximationSurface
{
//Q_OBJECT
//Q_CLASSINFO("Type", "SPSS")
//Q_CLASSINFO("Description", "Simple Point Set Surfaces is defined using a plane fit with normal averaging.");

public:

    SimplePSS(ConstPointSetPtr pPoints);
    
    virtual ~SimplePSS();
    

protected:

    virtual bool fit(const Vector3& position) ;

    virtual Real evalPotentiel(const Vector3& position);

    virtual Vector3 evalMlsGradient(const Vector3& position);

    virtual bool doProjection(Vector3& position, Vector3& normal, Real& delta);

protected:

    Vector3 mNormal;
    Real mOffset;

};

}

#endif

 
