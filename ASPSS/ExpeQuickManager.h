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



#ifndef _ExpeQuickManager_h_ 
#define _ExpeQuickManager_h_

#include "ExpePrerequisites.h"
#include "ExpeLogManager.h"
#include <QMetaClassInfo>
#include <QStringList>
#include "ExpeStringHelper.h"
#include <map>

namespace Expe
{


/** Base class to create custom factories suitable for the QuickManager system.
*/
class QuickFactory
{
public:
    QuickFactory(const QString& type, const QMetaObject* meta=0) : mType(type), mpMetaObject(meta) {}
    inline const QString& getType(void) const {return mType;}
    inline const QMetaObject* getMetaObject(void) const {return mpMetaObject;}
protected:
    QString mType;
    const QMetaObject* mpMetaObject;
};

/** Template class with template subclassing to easily create a factory creator for objects based on QObject.

    The type of the factory is set to the respective class name unless the attribute "Type" is specified with
    the Qt's class info system, i.e., using Q_CLASSINFO("Type","NameOfTheTypeOfMyFactory").
    (See the Qt documentation for more information about the QMetaObject system)
*/
template <class BaseFactoryT,class T> class QuickFactoryCreator : public BaseFactoryT
{
public:
    QuickFactoryCreator(void)
        : BaseFactoryT( QString(T::staticMetaObject.className()).split(":",QString::SkipEmptyParts).last(),
                        &T::staticMetaObject)
    {
        int typeElementId = T::staticMetaObject.indexOfClassInfo("Type");
        if (typeElementId>=0)
        {
            BaseFactoryT::mType = QString(T::staticMetaObject.classInfo(typeElementId).value());
        }
    }
    virtual ~QuickFactoryCreator() {}
};

/** 
*/
template <class ObjectT, class FactoryT> class QuickManager : public Singleton< QuickManager<ObjectT,FactoryT> >
{
protected:

    typedef std::map<QString,FactoryT*> FactoryMap;

public:

    QuickManager(void)
    {}
    
    ~QuickManager()
    {}

    void registerFactory(FactoryT* pFactory)
    {
        typename FactoryMap::iterator iter = mFactories.find(pFactory->getType());
        if (iter!=mFactories.end())
        {
            LOG_ERROR("Factory " << pFactory->getType() << " already registered");
        }
        mFactories[pFactory->getType()] = pFactory;
    }
    
    ObjectT* create(const QString& type)
    {
        typename FactoryMap::iterator iter = mFactories.find(type);
        if (iter==mFactories.end())
            return 0;
        return iter->second->create();
    }
    
    template <typename Arg0T> ObjectT* create(const QString& type, Arg0T arg0)
    {
        typename FactoryMap::iterator iter = mFactories.find(type);
        if (iter==mFactories.end())
            return 0;
        return iter->second->create(arg0);
    }
    
    QStringList getAvailableTypes(void) const
    {
        QStringList list;
        for (typename FactoryMap::const_iterator iter=mFactories.begin() ; iter!=mFactories.end() ; ++iter)
        {
            list.push_back(iter->first);
        }
        return list;
    }
    
    template <typename Arg0T> QStringList getAvailableTypes(Arg0T arg0) const
    {
        QStringList list;
        for (typename FactoryMap::const_iterator iter=mFactories.begin() ; iter!=mFactories.end() ; ++iter)
        {
            if (iter->second->match(arg0))
                list.push_back(iter->first);
        }
        return list;
    }
    
    const QMetaObject* getMetaObject(const QString& type) const
    {
        typename FactoryMap::const_iterator iter = mFactories.find(type);
        if (iter==mFactories.end())
            return 0;
        return iter->second->getMetaObject();
    }

protected:

    FactoryMap mFactories;
    
};


}

#endif

