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



#include "ExpePointSet.h"

namespace Expe
{


#define BUILTIN_ATTRIBUTE_INSTANCE(NAME)   \
    /*template<>*/ _PointSetBuiltinData::_MetaAttribute_ ## NAME ## _   _PointSetBuiltinData::Attribute_ ## NAME (#NAME)

BUILTIN_ATTRIBUTE_INSTANCE(position);
BUILTIN_ATTRIBUTE_INSTANCE(radius);
BUILTIN_ATTRIBUTE_INSTANCE(normal);
BUILTIN_ATTRIBUTE_INSTANCE(color);
BUILTIN_ATTRIBUTE_INSTANCE(texcoord);
BUILTIN_ATTRIBUTE_INSTANCE(materialId);
BUILTIN_ATTRIBUTE_INSTANCE(flags);
BUILTIN_ATTRIBUTE_INSTANCE(scalarradius);
BUILTIN_ATTRIBUTE_INSTANCE(scalar);
BUILTIN_ATTRIBUTE_INSTANCE(siteid);

const PointSet::AttributeList PointSet::DefaultFormat = PointSet::Attribute_position
        | PointSet::Attribute_radius
        | PointSet::Attribute_normal
        | PointSet::Attribute_color;

PointSet* PointSet::clone(void) const
{
    // FIXME : use memcpy !
    
    PointSet* clonePoints = new PointSet(this->getFormat().getAttributes());
    clonePoints->reserve(this->size());
    for(PointSet::const_iterator point_it = this->begin() ; point_it!=this->end() ; ++point_it)
    {
        clonePoints->append().copyFrom(*point_it);
    }
//     clonePoints->updateVBO();
    return clonePoints;
}

AxisAlignedBox PointSet::computeAABB(void) const
{
    AxisAlignedBox aabb;
    for(PointSet::const_iterator point_iter = this->begin() ; point_iter!=this->end() ; ++point_iter)
    {
        aabb.extend(point_iter->position());
    }
    return aabb;
}

}

//#include "ExpePointSet.moc"
