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



#include "ExpePtsReader.h"
#include "ExpePointSet.h"
#include "ExpeColor.h"
#include "ExpeLogManager.h"

namespace Expe
{

const QString Header_EPSB01 = "[Expe/PointSet/Binary/0.1]";
const QString Header_EPSB02 = "[Expe/PointSet/Binary/0.2]"; // support sized type
const QString Header_EPSA02 = "[Expe/PointSet/Ascii/0.2]";

PtsReader::PtsReader(QIODevice* pDevice)
    : PointSetReader(pDevice)
{
}

PtsReader::~PtsReader()
{
}

struct PtsReader::FileProperty
{
    FileProperty(QString _name, uint _size)
        : name(_name), size(_size)
    {}
    QString name;
    uint size;
    bool hasProperty;
    const PointSet::AttributeHandle* pPropertyHandle;
};


uint getSizeOfPropertyType(const QString& type)
{
    uint size = 0;
    if(type=="r32" || type=="ui32" || type=="i32")
    {
        size = 4;
    }
    else if(type=="r16" || type=="ui16" || type=="i16")
    {
        size = 2;
    }
    else if(type=="ui8" || type=="i8")
    {
        size = 1;
    }
    return size;
}

void PtsReader::readPointSet(PointSet* pDest, Options& options)
{
    if ( (!mpDevice->isOpen()) && (!mpDevice->open(QFile::ReadOnly)) )
        return;
    
    QTextStream stream(const_cast<QIODevice*>(mpDevice));
    
    LOG_DEBUG_MSG(8,"PointSetReader::readPointSet");
    // check the header file and version
    QString tmp, propertyName, commande;
    QString dataFormat;
    int propertySize, nofPoints;
    stream >> tmp;

    QStringList header = tmp.split(QRegExp("[\\[/\\]]"),QString::SkipEmptyParts);
    if(header.size()!=4)
    {
        LOG_ERROR("Invalid file header" << tmp);
        return;
    }

    if(header[0] != "Expe")
    {
        LOG_ERROR("Invalid file header");
        return;
    }

    if(header[1] != "PointSet")
    {
        LOG_ERROR("Invalid file header : must be a PointSet");
        return;
    }

    if(header[2]!="Ascii" && header[2]!="Binary")
    {
        LOG_ERROR("Invalid file header : must be Ascii or Binary");
        return;
    }

    if(header[3]!="0.1" && header[3]!="0.2")
    {
        LOG_ERROR("Invalid file header : unknow version");
        return;
    }

    // list property : name/type/
    const PointSet::Format& format = pDest->getFormat();
    
    FileProperties fileProperties;
    uint pointSize = 0;
    
    stream >> commande;
    while(commande!="data")
    {
        if(commande=="property")
        {
            if(header[3]=="0.1")
            {
                stream >> propertyName >> propertySize;
                
                fileProperties.push_back(FileProperty(propertyName,propertySize));
                if(!format.hasAttribute(propertyName))
                {
                    // try to append the property
                    if(propertyName=="radius")
                        pDest->addAttribute(PointSet::Attribute_radius);
                    else if(propertyName=="normal")
                        pDest->addAttribute(PointSet::Attribute_normal);
                    else if(propertyName=="color")
                        pDest->addAttribute(PointSet::Attribute_color);
                }
                fileProperties.back().hasProperty = format.hasAttribute(propertyName);
                if(fileProperties.back().hasProperty)
                {
                    fileProperties.back().pPropertyHandle = &format._getAttributeHandle(propertyName);
                }
                pointSize += propertySize;
                
                LOG_DEBUG_MSG(5,"pts reader " << "  property "
                    << propertyName << " " << propertySize << " bytes " 
                    << (fileProperties.back().hasProperty ? " X" : " ") );
            }
            else if(header[3]=="0.2")
            {
                QString propertyType;
                // name / nof atomic element / type of an atomic element
                stream >> propertyName >> propertySize >> propertyType;
                propertySize *= getSizeOfPropertyType(propertyType);
                
                fileProperties.push_back(FileProperty(propertyName,propertySize));
                if(!format.hasAttribute(propertyName))
                {
                    // try to append the property
                    if(propertyName=="radius")
                        pDest->addAttribute(PointSet::Attribute_radius);
                    else if(propertyName=="normal")
                        pDest->addAttribute(PointSet::Attribute_normal);
                    else if(propertyName=="color")
                        pDest->addAttribute(PointSet::Attribute_color);
                }
                fileProperties.back().hasProperty = format.hasAttribute(propertyName);
                if(fileProperties.back().hasProperty)
                {
                    fileProperties.back().pPropertyHandle = &format._getAttributeHandle(propertyName);
                }
                pointSize += propertySize;
                
                LOG_DEBUG_MSG(5,"pts reader " << "\tproperty "
                    << propertyName << " " << propertySize << " bytes " 
                    << (fileProperties.back().hasProperty ? " X" : " ") );
            }
        }
        else if(commande=="nofpoints")
        {
            stream >> nofPoints;
            LOG_DEBUG_MSG(5,"pts reader " << "\tnofpoints = " << nofPoints );
        }
        else
        {
            LOG_ERROR("Unknow command " + commande + " (skip)");
        }
        stream >> commande;
    }

    pDest->reserve(nofPoints);
    
    qint64 streamPos = stream.pos();
    stream.setDevice(0);
    mpDevice->reset();
    mpDevice->seek(streamPos);

    if(header[2]=="Binary")
        appendBinaryData(pDest, nofPoints, fileProperties, pointSize);
    else if(header[2]=="Ascii")
        appendAsciiData(pDest, nofPoints, fileProperties);
    
}

void PtsReader::appendAsciiData(PointSet* pDest, uint nofPoints, const FileProperties& fileProperties)
{
    QTextStream stream(const_cast<QIODevice*>(mpDevice));
    
    QStringList line;
    uint i=0;
    while(i<nofPoints && !stream.atEnd())
    {
        line = stream.readLine(1024).split(";");
        if (line.size()==int(fileProperties.size()))
        {
            PointSet::PointHandle point = pDest->append();
            point.color() = 0xffffffff;
            
            for(uint k=0 ; k<fileProperties.size() ; ++k)
            {
                if(fileProperties[k].hasProperty)
                {
                    if(fileProperties[k].name=="position")
                    {
                        if (!parse_vector(line[k], point.position())) {LOG_ERROR("Error parsing position " << line[k]);}
                    }
                    else if(fileProperties[k].name=="normal")
                    {
                        if (!parse_vector(line[k], point.normal())) {LOG_ERROR("Error parsing normal " << line[k]); }
                    }
//                     else if(fileProperties[k].name=="secondNormal")
//                     {
//                         point.secondNormal() = String::parseVector3(line[k]);
//                     }
                    else if(fileProperties[k].name=="radius")
                    {
                        bool ok = true;
                        point.radius() = line[k].toFloat(&ok);
                        if (!ok)
                            LOG_ERROR("Error parsing radius " << line[k]);
//                         if (!parse_vector(line[k], point.radius())) {LOG_ERROR("Error parsing radius " << line[k]); }
                    }
                    else if(fileProperties[k].name=="color")
                    {
                        Color c;
                        if (!parse_vector(line[k], c)) {LOG_ERROR("Error parsing color " << line[k]); }
                        point.color() = c.toRGBA();
                    }
                    else
                    {
                        LOG_ERROR("unsupported property " + fileProperties[k].name);
                    }
                }
            }
        }
        else if(line.size()!=0)
        {
            LOG_WARNING("\tInvalid line : " + line[0] + "...");
        }
    }
}

void PtsReader::appendBinaryData(PointSet* pDest, uint nofPoints, const FileProperties& fileProperties, uint pointSize)
{
    QDataStream stream(const_cast<QIODevice*>(mpDevice));
    ubyte* data = (ubyte*) malloc(pointSize * sizeof(ubyte));
    
    // skip \n
    stream.skipRawData(1);
    
    for(uint i=0 ; i<nofPoints ; ++i)
    {
        stream.readRawData(reinterpret_cast<char*>(data), pointSize);
        
        PointSet::PointHandle point = pDest->append(); //.copyFrom(PointSet::Point(point, fileFormat));
        uint offset = 0;
        for(uint k=0 ; k<fileProperties.size() ; ++k)
        {
            if(fileProperties[k].hasProperty)
            {
                memcpy(
                    point.attributePtr(*(fileProperties[k].pPropertyHandle)),
                    (data+offset),
                    fileProperties[k].size);
                //point.setProperty(fileProperties[k].name, data+offset);
            }
            offset += fileProperties[k].size;
        }
    }

	free(data);
}


}
