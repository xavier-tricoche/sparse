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



#ifndef _ExpeStaticInitializer_h_
#define _ExpeStaticInitializer_h_

#include "ExpeCore.h"
#include "ExpeSingleton.h"

namespace Expe
{

class StaticFunction;

/** The goal of this framework is to allow to automatically run initialization code.

    Typical use:
    \code
    EXPE_INIT_CODE(MY_CLASS_NAME) {
        // code to initialize/register/etc my class
    }
    \endcode
    Next the code will be automatically executed by calling:
    \code
    StaticInitializer::InstanceSafe().processAll();
    \endcode
    Note that you usually don't have to call processAll() yourself,
    since this is automatically done if you create an Expe::Application instance (that is HIGHLY recommended !).
*/
class StaticInitializer : public Singleton<StaticInitializer>
{
public:

    StaticInitializer(void);
    
    void add(StaticFunction* sf);
    
    void processAll(void);
    
protected:

    std::vector<StaticFunction*> mPool;

};

#define EXPE_INIT_CODE(UNIQUE)  class UNIQUE ## _StaticInit : public StaticFunction { public: \
    UNIQUE ## _StaticInit(void) {StaticInitializer::InstanceSafe().add(this);} \
    virtual ~UNIQUE ## _StaticInit() {} \
    virtual void call(void); };  \
    static class UNIQUE ## _StaticInit UNIQUE ## staticCode;  \
    void UNIQUE ## _StaticInit::call(void)

class StaticFunction
{
public:
    virtual ~StaticFunction() {}
    virtual void call(void) = 0;
};

}

#endif
