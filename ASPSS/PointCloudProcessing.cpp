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



#include "ExpeCore.h"
#include "ExpePointSet.h"
#include "ExpeStringHelper.h"

#include "ExpeMlsSurface.h"
#include "ExpeApplication.h"
#include "ExpeIOManager.h"
#include <QMetaProperty>
#include "ExpeLazzyUi.h"
#include "ExpeTimer.h"
#include "ExpeCliProgressBar.h"
#include "ExpeGeometryOperator.h"

using namespace Expe;

PointSet* loadInput(const QString& filename);
void printUsage(void);
const char* getInfo(const QMetaObject* meta, const char* name);

// template <class ManagerT> void printManagedHelp(const QString& msg)
// {
//     QStringList list = ManagerT::Instance().getAvailableTypes();
//     
//     std::cout << "\n" << msg << "\n";
//     for (QStringList::const_iterator it=list.begin() ; it!=list.end() ; ++it)
//     {
//         std::cout << "  * " << *it << "\t" << getInfo(ManagerT::Instance().getMetaObject(*it),"Description") << "\n";
//     }
// }
// 
void printObjectHelp(const QMetaObject* pMetaObject, const QObject* pObj=0)
{
    const Expe::LazzyUi::UiDescription* pUiDesc = LazzyUi::Instance().getUiDescription(pMetaObject->className());
    
    for (int i=0 ; i<pMetaObject->propertyCount() ; ++i)
    {
        QMetaProperty metaProp = pMetaObject->property(i);
        if (metaProp.isValid() && metaProp.isWritable() && metaProp.isDesignable(pObj))
        {
            std::cout << "  * " << metaProp.name() << " (" << metaProp.typeName() << ")";
            if (pObj)
            {
                QVariant val = metaProp.read(pObj);
                if (val.isValid())
                    std::cout << "\t = " << val.toString();
            }
            std::cout << "\n";
            if (metaProp.isEnumType())
            {
                QMetaEnum metaEnum = metaProp.enumerator();
                std::cout << "      | possible values are: ";
                for (int j=0 ; j<metaEnum.keyCount() ; ++j)
                    std::cout << " " << metaEnum.key(j) << ",";
                std::cout << "\n";
            }
            if (pUiDesc)
            {
                const LazzyUi::PropertyOptions* pPropOpts = pUiDesc->getPropertyOptions(metaProp.name());
                if (pPropOpts && pPropOpts->contains("description"))
                {
                    std::cout << "      | " << pPropOpts->value("description").toString() << "\n";
                }
            }
        }
    }
}

template <class ManagerT> void printManagedObjectHelp(const QString& objecttype, const QString& type, const QObject* pObj=0)
{
    const QMetaObject* pMetaObject = ManagerT::Instance().getMetaObject(type);
    if (!pMetaObject)
    {
        std::cout << "Unknow " << objecttype << " of type " << type << "\n";
        exit(2);
    }
    std::cout << "\nOptions for " << type << ":\n";
    printObjectHelp(ManagerT::Instance().getMetaObject(type), pObj);
}

void assignOptionsToObject(const QString& options, QObject* pObj)
{
    if (options=="")
        return;
    QStringList optList = options.split(":");
    for (int i=0 ; i<optList.size() ; ++i)
    {
        QStringList name_value = optList[i].split("=");
        if (name_value.size()==2)
        {
            if (pObj->metaObject()->indexOfProperty(name_value[0].toAscii())>=0)
                pObj->setProperty(name_value[0].toAscii(), name_value[1]);
            else
                std::cerr << "warning: unknow option \"" << name_value[0] << "\"\n";
        }
        else
        {
            std::cerr << "invalid option \"" << optList[i] << "\" parsing \"" << options << "\"\n";
            std::cerr << "should be of the form OptionName=value without any space.\n";
            exit(2);
        }
    }
}

struct ProgramArguments
{
    ProgramArguments(void)
    {
        inputFilename = "";
        outputFilename = "";
        queryHelp = false;
    }
    
