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



#ifndef _ExpeRgba_h_
#define _ExpeRgba_h_

#include "ExpePrerequisites.h"

namespace Expe
{

struct Rgba
{
    union
    {
        uint asInt;
        struct {
            ubyte r, g, b, a;
        };
    };
    
    inline Rgba() {}
    inline Rgba(uint rgba)
    {
        asInt = rgba;
    }
    inline Rgba(ubyte _r, ubyte _g, ubyte _b, ubyte _a = 255)
    {
        r = _r; g = _g; b = _b; a = _a;
    }
    inline operator uint () const {return asInt;}
    inline const Rgba& operator = (uint rgba)
    {
        asInt = rgba;
        return *this;
    }

    static const Rgba WHITE;
    static const Rgba BLACK;
    static const Rgba RED;
    static const Rgba GREEN;
    static const Rgba BLUE;
    static const Rgba GREY30;
    static const Rgba GREY50;
    static const Rgba GREY80;
};

template <>
struct TypeInfo<Rgba>
{
    // FIXME Id
    enum {NofComponents=4};
    typedef ubyte       ComponentType;
    typedef Rgba        IntegerType;
};

}

#include <qmetatype.h>
Q_DECLARE_METATYPE(Expe::Rgba);

#endif
 
