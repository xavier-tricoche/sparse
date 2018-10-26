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

struct ProgramArguments
{
    ProgramArguments(void)
    {
        inputFilename = "";
        outputFilename = "";
        swapInput = false;
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
            else if (args[i] == "-i_swap")
                swapInput = true;
            else
            {
                std::cout << "Error: unknow option " << args[i] << "\n";
                exit(2);
            }
        }
    }
    
    QString inputFilename, outputFilename;
    
    bool queryHelp, swapInput;
};

int main(int argc, char* argv[])
{
    Expe::Application app(argc,argv);
    
    ProgramArguments args;
    args.parse(eApp->arguments());
    
    if (args.queryHelp || args.inputFilename=="" || args.outputFilename=="")
    {
        printUsage();
        exit(0);
    }
    
    // input
    Options customOpts;
    customOpts["reshape"] = true;
    customOpts["reshape::size"].setValue(100.f);
    customOpts["reshape::offset"].setValue(Vector3::ZERO);
    
    if (args.swapInput)
        customOpts["swap"] = true;
    
    MeshPtr pMesh = dynamic_cast<Mesh*>(IOManager::Instance().createFromFile(args.inputFilename, customOpts));
    if (!pMesh)
    {
        std::cout << "Failed to load input.\n";
        exit(1);
    }

    // convert the Mesh to a PointSet
    BasicMesh2PointSet mesh2points(pMesh);
    if (!mesh2points.apply())
    {
        std::cout << "Convertion failed.\n";
        delete pMesh;
        exit(1);
    }
    PointSetPtr pPoints = dynamic_cast<PointSet*>(mesh2points.getResult());

    IOManager::Instance().writeToFile(pPoints, args.outputFilename);
    
    std::cout << "Done.\n";
    return 0;
}


void printUsage(void)
{
    std::cout << "Usage: qApp.arguments().at(0) <argument list>\n";
    std::cout << "\narguments/options:\n";
    std::cout << "  -h --help               Print this help.\n";
    std::cout << "  -i <string>             Input file name, REQUIRED.\n";
    std::cout << "      -i_swap             Swap the face orientation of the input mesh. (by default a ccw orientation is assumed)\n";
    std::cout << "  -o <string>             Output file name, REQUIRED.\n";
    std::cout << "\n";
}

