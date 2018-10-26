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


#ifndef _ExpeMetaProgramming_h_
#define _ExpeMetaProgramming_h_

#include "ExpePrerequisites.h"
// #include "ExpeTypeInfo.h"

namespace Expe
{



namespace MTP
{


struct FalseType { enum { value = false }; };
struct TrueType { enum { value = true }; };


//--------------------------------------------------------------------------------
// IF
//--------------------------------------------------------------------------------
template <bool condition, class Then, class Else>
struct IF
{
    typedef Then RET;
};

template <class Then, class Else>
struct IF<false, Then, Else>
{
    typedef Else RET;
};
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
// type comparison
//--------------------------------------------------------------------------------
template <typename T1, typename T2> struct compare_type
{
    enum {result=false};
};

template <typename T> struct compare_type<T,T>
{
    enum {result=true};
};
//--------------------------------------------------------------------------------



}

}

#endif

