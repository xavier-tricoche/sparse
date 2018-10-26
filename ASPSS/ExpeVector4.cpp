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



#include "ExpeVector4.h"
#include "ExpeStaticInitializer.h"

namespace Expe
{

const Vector4f Vector4f::ZERO( 0, 0, 0 );
const Vector4f Vector4f::UNIT_X( 1, 0, 0 );
const Vector4f Vector4f::UNIT_Y( 0, 1, 0 );
const Vector4f Vector4f::UNIT_Z( 0, 0, 1 );
const Vector4f Vector4f::UNIT_SCALE(1, 1, 1);

const Vector4d Vector4d::ZERO( 0, 0, 0 );
const Vector4d Vector4d::UNIT_X( 1, 0, 0 );
const Vector4d Vector4d::UNIT_Y( 0, 1, 0 );
const Vector4d Vector4d::UNIT_Z( 0, 0, 1 );
const Vector4d Vector4d::UNIT_SCALE(1, 1, 1);

const Vector4i Vector4i::ZERO( 0, 0, 0 );
const Vector4i Vector4i::UNIT_X( 1, 0, 0 );
const Vector4i Vector4i::UNIT_Y( 0, 1, 0 );
const Vector4i Vector4i::UNIT_Z( 0, 0, 1 );
const Vector4i Vector4i::UNIT_SCALE(1, 1, 1);

const Vector4i64 Vector4i64::ZERO( 0, 0, 0 );
const Vector4i64 Vector4i64::UNIT_X( 1, 0, 0 );
const Vector4i64 Vector4i64::UNIT_Y( 0, 1, 0 );
const Vector4i64 Vector4i64::UNIT_Z( 0, 0, 1 );
const Vector4i64 Vector4i64::UNIT_SCALE(1, 1, 1);

EXPE_INIT_CODE(Vector4) {
     qRegisterMetaType<Vector4f>("Vector4f");
     qRegisterMetaType<Vector4d>("Vector4d");
     qRegisterMetaType<Vector4i>("Vector4i");
     qRegisterMetaType<Vector4i64>("Vector4i64");
}

}