    void parse(const QStringList& args)
    {
        uint nb = args.size();
        for (uint i=1 ; i<nb ; ++i)
        {
            if (args[i] == "--help" || args[i] == "-h")
                queryHelp = true;
            else if (args[i] == "-i" && i+1)
                inputFilename = args[++i];
            else if (args[i] == "-o" && i+1)
                outputFilename = args[++i];
            else if (args[i] == "-op" && i+1)
            {
                operations.push_back(args[++i]);
                operationOptions.resize(operations.size());
            }
            else if (args[i] == "-opopts" && i+1)
                operationOptions.back() = args[++i];
            else
            {
                std::cout << "Error: unknow option " << args[i] << "\n";
                exit(2);
            }
        }
    }
    
    QString inputFilename, outputFilename;
    
    bool queryHelp;
    std::vector<QString> operations;
    std::vector<QString> operationOptions;
};

int main(int argc, char* argv[])
{
    Expe::Application app(argc,argv);
    
    ProgramArguments args;
    args.parse(eApp->arguments());
    
    if (args.queryHelp || args.inputFilename=="" || args.outputFilename=="")
    {
        printUsage();
        
        for (uint i=0 ; i<args.operations.size() ; ++i)
        {
            if (args.operationOptions[i]=="help")
            {
                printManagedObjectHelp<GeometryOperatorManager>("GeometryOperator",args.operations[i]);
            }
        }
        std::cout << "\n";
        exit(0);
    }
    
    // input
    PointSetPtr pPoints = loadInput(args.inputFilename);
    
    if (!pPoints)
    {
        std::cout << "Failed to load input.\n";
        exit(1);
    }
    
    for (uint i=0 ; i<args.operations.size() ; ++i)
    {
        std::cout << "Apply operation #" << i << " => " << args.operations[i] << "\n";
        GeometryOperator* pOp = GeometryOperatorManager::Instance().create(args.operations[i],pPoints);
        if (!pOp)
        {
            std::cout << "Failed to create the operation.\n";
            exit(1);
        }
        if (args.operationOptions[i]=="help")
        {
            printManagedObjectHelp<GeometryOperatorManager>("GeometryOperator",args.operations[i],pOp);
            std::cout << "\n";
            exit(0);
        }
        assignOptionsToObject(args.operationOptions[i], pOp);
        if (!pOp->apply())
        {
            std::cout << "Failed to apply the operation.\n";
            exit(1);
        }
    }
    
    std::cout << "Save output...";
    IOManager::Instance().writeToFile(pPoints, args.outputFilename);
    std::cout << "Done.\n";
    return 0;
}


void printUsage(void)
{
    std::cout << "Usage: qApp.arguments().at(0) -i <input filename -o <output filename> -op <operation 1> -opopts <options for op1> -op <operation 2> ...\n";
    std::cout << "\narguments/options:\n";
    std::cout << "  -i <string>             Input file name, REQUIRED.\n";
    std::cout << "  -o <string>             Output file name, REQUIRED.\n";
    std::cout << "  -op <string>            Name the operation to apply.\n";
    std::cout << "  -opopts <opt list>      Options for the previous operation (use help to see the list of available options).\n";
    std::cout << "\nA <opt list> is a list of option_name=value separated by \":\" as follow (without any space):\n";
    std::cout << "-*opts OptName1=Value1:OptName2=Value2\n";
    std::cout << "\nList of available operations:\n";
    PointSet aux;
    QStringList list = GeometryOperatorManager::Instance().getAvailableTypes(&aux);
    for (QStringList::const_iterator it=list.begin() ; it!=list.end() ; ++it)
    {
        std::cout << "  * " << *it << "\t" << getInfo(GeometryOperatorManager::Instance().getMetaObject(*it),"Description") << "\n";
    }
    std::cout << "\n";
}

const char* getInfo(const QMetaObject* meta, const char* name)
{
    static const char* empty = "";
    if (meta==0)
        return empty;
    int i = meta->indexOfClassInfo(name);
    if (i<0)
        return empty;
    return meta->classInfo(i).value();
}


PointSet* loadInput(const QString& filename)
{
    Options customOpts;
    customOpts["reshape"] = true;
    customOpts["reshape::size"].setValue(100.f);
    customOpts["reshape::offset"].setValue(Vector3::ZERO);
    
    if (filename!="")
    {
        PointSet* pPoints = dynamic_cast<PointSet*>(IOManager::Instance().createFromFile(filename, customOpts));
        if (!pPoints)
            return 0;
        return pPoints;
    }
    return 0;
}

