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



#include "ExpeLogManager.h"
#include <iostream>

namespace Expe
{

// TODO make le log streams customizable
// For instance, using a config file
void Log::Initialize(int dbgLevel)
{
    msDebug = &std::cerr;
    msError = &std::cerr;
    msMessage = &std::cout;
    msDebugLevel = dbgLevel;
    
    msInitialized = true;
}

bool Log::msInitialized = false;
std::ostream* Log::msDebug = 0;
std::ostream* Log::msError = 0;
std::ostream* Log::msMessage = 0;
int Log::msDebugLevel = 9;

}
 
