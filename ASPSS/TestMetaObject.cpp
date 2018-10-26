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



#include "ExpeVector2.h"
#include "ExpeStringHelper.h"
#include <QMetaProperty>
#include "ExpeTimer.h"

#include "ExpeSmartCast.h"
#include "ExpeLazzyUi.h"



namespace Expe {


class MyClassA : public QObject
{
Q_OBJECT
    
    Q_PROPERTY(Vector3 Position READ getPosition WRITE setPosition DESIGNABLE true STORED true);
    
    Q_PROPERTY(MyEnumC myenum READ getMyEnum WRITE setMyEnum DESIGNABLE true STORED true);
    
    Q_ENUMS(MyEnumC);
    
public:
    
    MyClassA() {}
    virtual ~MyClassA();
    
    enum MyEnumC {MEC_Value0, MEC_Value2 = 2, MEC_End};
    
public slots:
    
    const Vector3& getPosition(void) const {return mPosition;}
    void setPosition(const Vector3& p) {mPosition = p;}
    
    MyEnumC getMyEnum(void) const {return mMyEnum;}
    void setMyEnum(MyEnumC p) {mMyEnum = p;}
    
protected:
    
    MyEnumC mMyEnum;
    Vector3 mPosition;

};

class MyClassB : public MyClassA
{
    Q_OBJECT
    
    Q_PROPERTY(Vector3 Scale READ getScale WRITE setScale DESIGNABLE true STORED true);
    
public:
    
    MyClassB() {}
    virtual ~MyClassB();
    
public slots:
    
    // virtual void unSlot(void) const {std::cout << "\n unSlot A" << "\n\n";}
    
    const Vector3& getScale(void) const {return mScale;}
    void setScale(const Vector3& p) {mScale = p;}
    
protected:
    
    Vector3 mScale;

};


// class implementations

ELU_DECLARE(MyClassB,
    ELU_PROPERTY("Scale",
        ELU_OPT("category",QString("cat2"))
        ELU_OPT("description",QString("property1 !"))
        ELU_OPT("min",0.)
        ELU_OPT("max",10.)
        ELU_OPT("step",0.01)
    )
);

MyClassB::~MyClassB() {}


ELU_DECLARE(MyClassA,
    ELU_PROPERTY("Position",
        ELU_OPT("category",QString("cat1"))
        ELU_OPT("description",QString("property1 !"))
        ELU_OPT("min",0.)
        ELU_OPT("max",1.)
        ELU_OPT("step",0.1)
    )
);

MyClassA::~MyClassA() {}


#include "TestMetaObject.moc.cpp"

void dumpLazzyUi(const QObject* pObj)
{

    const QMetaObject* pMetaObject = pObj->metaObject();
    const Expe::LazzyUi::UiDescription* pUiDesc = LazzyUi::Instance().getUiDescription(pMetaObject->className());
    
    for (int i=0 ; i<pMetaObject->propertyCount() ; ++i)
    {
        QMetaProperty metaProp = pMetaObject->property(i);
        std::cout << "Property #" << i << " - \"" << metaProp.name() << "\"\n";
        std::cout << "\ttype = " << metaProp.typeName() << "\n";
        std::cout << "\treadable = " << metaProp.isReadable() << "\n";
        std::cout << "\twritable = " << metaProp.isWritable() << "\n";
        std::cout << "\tvalid = " << metaProp.isValid() << "\n";
        if (metaProp.isEnumType())
        {
            QMetaEnum metaEnum = metaProp.enumerator();
            std::cout << "\tenumerator = " << metaEnum.name() << "\n";
            for (uint j=0 ; j<metaEnum.keyCount() ; ++j)
                std::cout << "\t\t" << metaEnum.key(j) << " = " << metaEnum.value(j) << "\n";
        }
        if (pUiDesc)
        {
            const LazzyUi::PropertyOptions* pPropOpts = pUiDesc->getPropertyOptions(metaProp.name());
            if (pPropOpts)
            {
                std::cout << "\tlazzy ui options:\n";
    
                LazzyUi::PropertyOptions::Iterator opt(*pPropOpts);
                while (opt.hasNext()) {
                    opt.next();
                    std::cout << "\t\t" << opt.key() << " = " << opt.value().toString() << " (" << opt.value().typeName() << ")\n";
                }
            }
        }
    }
}

}

void testCustumType(void)
{
    Expe::MyClassB obj;
    dumpLazzyUi(&obj);
}


using namespace Expe;

int main(int argc, char* argv[])
{
    Expe::SmartCastManager::Initialize();
    testCustumType();

    return 0;
}
