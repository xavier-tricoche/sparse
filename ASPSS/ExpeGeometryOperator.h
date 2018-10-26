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



#ifndef _ExpeGeometryOperator_h_
#define _ExpeGeometryOperator_h_

#include "ExpeGeometryObject.h"
#include "ExpeQuickManager.h"
#include <QObject>
#include <map>

namespace Expe
{

class GeometryOperator : public QObject
{
//Q_OBJECT

public:

  //  Q_PROPERTY(bool InputAsOutput READ isInputAsOutput WRITE setInputAsOutput DESIGNABLE true STORED true);

    struct InputDescritption
    {
        //String semantique;
        //String description;
        bool isRequired;
    };

    GeometryOperator(GeometryObject* pInput);
    
    virtual ~GeometryOperator(void);
    
    const QString& getName(void) const {return mName;}

    virtual bool apply(void) = 0;
    
    GeometryObject* getResult(void) {if (mInputAsOutput) return mpInput; else return mpOutput;}
    
    bool isInputAsOutputDoable(void) const {return mIsInputAsOutputDoable;}
    void setInputAsOutput(bool on) {if (mIsInputAsOutputDoable) mInputAsOutput=on;}
    bool isInputAsOutput(void) const {return mInputAsOutput;}

protected:

    QString mName;

    GeometryObject* mpInput;
    GeometryObject* mpOutput;

    bool mIsInputAsOutputDoable;
    bool mInputAsOutput;
};

//--------------------------------------------------------------------------------
/** Management
*/
/*
class GeometryOperatorFactory : public QuickFactory
{
public:
    GeometryOperatorFactory(const QString& type, const QMetaObject* meta=0) : QuickFactory(type, meta) {}
    virtual ~GeometryOperatorFactory() {}
    virtual GeometryOperator* create(GeometryObject* pInput) = 0;
    virtual bool match(const GeometryObject* pInput) = 0;
};

template <class T> class GeometryOperatorFactoryCreator : public QuickFactoryCreator<GeometryOperatorFactory,T>
{
public:
    virtual ~GeometryOperatorFactoryCreator() {}
    
    virtual GeometryOperator* create(GeometryObject* pInput)
    {
        return new T(pInput);
    }
};*/

// template <class T> class GeometryOperatorFactoryCreator : public GeometryOperatorFactory
// {
// public:
//     GeometryOperatorFactoryCreator(void) : GeometryOperatorFactory(T::staticMetaObject.className(), &T::staticMetaObject)
//     {
//         int typeElementId = T::staticMetaObject.indexOfClassInfo("Type");
//         if (typeElementId>=0)
//             mType = String(T::staticMetaObject.classInfo(typeElementId).value());
//     }
//     virtual ~GeometryOperatorFactoryCreator() {}
//     
//     virtual GeometryOperator* create(Geometry* pInput)
//     {
//         return new T(dynamic_cast<Mesh*>(pInput));
//     }
// };
/*
template <class T,class T_Match1> class GeometryOperatorFactoryCreatorMatch1 : public GeometryOperatorFactoryCreator<T>
{
public:
    virtual bool match(const GeometryObject* pInput)
    {
        return (dynamic_cast<const T_Match1*>(pInput)!=0);
    }
};

template <class T,class T_Match1,class T_Match2> class GeometryOperatorFactoryCreatorMatch2 : public GeometryOperatorFactoryCreator<T>
{
public:
    virtual bool match(const GeometryObject* pInput)
    {
        return (dynamic_cast<const T_Match1*>(pInput)!=0) || (dynamic_cast<const T_Match2*>(pInput)!=0);
    }
};

typedef QuickManager<GeometryOperator,GeometryOperatorFactory> GeometryOperatorManager;
*/

// class GeometryOperatorManager : public Singleton<GeometryOperatorManager>
// {
// public:
// 
//     void registerFactory(const String& type, GeometryOperatorFactory* pFactory);
//     
//     GeometryOperator* create(const String& type, Geometry* pInput);
//     
//     /** Return the list of all operators.
//     */
//     StringArray getAvailableTypes(void) const;
//     
//     /** Return the list of all operators matching pGeometry as input.
//     */
//     StringArray getAvailableTypes(const Geometry* pGeometry) const;
//     
// protected:
// 
//     typedef std::map<String,GeometryOperatorFactory*> FactoryMap;
//     FactoryMap mFactories;
// };

}

#endif

