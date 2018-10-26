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



#ifndef _ExpeLogManager_h_ 
#define _ExpeLogManager_h_

#include <iostream>

#include "ExpePrerequisites.h"
#include "ExpeSingleton.h"

namespace Expe
{

#if defined(DEBUG) || defined(EXPE_DEBUG)

    #define LOG_DEBUG(LEVEL,MSG)    if (LEVEL<=Log::debugLevel()) Log::debug() \
        <<  "DEBUG in file " << __FILE__ << " (" << __LINE__ << ") : " << MSG << std::endl

    #define LOG_DEBUG_MSG(LEVEL,MSG) if (LEVEL<=Log::debugLevel()) Log::debug() \
        <<  "DEBUG in file " << MSG << "\n"
    
    #define LOG_WARNING(MSG)  Log::debug() \
        << "WARNING in file " << __FILE__ << " (" << __LINE__ << ") : " << MSG << std::endl
#else
    #define LOG_DEBUG(LEVEL,MSG)  
    #define LOG_DEBUG_MSG(LEVEL,MSG)  
    #define LOG_WARNING(MSG)  
#endif

#define LOG_ERROR(MSG)    Log::error() \
    << "ERROR in file " << __FILE__ << " (" << __LINE__ << ") : " << MSG << std::endl

#define LOG_MESSAGE(MSG)  Log::message() \
    << MSG << std::endl

class Log
{
public:

    inline static std::ostream& debug(void) {return *msDebug;}
    inline static std::ostream& error(void) {return *msError;}
    inline static std::ostream& message(void) {return *msMessage;}

    static void Initialize(int dbgLevel = 9);
    
    inline static int debugLevel(void) {return msDebugLevel;}
    
private:
    static bool msInitialized;
    static std::ostream* msDebug;
    static std::ostream* msError;
    static std::ostream* msMessage;
    static int msDebugLevel;
};

}

#endif

