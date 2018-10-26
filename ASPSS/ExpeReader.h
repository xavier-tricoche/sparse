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



#ifndef _ExpeReader_h_
#define _ExpeReader_h_

#include "ExpePrerequisites.h"

#include "ExpeSerializableObject.h"
#include "ExpeOptions.h"
#include <QStringList>

class QIODevice;

namespace Expe
{

class Reader
{
public:

    Reader(QIODevice* pDevice);

    virtual ~Reader();

    virtual SerializableObject* create(void) = 0;
    virtual void read(SerializableObject* pDest, Options& options = Options::EMPTY) = 0;

protected:

    QIODevice* mpDevice;

};



class ReaderFactory
{
public:

    ReaderFactory()
    {}

    virtual ~ReaderFactory()
    {}

    virtual const QString& getType(void) const = 0;
    virtual const QStringList& getSupportedExtensions(void) const = 0;

    /** Return true if the Reader can handle this kind of object.
    */
    virtual bool match(const SerializableObject* pObject) const = 0;

    virtual Reader* create(QIODevice* pDevice) = 0;

protected:
};


/** Convenient template class allowing to easily create a ReaderFactory.
*/
template<class T_reader,class T_object> class ReaderFactoryT : public ReaderFactory
{
public:

    ReaderFactoryT(const QString& name, const QString& type, const QString& exts)
        : mName(name), mType(type)
    {
        mExtensions = exts.split(";");
    }

    virtual ~ReaderFactoryT()
    {
    }

    virtual const QString& getType(void) const
    {
        return mType;
    }

    virtual const QStringList& getSupportedExtensions(void) const
    {
        return mExtensions;
    }

    virtual bool match(const SerializableObject* pObject) const
    {
        const T_object* pTest = dynamic_cast<const T_object*>(pObject);
        return pTest==pObject;
    }

    virtual Reader* create(QIODevice* pDevice)
    {
        return new T_reader(pDevice);
    }

protected:

    QString mName;
    QString mType;
    QStringList mExtensions;

};

}

#endif

