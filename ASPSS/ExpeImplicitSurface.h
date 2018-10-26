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



#ifndef _ExpeImplicitSurface_h_
#define _ExpeImplicitSurface_h_

#include "ExpeCore.h"
#include "ExpeTypedObject.h"

namespace Expe
{

/** Abstract class for implicit surfaces.
    This class also supports surface definitions based on a projection operator.
*/

class ImplicitSurface : public TypedObject
{
//Q_OBJECT

public:

    ImplicitSurface(void);
    
    virtual ~ImplicitSurface();
    
    /** Must be called before starting to evaluate the surface.
        Must be re-called after changing the surface parameters.
    */
    virtual void initSession(void) {}
    
    /** Projects a point onto the surface.
        \param position position of the point to project (in/out)
        \param normal estimated normal of the surface (out)
        \param color color of the surface (out)
        \return false if the projection failed.
        Should be overloaded.
        \todo implement a default projection operator that would follows the gradient field.
    */
    virtual bool project(Vector3& position, Vector3& normal, Color& color) const {return false;}
    
    /** Return scalar field value for the point \param position.
        Should be close to the distance field.
        Should be overloaded. By default the scalar field value is inferred from the projection operator.
    */
    virtual Real potentiel(const Vector3& position) const;
    
    /** Returns the gradient vector for the point \param position.
        Should be overloaded.
        By default, the gradient is computed numerically using numericalGradient.
    */
    virtual Vector3 gradient(const Vector3& position) const;
    
    /** Returns true if the point position is potentially on the surface.
        Should be overloaded.
        By default, returns true.
    */
    virtual bool isValid(const Vector3& position) const;
    
    /** Prints some statistics.
    */
    virtual void flushStatistics(void);
    
    /** Returns the gradient vector for the point \param position.
        The gradient is computed numerically by evaluating the scalar field at neighbor positions.
        \param position the evaluation point
        \param delta the distance to neigbor positions
    */
    Vector3 numericalGradient(const Vector3& position, Real delta) const;

protected:

};

}

#endif

