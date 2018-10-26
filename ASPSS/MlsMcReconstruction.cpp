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
#include "ExpeVector3.h"
#include "ExpeStringHelper.h"

#include "ExpeMlsSurface.h"
#include "ExpeApplication.h"
#include "ExpeBasicMesh2PointSet.h"
#include "ExpeMeshNormalEvaluator.h"
#include "ExpeIOManager.h"
#include "ExpeMarchingCube.h"
#include <QMetaProperty>
#include "ExpeLazzyUi.h"
#include "ExpeTimer.h"
#include "ExpeCliProgressBar.h"

using namespace Expe;

void printUsage(void);
PointSet* loadInput(const QString& filename, bool swap);

template <class ManagerT> void printManagedHelp(const QString& msg)
{
    QStringList list = ManagerT::Instance().getAvailableTypes();
    
    std::cout << "\n" << msg << "\n";
    for (QStringList::const_iterator it=list.begin() ; it!=list.end() ; ++it)
    {
        std::cout << "  * " << *it << "\t" << getInfo(ManagerT::Instance().getMetaObject(*it),"Description") << "\n";
    }
}

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
        mcOptions = "";
        outputNormalMode = "";
        mlsVariant = "APSS";
        mlsOptions = "";
        neighborhoodType = "EuclideanNeighborhood";
        neighborhoodOptions = "";
        weightFunction = "OneMinusX2Power4";
        weightFunctionOptions = "";
        swapInput = false;
        swapOutput = false;
        mcRaw = false;
        queryHelp = false;
        mcResolution = 200;
        normalMode = "none";
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
            else if (args[i] == "-mls" && i+1)
                mlsVariant = args[++i];
            else if (args[i] == "-mlsopts" && i+1)
                mlsOptions = args[++i];
            else if (args[i] == "-n" && i+1)
                neighborhoodType = args[++i];
            else if (args[i] == "-nopts" && i+1)
                neighborhoodOptions = args[++i];
            else if (args[i] == "-w" && i+1)
                weightFunction = args[++i];
            else if (args[i] == "-wopts" && i+1)
                weightFunctionOptions = args[++i];
            else if (args[i] == "-mc_gs" && i+1)
                mcResolution = args[++i].toInt();
            else if (args[i] == "-o_normal" && i+1)
                outputNormalMode = args[++i];
            else if (args[i] == "-mc_raw")
                mcRaw = true;
            else if (args[i] == "-i_swap")
                swapInput = true;
            else if (args[i] == "-o_swap")
                swapOutput = true;
            else
            {
                std::cout << "Error: unknow option " << args[i] << "\n";
                exit(2);
            }
        }
    }
    
    QString inputFilename, outputFilename,
        mcOptions,
        outputNormalMode,
        mlsVariant, mlsOptions,
        neighborhoodType, neighborhoodOptions,
        weightFunction, weightFunctionOptions,
        normalMode;
    
    bool queryHelp, swapInput, swapOutput, mcRaw;
    int mcResolution;
};

