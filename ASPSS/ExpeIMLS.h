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



#ifndef _ExpeIMLS_h_
#define _ExpeIMLS_h_

#include "ExpeMlsSurface.h"

namespace Expe
{

class Neighborhood;
class WeightingFunction;

class IMLS : public MlsSurface
{
Q_OBJECT
Q_CLASSINFO("Type", "IMLS")
Q_CLASSINFO("Description", "\"Implicit Moving Least Squares\" as described in [Kolluri 2005] that is a simplification to point clouds of the method initialy proposed by Shen et al. [2004]. With this technique the approximate normals corresponds to the analytic ones.");

public:

    IMLS(ConstPointSetPtr pPoints);
    
    virtual ~IMLS();
    
    virtual void initSession(void);
    
    virtual bool project(Vector3& position, Vector3& normal, Color& color) const;
    
    virtual Real potentiel(const Vector3& position) const;
    
    virtual Vector3 gradient(const Vector3& position) const;
    
    virtual bool isValid(const Vector3& position) const;

protected:

    bool computePotentielAndGradient(const Vector3& position) const;

    mutable Vector3 mCachedPosition;
    mutable Vector3 mCachedGradient;
    mutable Real mCachedPotentiel;
    
};

}

#endif

 
