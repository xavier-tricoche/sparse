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


#ifndef _ExpeIOManager_h_
#define _ExpeIOManager_h_

#include "ExpePrerequisites.h"
#include <QStringList>
#include "ExpeStringHelper.h"
#include "ExpeSingleton.h"
#include "ExpeLogManager.h"
#include "ExpeSerializableObject.h"
#include "ExpeReader.h"
#include "ExpeWriter.h"
#include "ExpeOptions.h"
#include <map>
#include <QFile>

namespace Expe
{

/** Manage data import/export
    To import to (resp. export from) q file an objet, it must derive from the base class SerializableObject.
    
    For import:
    \code
    // with automatic creation/initialization of the object,
    MyDataType* pData = IOManager::Instance().createFromFile\<MyDataType>("filename.ext");
    if (pData) {
        // OK !
    }
    // or,
    MyDataType* pData = new MyDataType(...);
    if (IOManager::Instance().loadFromFile("filename.ext", pData)) {
        // OK !
    }
    \endcode
*/
class IOManager : public Singleton<IOManager>
{
public:

    IOManager()
    {}

    ~IOManager();

    /** Create an object of type T from the file filename.
        Return 0 in the case of error.
    */
    template <class T> T* createFromFile(const QString& filename, Options& options = Options::EMPTY)
    {
        QStringList sa = filename.split(".");
        QString name = sa.front();
        QString ext = sa.back();
        ext.toLower();

        if( mReaderFactories.find(ext)!=mReaderFactories.end() )
        {
            QFile file(filename);
            Reader * pReader = mReaderFactories[ext]->create(&file);
            SerializableObject* pGenObject = pReader->create();
            // match ?
            T* pObject = dynamic_cast<T*>(pGenObject);
            assert(pObject==pGenObject);

            pReader->read(pGenObject, options);
            delete pReader;

            return pObject;
        }
        else
        {
            // try another (slower) way for complex extensions
            for (ReaderFactoryMap::iterator it = mReaderFactories.begin() ; it!=mReaderFactories.end() ; ++it)
            {
                if (filename.endsWith(QString(it->first)))
                {
                    QFile file(filename);
                    Reader * pReader = it->second->create(&file);
                    SerializableObject* pGenObject = pReader->create();
                    
                    // match ?
                    T* pObject = dynamic_cast<T*>(pGenObject);
                    assert(pObject==pGenObject);
                    
                    pReader->read(pGenObject, options);
                    delete pReader;
            
                    return pObject;
                }
            }
            LOG_ERROR("Cannot find any Reader for " + ext + " file type.");
        }

        return 0;
    }

    /** Generic version of the previous method allowing less control of the created data type.
    */
    SerializableObject* createFromFile(const QString& filename, Options& options = Options::EMPTY);

    /** Load the data of an object pGenObject from the file filename.
    */
    bool loadFromFile(const QString& filename, SerializableObject* pGenObject, Options& options = Options::EMPTY);

    /** Write pEntity to the file filename using the exporter of type method.
        If method is equal to "", an exporter is automatically selected from the filename extention.
    */
    void writeToFile(const SerializableObject* pEntity, const QString& filename, Options& options = Options::EMPTY, const QString& method="");

    void registerReader(ReaderFactory* factory);
    void registerWriter(WriterFactory* factory);

protected:

    typedef std::map<QString,ReaderFactory*> ReaderFactoryMap;
    typedef std::map<QString,WriterFactory*> WriterFactoryMap;

    ReaderFactoryMap mReaderFactories;
    WriterFactoryMap mWriterFactories;

};

}

#endif

