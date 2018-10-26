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



#include "ExpePtsWriter.h"
#include "ExpePointSet.h"
#include "ExpeColor.h"

namespace Expe
{

const QString Header_EPSB01 = "[Expe/PointSet/Binary/0.1]";
const QString Header_EPSB02 = "[Expe/PointSet/Binary/0.2]";
const QString Header_EPSA02 = "[Expe/PointSet/Ascii/0.2]";


uint getSizeOfPropertyType(const QString& type);

QString getElementTypeOfProperty(const PointSet::Attribute& prop)
{
    QString str = "";
    if(prop.componentType()==Type::INT)
        str = "i32";
    else if(prop.componentType()==Type::UINT)
        str = "ui32";
    else if(prop.componentType()==Type::REAL)
        str = "r32";
    else if(prop.componentType()==Type::BYTE)
        str = "i8";
    else if(prop.componentType()==Type::UBYTE)
        str = "ui8";
    
    return str;
}

BinaryPtsWriter::BinaryPtsWriter(QIODevice* pDevice)
    : PointSetWriter(pDevice)
{
}

BinaryPtsWriter::~BinaryPtsWriter()
{
}

void BinaryPtsWriter::writePointSet(const PointSet* pPoints, Options& options)
{
    if ( (!mpDevice->isOpen()) && (!mpDevice->open(QFile::WriteOnly)) )
        return;
    
    QTextStream tstream(mpDevice);
    
    QString version="0.2";
    
    // write header
    if(version=="0.1")
        tstream << Header_EPSB01 << "\n";
    else if(version=="0.2")
        tstream << Header_EPSB02 << "\n";
    
    // write format
    // list property : name/type/
    uint size = pPoints->getFormat().size();
    const PointSet::AttributeList& attributes = pPoints->getFormat().getAttributes();
    for (PointSet::AttributeList::const_iterator a_it = attributes.begin() ; a_it!=attributes.end() ; ++a_it)
    {
        // write the property that will be storded
        
        if(version=="0.2")
        {
            QString elementTypeStr = getElementTypeOfProperty(*a_it);
            tstream << "property " 
                << a_it->name() <<  " " 
                << (a_it->size()/getSizeOfPropertyType(elementTypeStr)) << " "
                << elementTypeStr
                << "\n";
        }
        else
        {
            tstream << "property " 
                << a_it->name() << " " 
                << a_it->size() 
                << "\n";
        }
    }
    
    tstream << "nofpoints " << pPoints->size() << "\n" << "data" << "\n";
    
    tstream.setDevice(0);
    mpDevice->close();
    if ( (!mpDevice->isOpen()) && (!mpDevice->open(QFile::WriteOnly | QFile::Append)) )
        return;
    
    
//     qint64 streamPos = tstream.pos();
//     tstream.setDevice(0);
//     mpDevice->reset();
//     mpDevice->seek(streamPos);
    
    QDataStream bstream(mpDevice);
    
    for(PointSet::const_iterator point_iter = pPoints->begin() ; point_iter!=pPoints->end() ; ++point_iter)
    {
        bstream.writeRawData((const char *)(point_iter->position().data()),size);
//         writeData(reinterpret_cast<ConstPointer>(point_iter->position().data()),format.size(),1);
    }
}

AsciiPtsWriter::AsciiPtsWriter(QIODevice* pDevice)
    : PointSetWriter(pDevice)
{
}

AsciiPtsWriter::~AsciiPtsWriter()
{
}

void AsciiPtsWriter::writePointSet(const PointSet* pPoints, Options& options)
{
    if ( (!mpDevice->isOpen()) && (!mpDevice->open(QFile::WriteOnly)) )
        return;
    
    QTextStream stream(mpDevice);
    
    stream << Header_EPSA02 << "\n";
    
    // write format
    // list property : name/type/
    const PointSet::AttributeList& attributes = pPoints->getFormat().getAttributes();
    for (PointSet::AttributeList::const_iterator a_it = attributes.begin() ; a_it!=attributes.end() ; ++a_it)
    {
        // write the property that will be storded
        QString elementTypeStr = getElementTypeOfProperty(*a_it);
        stream << "property " 
            << a_it->name() <<  " " 
            << (a_it->size()/getSizeOfPropertyType(elementTypeStr)) << " "
            << elementTypeStr
            << "\n";
    }
    
    stream << "nofpoints " << pPoints->size() << "\n" << "data" << "\n";
    
    for (PointSet::const_iterator point_iter = pPoints->begin() ; point_iter!=pPoints->end() ; ++point_iter)
    {
        // position
        stream << point_iter->position().x << " " << point_iter->position().y << " " << point_iter->position().z;
        stream << " ; ";
        stream << point_iter->radius();
        stream << " ; ";
        stream << point_iter->normal().x << " " << point_iter->normal().y << " " << point_iter->normal().z;
        stream << " ; ";
        Color c;
        c.fromRGBA(point_iter->color());
        stream << c.r << " " << c.g << " " << c.b << " " << c.a;
        stream << "\n";
    }
}

}
