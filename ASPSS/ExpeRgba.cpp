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



#include "ExpeRgba.h"

namespace Expe
{

const Rgba Rgba::WHITE(0xffffffff);
const Rgba Rgba::BLACK(0x0);
const Rgba Rgba::RED(0xff,0x0,0x0);
const Rgba Rgba::GREEN(0x0,0xff,0x0);
const Rgba Rgba::BLUE(0x0,0x0,0xff);
const Rgba Rgba::GREY30(0x4C,0x4C,0x4C);
const Rgba Rgba::GREY50(0x7f,0x7f,0x7f);
const Rgba Rgba::GREY80(0xcc,0xcc,0xcc);

}

