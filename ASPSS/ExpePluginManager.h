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



#ifndef _ExpePluginManager_h_ 
#define _ExpePluginManager_h_

#include "ExpePrerequisites.h"

#include "ExpeSingleton.h"
#include <QStringList>
class QLibrary;

namespace Expe
{

class PluginManager : public Singleton<PluginManager>
{
public:

    PluginManager();
    
    ~PluginManager();
    
    /** Loads all the plugins found in the directory dirName.
    */
    void loadAllPlugins(const QString& dirName);
    
    /** Unloads all loaded plugins.
    */
    void unloadAllPlugins(void);
    
    /** Loads a plugin.
    */
    void loadPlugin(const QString& pluginName);
    
    /** Unloads a plugins.
    */
    void unloadPlugin(const QString& pluginName);

protected:

    typedef void (*StartPlugin)(void);
    typedef void (*StopPlugin)(void);
    
    std::vector<QLibrary*> mPlugins;
    
    QStringList mPluginDirs;

};

}

#endif

