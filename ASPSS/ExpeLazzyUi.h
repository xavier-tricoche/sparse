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



#ifndef _ExpeSmartObject_h_ 
#define _ExpeSmartObject_h_

#include "ExpeCore.h"

#include "ExpeSingleton.h"
#include <QObject>
#include <QObject>
#include <QVariant>
#include <map>
#include <QMap>

namespace Expe
{

class LazzyUi : public Singleton<LazzyUi>
{
public:

    /** PropertyName aims to store the name of a property.
        It is basically a simple string, but a new type is required by the PropertyOptions system to distinguish it from a common string. Indeed, a property option can be a property, i.e., the value of the option will be dynamically evaluated by querying the property of the given object.
    */
    class PropertyName
    {
    public:
        PropertyName() {}
        PropertyName(const ByteString& propName)
            : mPropName(propName)
        {}
        
        operator const char* (void)
        {
            return mPropName.data();
        }
        
    protected:
        ByteString mPropName;
    };

    class PropertyOptions : public QMap<ByteString,QVariant>
    {
    public:
    
        typedef QMapIterator<ByteString,QVariant> Iterator;

        inline PropertyOptions* _insert(const ByteString& key, const QVariant& value)
        {
            insert(key,value);
            return this;
        }

    protected:
    };


    /** \internal
        Basically, an UiDescription instance store the list of the class properties, each property being associated a list options via PropertyOptions.
        There can be only one UiDescription per class. Properties are automaticaly inherited.
    */
    class UiDescription
    {
    friend class LazzyUi;
    public:
    
        UiDescription()
            : mSuperClassName(""), mpSuper(0)
        {
        }
        
        ~UiDescription()
        {
        }
        
        void _setSuperClassName(const ByteString& superClassName) {mSuperClassName=superClassName;}
        
        inline const ByteString& getSuperClassName(void) const {return mSuperClassName;}
    
        inline UiDescription*  _insert(const ByteString& key, PropertyOptions* value)
        {
            mData.insert(std::pair<ByteString,PropertyOptions*>(key,value));
            return this;
        }
        
        const PropertyOptions* getPropertyOptions(const ByteString& name) const
        {
            Data::const_iterator iter = mData.find(name);
            if (iter!=mData.end())
                return iter->second;
            
            if (mpSuper)
                return mpSuper->getPropertyOptions(name);
            
            return 0;
        }
    protected:
    
        void setSuperUiDescription(const UiDescription* super) {mpSuper=super;}
    
    private:
        typedef std::map<ByteString,PropertyOptions*> Data;
        Data mData;
        ByteString mSuperClassName;
        const UiDescription* mpSuper;
    };
    
    
public:

    void registerUiDescription(const ByteString& className, UiDescription* uiDescription)
    {
//         std::cout << "LazzyUi::registerUiDescription " << className << "\n";
        // add the ui description into the map
        mData[className] = uiDescription;
        // update inheritence links
        // first try to find its own super
        const UiDescription* pSuper = editUiDescription(uiDescription->getSuperClassName());
        if (pSuper)
            uiDescription->setSuperUiDescription(pSuper);
        // second try to update the super of other
        for (Data::iterator iter = mData.begin() ; iter!=mData.end() ; ++iter)
        {
            if (iter->second->getSuperClassName()==className)
                iter->second->setSuperUiDescription(uiDescription);
        }
    }
    
    inline const UiDescription* getUiDescription(const ByteString& className) const
    {
        Data::const_iterator iter = mData.find(className);
        if (iter==mData.end())
            return 0;
        return iter->second;
    }
    
    UiDescription* editUiDescription(const ByteString& className)
    {
        Data::iterator iter = mData.find(className);
        if (iter==mData.end())
            return 0;
        return iter->second;
    }

    template <class T> class StaticRegister
    {
    public:
        StaticRegister(const ByteString& className, UiDescription* uiDescription)
        {
            // automatic superClassName from the Qt meta object system
            const QMetaObject* pSuperMeta = T::staticMetaObject.superClass();
            if (pSuperMeta)
                uiDescription->_setSuperClassName(pSuperMeta->className());
            
            LazzyUi::InstanceSafe().registerUiDescription(T::staticMetaObject.className(),uiDescription);
        }
    };

    protected:
        typedef std::map<ByteString,UiDescription*> Data;
        Data mData;
};



// static const LazzyUi::StaticRegister<CLASSNAME> CLASSNAME_lazzyuiregister(
//     (new LazzyUi::UiDescription())->_insert(....))

/** Declare the UI for the class CLASSNAME
    \code
        ELU_DECLARE(MyClass,
            ELU_PROPERTY("MyProperty1",
                ELU_OPT("category",ELU_STR("mycategory"))
                ELU_OPT("description",ELU_STR("here the description of the property"))
                ELU_OPT("min",0.)
                ELU_OPT("max",10.)
                ELU_OPT("step",0.01)
            )
        );
    \endcode
*/

#define ELU_DECLARE(CLASSNAME,PROPERTYCODE)   \
    static const LazzyUi::StaticRegister<CLASSNAME> CLASSNAME##_lazzyuiregister( # CLASSNAME, \
        (new LazzyUi::UiDescription())PROPERTYCODE)

/// Define a set of meta property meta fields for a  given property
#define ELU_PROPERTY(PROPERTYNAME,OPTIONCODE) ->_insert(PROPERTYNAME,(new LazzyUi::PropertyOptions())OPTIONCODE)

/// Adds a new meta property meta field
#define ELU_OPT(KEY,VALUE) ->_insert(KEY,QVariant::fromValue(VALUE))

/// Use it to define a field value as a string
#define ELU_STR(STR) (ByteString((STR)))

/// Use it to define a field value as a property call
#define ELU_PROPERTY_CALL(STR) (LazzyUi::PropertyName((STR)))

}

Q_DECLARE_METATYPE(Expe::LazzyUi::PropertyName);

#endif
