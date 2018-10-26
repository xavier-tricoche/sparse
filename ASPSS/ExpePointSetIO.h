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



#ifndef _ExpePointSetIO_h_
#define _ExpePointSetIO_h_

#include "ExpePrerequisites.h"

#include "ExpeIOManager.h"
#include "ExpeReader.h"
#include "ExpeWriter.h"
#include "ExpePointSet.h"

namespace Expe
{


class PointSetReader : public Reader
{
public:
    PointSetReader(QIODevice* pDevice);
    
    virtual ~PointSetReader();
    
    virtual SerializableObject* create(void);
    virtual void read(SerializableObject* pDest, Options& options = Options::EMPTY);
    
protected:

    virtual void readPointSet(PointSet* pPointSet, Options& options = Options::EMPTY) = 0;

};

template <class T_reader> class PointSetReaderFactoryT : public ReaderFactoryT<T_reader,PointSet>
{
public:
    PointSetReaderFactoryT(const QString& name, const QString& exts)
        : ReaderFactoryT<T_reader,PointSet>(name, "PointSet", exts)
    {}
};



class PointSetWriter : public Writer
{
public:
    PointSetWriter(QIODevice* pDevice);
    
    virtual ~PointSetWriter();
    
    virtual void write(const SerializableObject* pDest, Options& options = Options::EMPTY);
    
protected:
    virtual void writePointSet(const PointSet* pPointSet, Options& options = Options::EMPTY) = 0;
};

template <class T_writer> class PointSetWriterFactoryT : public WriterFactoryT<T_writer,PointSet>
{
public:
    PointSetWriterFactoryT(const QString& name, const QString& exts)
        : WriterFactoryT<T_writer,PointSet>(name, "PointSet", exts)
    {}
};

}

#endif