int main(int argc, char* argv[])
{
    Expe::Application app(argc,argv);
    
    ProgramArguments args;
    args.parse(eApp->arguments());
    
    if (args.queryHelp || args.inputFilename=="" || args.outputFilename=="")
    {
        printUsage();
        if (args.mlsVariant=="help")
            printManagedHelp<MlsSurfaceManager>("List of available MLS implementations:");
        else if ( (args.mlsVariant!="") && (args.mlsOptions=="help") )
            printManagedObjectHelp<MlsSurfaceManager>("MlsSurface",args.mlsVariant);
            
        if (args.neighborhoodType=="help")
            printManagedHelp<NeighborhoodManager>("List of available neighborhoods:");
        else if ( (args.neighborhoodType!="") && (args.neighborhoodOptions=="help") )
            printManagedObjectHelp<NeighborhoodManager>("Neighborhood",args.neighborhoodType);
        
        if (args.weightFunction=="help")
            printManagedHelp<WeightingFunctionManager>("List of available weight functions:");
        else if ( (args.weightFunction!="") && (args.weightFunctionOptions=="help") )
            printManagedObjectHelp<WeightingFunctionManager>("WeightingFunction",args.weightFunction);
        
        if (args.mcOptions=="help")
            printObjectHelp(&MarchingCube::staticMetaObject, 0);
        exit(0);
    }
    
    // input
    PointSetPtr pPoints = loadInput(args.inputFilename, args.swapInput);
    
    if (!pPoints)
    {
        std::cout << "failed to load input\n";
        exit(1);
    }
    
    bool helpQueried = false;
    
    // create the MLS surface
    MlsSurface* mls = MlsSurfaceManager::Instance().create(args.mlsVariant,pPoints);
    if (!mls)
    {
        std::cout << "Error creating the MLS surface type \"" << args.mlsVariant << "\"\n";
        exit(2);
    }
    
    if (args.mlsOptions=="help")
    {
        printManagedObjectHelp<MlsSurfaceManager>("MlsSurface", args.mlsVariant, mls);
        helpQueried = true;
    }
    else
    {
        assignOptionsToObject(args.mlsOptions,mls);
    }
    
    if (NeighborhoodManager::Instance().getMetaObject(args.neighborhoodType)==0)
    {
        std::cout << "Error: unknow Neighborhood of type \"" << args.neighborhoodType << "\"\n";
        exit(2);
    }
    mls->selectNeighborhood(args.neighborhoodType);
    if (args.neighborhoodOptions=="help")
    {
        printManagedObjectHelp<NeighborhoodManager>("Neighborhood", args.neighborhoodType, mls->editNeighborhood());
        helpQueried = true;
    }
    else
    {
        assignOptionsToObject(args.neighborhoodOptions,mls->editNeighborhood());
    }
    
    if (WeightingFunctionManager::Instance().getMetaObject(args.weightFunction)==0)
    {
        std::cout << "Error: unknow WeightingFunction of type \"" << args.weightFunction << "\"\n";
        exit(2);
    }
    mls->selectWeightingFunction(args.weightFunction);
    if (args.weightFunctionOptions=="help")
    {
        printManagedObjectHelp<NeighborhoodManager>("Neighborhood", args.neighborhoodType, mls->editNeighborhood());
        helpQueried = true;
    }
    else
    {
        assignOptionsToObject(args.neighborhoodOptions,mls->editNeighborhood());
    }
    
    // do Marching Cube reconstruction
    MarchingCube* mc = new MarchingCube();
    if (args.mcOptions=="help")
    {
        printObjectHelp(&MarchingCube::staticMetaObject, mc);
        helpQueried = true;
    }
    else
    {
        assignOptionsToObject(args.mcOptions,mc);
    }
    
    if (helpQueried)
        exit(0);
    
    AxisAlignedBox aabb;
    Real maxr = 0.;
    for (uint i=0 ; i<pPoints->size() ; ++i)
    {
        if (pPoints->at(i).radius()>maxr)
            maxr = pPoints->at(i).radius();
        aabb.extend(pPoints->at(i).position());
    }
    Vector3 epsilon = 0.1 * (aabb.max() - aabb.min()) + 3.*Vector3(maxr,maxr,maxr);
    aabb.setMinimum( aabb.min() - epsilon );
    aabb.setMaximum( aabb.max() + epsilon );
    mc->setAABB(aabb);
    
    if (args.mcResolution<1 || args.mcResolution>10000)
    {
        std::cout << "Error: invalid marching cube grid size :" << args.mcResolution << "\n";
        exit(2);
    }
    
    mc->setResolution(args.mcResolution);
    mc->setIsoValue(0.);
    mc->setSurface(mls);
    
    bool mcok;
    if (args.mcRaw)
    {
        mc->setClusteringThreshold(1e-9);
        mcok = mc->_polygonize();
    }
    else
    {
        mcok = mc->doReconstruction();
    }
    
    if (mcok)
    {
        MeshPtr pMesh = mc->getMesh();
        mc->printStats();
        
        if (!args.outputFilename.isNull())
        {
            Options opts;
            opts["export_normals"] = false;
            if (args.normalMode=="mesh")
            {
                MeshNormalEvaluator normalEvaluator(pMesh);
                if (!normalEvaluator.apply())
                {
                    LOG_ERROR("ObjMeshReader: error computing the normals.");
                    exit(2);
                }
                opts["export_normals"] = true;
            }
            else if (args.normalMode=="numeric")
            {
                LOG_MESSAGE("Compute the normals of the mesh...");
                mc->_computeNormalsFromNumericGradient();
                opts["export_normals"] = true;
            }
            else if (args.normalMode=="analytic")
            {
                LOG_MESSAGE("Compute the normals from the analytic gradient...");
                uint nb = pMesh->getNofVertices();
                CliProgressBarT<int> progressBar(0,nb-1);
                for(uint i=0 ; i<nb ; i++)
                {
                    pMesh->vertex(i).normal() = mls->gradient(pMesh->vertex(i).position()).normalized();
                    progressBar.update(i);
                }
                opts["export_normals"] = true;
            }
            else if (args.normalMode=="approx")
            {
                if (args.mcRaw)
                {
                    mc->_projection();
                }
                opts["export_normals"] = true;
            }
            
            if (args.swapOutput)
                opts["swap"] = true;

            std::cout << "write mesh to " << args.outputFilename << "\n";
            IOManager::Instance().writeToFile(pMesh, args.outputFilename, opts);
            
//             BasicMesh2PointSet mesh2points(pMesh);
//             if (mesh2points.apply())
//             {
//                 IOManager::Instance().writeToFile(dynamic_cast<PointSet*>(mesh2points.getResult()), "output.pts");
//             }
        }
    }
    else
    {
        std::cerr << "Failed to reconstruct the surface\n";
        exit(1);
    }
    
    return 0;
}


