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



#ifndef _ExpeSingleton_h_ 
#define _ExpeSingleton_h_

#include "ExpePrerequisites.h"
#ifdef EXPE_DEBUG
#include <iostream>
#endif

namespace Expe
{

template <class T> class Singleton
{
public:

    Singleton()
    {
        ArithPtr offset = (ArithPtr)(T*)1 - (ArithPtr)(Singleton <T>*)(T*)1;
        T::mspSingleton = (T*)((ArithPtr)this + offset);
        #ifdef EXPE_DEBUG
            std::cerr << "Singleton " << typeid(T).name() << " created." << std::endl;
        #endif
    }
    
    ~Singleton()
    {
        T::mspSingleton = 0;
    }
    
    inline static T& Instance(void)
    {
        return *T::mspSingleton;
    }
    
    inline static T* InstancePtr(void)
    {
        return T::mspSingleton;
    }

    static void Create(bool replace = false)
    {
        if(T::mspSingleton==0 || replace)
        {
            new T();
        }
    }
  
    inline static T& InstanceSafe(void)
    {
        if(T::mspSingleton==0)
        {
            new T();
        }
        return *T::mspSingleton;
    }


private:

  static T* mspSingleton;

};


/** Macro instancing the implementation details for a singleton class.
*/
#define EXPE_SINGLETON_IMPLEMENTATION(CLASSNAME)  \
    template<>  CLASSNAME* Singleton<CLASSNAME>::mspSingleton = 0;


/** Macro instancing the implementation details for a singleton class
    with automatic creation at runtime.
*/
#define EXPE_SINGLETON_IMPLEMENTATION_AUTO(CLASSNAME)  \
    template<>  CLASSNAME* Singleton<CLASSNAME>::mspSingleton = 0; \
    EXPE_INIT_CODE(CLASSNAME) { CLASSNAME::Create(); }

}

#endif

