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



#include "ExpeIOManager.h"
#include "ExpeStaticInitializer.h"

namespace Expe
{

EXPE_SINGLETON_IMPLEMENTATION_AUTO(IOManager);

void IOManager::registerReader(ReaderFactory* pFactory)
{
    const QStringList& exts = pFactory->getSupportedExtensions();
    for(QStringList::const_iterator ext_iter=exts.begin() ; ext_iter!=exts.end() ; ++ext_iter)
    {
        QString ext = *ext_iter;
        ext.toLower();
        LOG_MESSAGE("Register a new Reader for " + ext + " file : " + pFactory->getType() );
        mReaderFactories[ext] = pFactory;
    }
}

void IOManager::registerWriter(WriterFactory* pFactory)
{
    const QStringList& exts = pFactory->getExtensions();
    for(QStringList::const_iterator ext_iter=exts.begin() ; ext_iter!=exts.end() ; ++ext_iter)
    {
        QString ext = *ext_iter;
        ext.toLower();
        LOG_MESSAGE("Register a new Writer for " + ext + " file : " + pFactory->getType() );
        mWriterFactories[ext] = pFactory;
    }
}

SerializableObject* IOManager::createFromFile(const QString& filename, Options& options)
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
        if(!mReaderFactories[ext]->match(pGenObject))
        {
            delete pGenObject;
            return 0;
        }
        pReader->read(pGenObject, options);
        delete pReader;

        return pGenObject;
    }
    else
    {
        // try another way (for complex extension)
        for (ReaderFactoryMap::iterator it = mReaderFactories.begin() ; it!=mReaderFactories.end() ; ++it)
        {
            if (filename.endsWith(QString(it->first)))
            {
                QFile file(filename);
                Reader * pReader = it->second->create(&file);
                SerializableObject* pGenObject = pReader->create();
                if(!it->second->match(pGenObject))
                {
                    delete pGenObject;
                    return 0;
                }
                pReader->read(pGenObject, options);
                delete pReader;
        
                return pGenObject;
            }
        }
        LOG_ERROR("Cannot find any Reader for " + ext + " file type.");
    }

    return 0;
}

bool IOManager::loadFromFile(const QString& filename, SerializableObject* pGenObject, Options& options)
{
    QStringList sa = filename.split(".");
    QString name = sa.front();
    QString ext = sa.back();
    ext.toLower();

    if( mReaderFactories.find(ext)!=mReaderFactories.end() )
    {
        assert(mReaderFactories[ext]->match(pGenObject));
        QFile file(filename);
        Reader * pReader = mReaderFactories[ext]->create(&file);
        pReader->read(pGenObject, options);
        delete pReader;
    }
    else
    {
        // try another way (for complex extension)
        for (ReaderFactoryMap::iterator it = mReaderFactories.begin() ; it!=mReaderFactories.end() ; ++it)
        {
            if (filename.endsWith(QString(it->first)))
            {
                assert(it->second->match(pGenObject));
                QFile file(filename);
                Reader* pReader = it->second->create(&file);
                pReader->read(pGenObject, options);
                delete pReader;
            }
        }
        
        LOG_ERROR("Cannot find any Reader for " + ext + " file type.");
        return false;
    }

    return true;
}

void IOManager::writeToFile(const SerializableObject* pObject, const QString& filename, Options& options, const QString& method)
{
    QStringList chunks = filename.split(".");
    QString name = chunks.front();
    QString ext = chunks.back();
    ext.toLower();

    if( mWriterFactories.find(ext)!=mWriterFactories.end() )
    {
        assert(mWriterFactories[ext]->match(pObject));
        QFile file(filename);
        Writer * pWriter = mWriterFactories[ext]->create(&file);
        pWriter->write(pObject, options);
        delete pWriter;
    }
}

}

