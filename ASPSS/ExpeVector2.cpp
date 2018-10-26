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



#include "ExpeVector2.h"
#include "ExpeStaticInitializer.h"
#include <QDataStream>

namespace Expe
{

const Vector2 Vector2::ZERO( 0, 0 );
const Vector2 Vector2::UNIT_X( 1, 0 );
const Vector2 Vector2::UNIT_Y( 0, 1 );
const Vector2 Vector2::UNIT_SCALE(1, 1);

#ifndef EXPE_OLD_VECTOR2
const Vector2i Vector2i::ZERO( 0, 0 );
const Vector2i Vector2i::UNIT_X( 1, 0 );
const Vector2i Vector2i::UNIT_Y( 0, 1 );
const Vector2i Vector2i::UNIT_SCALE(1, 1);

QDataStream& operator << (QDataStream& s, const Vector2& vec)
{
    s << QVariant(vec.x) << QVariant(vec.y);
    return s;
}

QDataStream& operator >> (QDataStream& s, Vector2& vec)
{
    s >> vec.x; s >> vec.y;
    return s;
}

QDataStream& operator << (QDataStream& s, const Vector2i& vec)
{
    s << QVariant(vec.x) << QVariant(vec.y);
    return s;
}

QDataStream& operator >> (QDataStream& s, Vector2i& vec)
{
    s >> vec.x; s >> vec.y;
    return s;
}

EXPE_INIT_CODE(Vector2) {
     qRegisterMetaType<Vector2f>("Vector2f");
     qRegisterMetaType<Vector2d>("Vector2d");
     qRegisterMetaType<Vector2i>("Vector2i");
     qRegisterMetaType<Vector2i64>("Vector2i64");
}

#endif

}