void printUsage(void)
{
    std::cout << "Usage: qApp.arguments().at(0) <argument list>\n";
    std::cout << "\narguments/options:\n";
    std::cout << "  -h --help               Print this help.\n";
    std::cout << "  -i <string>             Input file name, REQUIRED. (use help to see the list of supported file format)\n";
    std::cout << "      -i_swap             Swap the face orientation of the input mesh. (by default a ccw orientation is assumed)\n";
    std::cout << "  -o <string>             Output file name, REQUIRED. (use help to see the list of supported mesh format)\n";
    std::cout << "      -o_normal <string>  Normal computation mode: none, approx, numeric, analytic, mesh (default: none)\n";
    std::cout << "      -o_swap             Swap the face orientation of the output mesh. (by default the faces are ccw oriented)\n";
    std::cout << "  -mls <string>           Specifies the MLS implementation. (use help to see the list of\n";
    std::cout << "                          available variants)\n";
    std::cout << "  -mlsopts <opt list>     Specifies the options of the MLS. (use help to get the list\n";
    std::cout << "                          of available options for the selected MLS)\n";
    std::cout << "  -n <string>             Specifies the neigborhood. (use help to see the list of\n";
    std::cout << "                          available variants)\n";
    std::cout << "  -nopts <opt list>       Specifies the options of the neighborhood. (use help to get the list\n";
    std::cout << "                          of available options for the selected neighborhood)\n";
    std::cout << "  -w <string>             Specifies the weight function. (use help to see the list of\n";
    std::cout << "                          available variants)\n";
    std::cout << "  -wopts <opt list>       Specifies the options of the weight function. (use help to get the list\n";
    std::cout << "                          of available options for the selected weight function)\n";
    std::cout << "  -mc_gs <int>            Specifies the grid size of the marching cube algorithm.\n";
    std::cout << "  -mc_raw                 Use a standard marching cube whithout any mesh optimizations.\n";
    std::cout << "  -mcopts <opt list>      Advanced marching cube options (use help).\n";
    std::cout << "\nA <opt list> is a list of option_name=value separated by \":\" as follow (without any space):\n";
    std::cout << "-*opts OptName1=Value1:OptName2=Value2\n";
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


PointSet* loadInput(const QString& filename, bool swap)
{
    Options customOpts;
    customOpts["reshape"] = true;
    customOpts["reshape::size"].setValue(100.f);
    customOpts["reshape::offset"].setValue(Vector3::ZERO);
    
    if (swap)
        customOpts["swap"] = true;
        
    if (filename!="")
    {
        SerializableObject* pGen = IOManager::Instance().createFromFile(filename, customOpts);
        if (!pGen)
            return 0;
        
        PointSet* pPoints = dynamic_cast<PointSet*>(pGen);
        if (pPoints)
            return pPoints;
        
        Mesh* pMesh = dynamic_cast<Mesh*>(pGen);
        if (pMesh)
        {
            // convert the Mesh to a PointSet
            BasicMesh2PointSet mesh2points(pMesh);
            if (!mesh2points.apply())
            {
                delete pMesh;
                return 0;
            }
            delete pMesh;
            return dynamic_cast<PointSet*>(mesh2points.getResult());
        }
    }
    return 0;
}

