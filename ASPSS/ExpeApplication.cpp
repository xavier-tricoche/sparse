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



#include "ExpeApplication.h"
#include "ExpeStaticInitializer.h"
#include "ExpeLogManager.h"
#include "ExpePluginManager.h"
#include "ExpeSmartCast.h"

namespace Expe
{

// const Application* eApp = 0;

Application::Application(int & argc, char ** argv, bool GUIenabled)
    : QApplication(argc, argv, GUIenabled)
{
    initializeExpe();
}

void Application::initializeExpe(void)
{
    // initialize the Expe library
    eApp = this;
    QStringList args = eApp->arguments();
    
    // init the log system
    int dbgLevel = 5;
    {
        int i = args.indexOf("-dbgLevel");
        if (i>0 && i+1<args.size())
            dbgLevel = args.at(i+1).toInt();
    }
    Log::Initialize(dbgLevel);
    
    SmartCastManager::Initialize();
    
    StaticInitializer::InstanceSafe().processAll();
    
    // load all plugins
    PluginManager::InstanceSafe().loadAllPlugins(EXPE_PLUGIN_DIR);
    
    std::cout << "\n/--------------------------------------\\\n";
    std::cout <<   "|        Expé succefully loaded        |\n";
    std::cout <<   "\\--------------------------------------/\n\n";
}

}
