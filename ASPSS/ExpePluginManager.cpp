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



#include "ExpePluginManager.h"
#include "ExpeStaticInitializer.h"
#include "ExpeLogManager.h"
#include <iostream>
#include "ExpeStringHelper.h"
#include <QDir>
#include <QLibrary>

namespace Expe
{

EXPE_SINGLETON_IMPLEMENTATION_AUTO(PluginManager);

PluginManager::PluginManager()
{
    #ifdef EXPE_PLUGIN_DIR
        std::cout << "PluginManager: plugin directories: " << EXPE_PLUGIN_DIR << "\n";
        mPluginDirs = QString(EXPE_PLUGIN_DIR).split(";");
    #else
        #warning EXPE_PLUGIN_DIR not defined. Set plugins path to "./".
        std::cerr << "EXPE_PLUGIN_DIR is not defined. Set plugins path to \"./\".\n";
        mPluginDirs.push_back("./");
    #endif
    
}

PluginManager::~PluginManager()
{
}
    
void PluginManager::loadAllPlugins(const QString& path)
{
    QDir pluginDirectory(path);
    QStringList allFiles = pluginDirectory.entryList(QStringList() << "*");
    for (QStringList::iterator it=allFiles.begin() ; it!=allFiles.end() ; ++it)
    {
        if (*it=="." || *it=="..")
            continue;
        
        QLibrary* plugin = new QLibrary(path + "/" + *it);
        StartPlugin startFunc = (StartPlugin)plugin->resolve("startPlugin");
        if (startFunc)
        {
            startFunc();
            mPlugins.push_back(plugin);
            LOG_MESSAGE("Plugin \"" + *it + "\" successfully loaded.");
        }
        else
        {
            LOG_MESSAGE("Error loading plugin \"" + *it + "\".");
        }
    }
}

void PluginManager::unloadAllPlugins(void)
{
    for (std::vector<QLibrary*>::iterator plug_iter=mPlugins.begin() ; plug_iter!=mPlugins.end() ; ++plug_iter)
    {
        StopPlugin stopFunc = (StopPlugin)(*plug_iter)->resolve("stopPlugin");
        if(!stopFunc)
        {
            LOG_ERROR("PluginManager::unloadPlugin: function stopPlugin not find");
            return;
        }
        stopFunc();
    }
}

void PluginManager::loadPlugin(const QString& pluginName)
{
    QStringList::iterator pluginDirIter = mPluginDirs.begin();
    QString pluginFileName("");
    bool found = false;
    while (pluginDirIter!=mPluginDirs.end() && !found)
    {
        QString sep("/");
        if (pluginDirIter->endsWith("/"))
            sep = "";
        
        QDir pluginDirectory(*pluginDirIter);
        
        QStringList possibilities = pluginDirectory.entryList(QStringList()
            << (QString("libPlugin_") + pluginName + "*")
            << (QString("lib") + pluginName + "*")
            << (pluginName + "*")
            << (QString("Plugin_") + pluginName + "*"));
        
        if (!possibilities.empty())
        {
            found = true;
            pluginFileName = possibilities.first();
        }
        
        ++pluginDirIter;
    }
    
    if (!found)
    {
        // let us do QLibrary its job
        pluginFileName = pluginName;
    }
    
    QLibrary* plugin = new QLibrary(pluginFileName);
    StartPlugin startFunc = (StartPlugin)plugin->resolve("startPlugin");
    if (!startFunc)
    {
        if (!found)
            LOG_ERROR("PluginManager::loadPlugin: function startPlugin not found in plugin (or plugin not found) " << pluginFileName);
        else
            LOG_ERROR("PluginManager::loadPlugin: function startPlugin not found in plugin " << pluginFileName);
            
        return;
    }
    startFunc();
    mPlugins.push_back(plugin);
    LOG_MESSAGE("Plugin \"" + pluginFileName + "\" successfully loaded.");
}

void PluginManager::unloadPlugin(const QString& /*pluginName*/)
{
    LOG_ERROR("Not implemented yet !");
    #pragma message ( "#warning Not implemented yet !" )
}

}
 
