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



#ifndef _ExpeShared_h_ 
#define _ExpeShared_h_

#include "ExpePrerequisites.h"
#include "ExpeLogManager.h"

namespace Expe
{

class SharedObject
{
public:

    inline SharedObject()
        : mCountRef(1)
    {}
    
    inline ~SharedObject()
    {}
    
    inline void ref(void)
    {
        mCountRef++;
    }
    
    /** return true if the object must be deleted
    */
    inline bool deref(void)
    {
        return !(--mCountRef);
    }

private:

    int mCountRef;

};

template<class T> class SharedPointerBase
{
public:
    class Data : public SharedObject
    {
    public:
        inline Data(T* o)
            : mpObject(o)
        {}
        inline ~Data(void)
        {
            LOG_DEBUG(9, "auto delete " << uint64(mpObject) << " from " << uint64(this));
            delete mpObject;
        }
        inline void destroy(void)
        {
            delete mpObject;
            mpObject = 0;
        }
        inline T* get(void)
        {
            return mpObject;
        }
    protected:
        T* mpObject;
    };
    
    inline SharedPointerBase(void)
        : mpData(0)
    {}
    
    inline SharedPointerBase(T* o)
        : mpData(new Data(o))
    {}
    
    inline SharedPointerBase(const SharedPointerBase<T>& p)
    {
        mpData = p.mpData;
        ref();
    }
    
    template <typename TOTHER>
    inline SharedPointerBase(const SharedPointerBase<TOTHER>& p)
    {
        // check if the convertion is valid
        T* test = p._get();
        
        mpData = reinterpret_cast<Data*>(p._data());
        ref();
    }
    
    inline ~SharedPointerBase(void)
    {
        deref();
    }
    
    inline SharedPointerBase<T>& operator=(const SharedPointerBase<T>& p)
    {
        LOG_DEBUG(9," !! SharedPointerBase = SharedPointerBase");
        
        if (mpData != p.mpData)
        {
            deref();
            mpData = p.mpData;
            ref();
        }
        return *this;
    }
    
    template <typename TOTHER>
    inline SharedPointerBase<T>& operator=(const SharedPointerBase<TOTHER>& p)
    {
        LOG_DEBUG(9," !! SharedPointerBase = TOTHER");
        
        // check if the convertion is valid
        T* test = p._get();
        
        if (mpData != p.mpData)
        {
            deref();
            mpData = p.mpData;
            ref();
        }
        return *this;
    }

    inline SharedPointerBase<T>& operator=(T* o)
    {
        LOG_DEBUG(9," !! SharedPointerBase = O");
        
        if (mpData==0)
        {
            mpData = new Data(o);
        }
        else
        {
            deref();
            mpData = new Data(o);
        }
        
        return *this;
    }
    
    inline bool operator== (const SharedPointerBase<T>& p) const
    {
        return _get() == p._get();
    }

    inline bool operator!= (const SharedPointerBase<T>& p) const
    {
        return !( *this == p );
    }

    inline bool isNull(void) const
    {
        return !mpData || !mpData->_get();
    }
    
    inline T* _get(void) const
    {
        return (T*)(mpData?mpData->get():0);
    }
    
    inline Data* _data(void) const
    {
        return mpData;
    }
    
protected:

    
    void ref(void)
    {
        if (mpData)
            mpData->ref();
    }

    void deref(void)
    {
        if (mpData && mpData->deref())
            delete mpData;
    }
    
    Data* mpData;
};



template<class T> class SharedPointer : public SharedPointerBase<T>
{
public:
    inline SharedPointer(void)
    {}
    
    inline SharedPointer(T* o)
        : SharedPointerBase<T>(o)
    {}
    
    inline SharedPointer(const SharedPointer<T>& p)
        : SharedPointerBase<T>(p)
    {
    }
    
    template <typename TOTHER>
    inline SharedPointer(const SharedPointer<TOTHER>& p)
        : SharedPointerBase<T>(p)
    {}
    
    inline void destroy(void)
    {
        SharedPointerBase<T>::mpData->destroy();
    }
    
    inline SharedPointer<T>& operator=(const SharedPointer<T>& p)
    {
        LOG_DEBUG(9," !! SharedPointer = SharedPointer");
        
        if (SharedPointerBase<T>::mpData != p.mpData)
        {
            SharedPointerBase<T>::deref();
            SharedPointerBase<T>::mpData = p.mpData;
            SharedPointerBase<T>::ref();
        }
        return *this;
    }

//     inline SharedPointer<T>& operator=(T* o)
//     {
//         if (SharedPointerBase<T>::mpData==0)
//         {
//             SharedPointerBase<T>::mpData = new Data(o);
//         }
//         else
//         {
//             SharedPointerBase<T>::deref();
//             SharedPointerBase<T>::mpData = new Data(o);
//         }
//         
//         return *this;
//     }
//     
//     inline bool operator== (const SharedPointer<T>& p) const
//     {
//         return (T*)(*this) == (T*) p;
//     }
// 
//     inline bool operator!= (const SharedPointer<T>& p) const
//     {
//         return !( *this == p );
//     }

    inline T* operator->(void) const
    {
        assert(SharedPointerBase<T>::mpData);
        return SharedPointerBase<T>::mpData->get();
    }

    inline T& operator*(void) const
    {
        assert(SharedPointerBase<T>::mpData);
        return *(SharedPointerBase<T>::mpData->get());
    }

    inline operator T*(void) const
    {
        return SharedPointerBase<T>::_get();
    }
};


template<class T> class ConstSharedPointer : public SharedPointerBase<T>
{
public:
    inline ConstSharedPointer(void)
    {}
    
    inline ConstSharedPointer(const T* o)
        : SharedPointerBase<T>(const_cast<T*>(o))
    {}
    
    inline ConstSharedPointer(const ConstSharedPointer<T>& p)
        : SharedPointerBase<T>(p)
    {}
    
    inline ConstSharedPointer(const SharedPointer<T>& p)
        : SharedPointerBase<T>(p)
    {
        LOG_DEBUG(9," !! ConstSharedPointer construct from SharedPointer");
    }
    
//     inline ConstSharedPointer<T>& operator=(const ConstSharedPointer<T>& p)
//     {
//         if (mpData != p.mpData)
//         {
//             deref();
//             mpData = p.mpData;
//             ref();
//         }
//         return *this;
//     }
//     
    inline ConstSharedPointer<T>& operator=(const SharedPointer<T>& p)
    {
        LOG_DEBUG(9," !! ConstSharedPointer = SharedPointer");
        
        if (this->mpData != p.mpData)
        {
            this->deref();
            this->mpData = p._data();
            this->ref();
        }
        return *this;
    }

    inline ConstSharedPointer<T>& operator=(T* o)
    {
        LOG_DEBUG(9," !! ConstSharedPointer = O");
        
        if (this->mpData==0)
        {
            this->mpData = new typename SharedPointerBase<T>::Data(o);
        }
        else
        {
            this->deref();
            this->mpData = new typename SharedPointerBase<T>::Data(o);
        }
        
        return *this;
    }
//     
//     inline bool operator== (const SharedPointer<T>& p) const
//     {
//         return (T*)(*this) == (T*) p;
//     }
// 
//     inline bool operator!= (const SharedPointer<T>& p) const
//     {
//         return !( *this == p );
//     }

    

    inline const T* operator->(void) const
    {
        assert(SharedPointerBase<T>::mpData);
        return SharedPointerBase<T>::mpData->get();
    }

    inline const T& operator*(void) const
    {
        assert(SharedPointerBase<T>::mpData);
        return *(SharedPointerBase<T>::mpData->get());
    }

    inline operator const T*(void) const
    {
        return SharedPointerBase<T>::_get();
    }
};

#if 0
template<class T> class SharedPointer
{
public:
    inline SharedPointer(void)
        : mpData(0)
    {}
    
    inline SharedPointer(T* o)
        : mpData(new Data(o))
    {}
    
    inline SharedPointer(const SharedPointer<T>& p)
    {
        mpData = p.mpData;
        ref();
    }
    
    inline ~SharedPointer(void)
    {
        deref();
    }
    
    inline void destroy(void)
    {
        mpData->destroy();
    }
    
    inline SharedPointer<T>& operator=(const SharedPointer<T>& p)
    {
        if (mpData != p.mpData)
        {
            deref();
            mpData = p.mpData;
            ref();
        }
        return *this;
    }

    inline SharedPointer<T>& operator=(T* o)
    {
        if (mpData==0)
        {
            mpData = new Data(o);
        }
        else
        {
            deref();
            mpData = new Data(o);
        }
        
        return *this;
    }
    
    inline bool operator== (const SharedPointer<T>& p) const
    {
        return (T*)(*this) == (T*) p;
    }

    inline bool operator!= (const SharedPointer<T>& p) const
    {
        return !( *this == p );
    }

    inline bool isNull(void) const
    {
        return !mpData || !mpData->get();
    }

    inline T* operator->(void) const
    {
        assert(mpData);
        return mpData->get();
    }

    inline T& operator*(void) const
    {
        assert(mpData);
        return *(mpData->get());
    }

    inline operator T*(void) const
    {
        return (T*)(mpData?mpData->get():0);
    }
    
private:

    class Data : public SharedObject
    {
    public:
        inline Data(T* o)
            : mpObject(o)
        {}
        inline ~Data(void)
        {
            delete mpObject;
        }
        inline void destroy(void)
        {
            delete mpObject;
            mpObject = 0;
        }
        inline T* get(void)
        {
            return mpObject;
        }
    protected:
        T* mpObject;
    };
    
    void ref(void)
    {
        if (mpData)
            mpData->ref();
    }

    void deref(void)
    {
        if (mpData && mpData->deref())
            delete mpData;
    }
    
    Data* mpData;
};
#endif
}

#endif

