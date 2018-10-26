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



#include "ExpeVector3.h"
#include "ExpeStaticInitializer.h"
#include <QDataStream>

namespace Expe
{

const Vector3f Vector3f::ZERO( 0, 0, 0 );
const Vector3f Vector3f::UNIT_X( 1, 0, 0 );
const Vector3f Vector3f::UNIT_Y( 0, 1, 0 );
const Vector3f Vector3f::UNIT_Z( 0, 0, 1 );
const Vector3f Vector3f::UNIT_SCALE(1, 1, 1);

const Vector3d Vector3d::ZERO( 0, 0, 0 );
const Vector3d Vector3d::UNIT_X( 1, 0, 0 );
const Vector3d Vector3d::UNIT_Y( 0, 1, 0 );
const Vector3d Vector3d::UNIT_Z( 0, 0, 1 );
const Vector3d Vector3d::UNIT_SCALE(1, 1, 1);

const Vector3i Vector3i::ZERO( 0, 0, 0 );
const Vector3i Vector3i::UNIT_X( 1, 0, 0 );
const Vector3i Vector3i::UNIT_Y( 0, 1, 0 );
const Vector3i Vector3i::UNIT_Z( 0, 0, 1 );
const Vector3i Vector3i::UNIT_SCALE(1, 1, 1);


QDataStream& operator << (QDataStream& s, const Vector3& vec)
{
    s << QVariant(vec.x) << QVariant(vec.y) << QVariant(vec.z);
    return s;
}

QDataStream& operator >> (QDataStream& s, Vector3& vec)
{
    s >> vec.x; s >> vec.y; s >> vec.z;
    return s;
}

QDataStream& operator << (QDataStream& s, const Vector3i& vec)
{
    s << QVariant(vec.x) << QVariant(vec.y) << QVariant(vec.z);
    return s;
}

QDataStream& operator >> (QDataStream& s, Vector3i& vec)
{
    s >> vec.x; s >> vec.y; s >> vec.z;
    return s;
}

EXPE_INIT_CODE(Vector3) {
     qRegisterMetaType<Vector3f>("Vector3f");
     qRegisterMetaType<Vector3d>("Vector3d");
     qRegisterMetaType<Vector3i>("Vector3i");
     qRegisterMetaType<Vector3i64>("Vector3i64");
}

}
