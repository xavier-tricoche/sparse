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



#include "ExpeStringHelper.h"
#include <iostream>

// std::ostream& operator << (std::ostream& o, const QString& str)
// {
//     o << str.toLocal8Bit().data();
//     return o;
// }

#if 0
#include "ExpeVector3.h"
#include "ExpeMatrix3.h"
#include "ExpeMatrix4.h"
#include "ExpeQuaternion.h"
#include "ExpeColor.h"

#include <qstringlist.h>

#include <sstream>
#include <fstream>

namespace Expe {

    const String String::EMPTY = "";

    //-----------------------------------------------------------------------
    void String::trim(bool left, bool right)
    {
        size_t lspaces, rspaces, len = length(), i;

        lspaces = rspaces = 0;

        if( left )
        {
            // Find spaces / tabs on the left
            for( i = 0;
                i < len && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r' || at(i) == '\n');
                ++lspaces, ++i );
        }

        if( right && lspaces < len )
        {
            // Find spaces / tabs on the right
            for( i = len - 1;
                i >= 0 && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r' || at(i) == '\n');
                rspaces++, i-- );
        }

        *this = substr(lspaces, len-lspaces-rspaces);
    }

    //-----------------------------------------------------------------------
    StringArray String::split( const String& delims, unsigned int maxSplits) const
    {
        // static unsigned dl;
        StringArray ret;
        unsigned int numSplits = 0;

        // Use STL methods
        size_t start, pos;
        start = 0;
        do
        {
            pos = find_first_of(delims, start);
            if (pos == start)
            {
                // Do nothing
                start = pos + 1;
            }
            else if (pos == npos || (maxSplits && numSplits == maxSplits))
            {
                // Copy the rest of the string
                ret.push_back( substr(start) );
            }
            else
            {
                // Copy up to delimiter
                ret.push_back( substr(start, pos - start) );
                start = pos + 1;
            }
            // parse up to next real data
            start = find_first_not_of(delims, start);
            ++numSplits;

        } while (pos != npos);



        return ret;
    }

    //-----------------------------------------------------------------------
    String String::toLowerCase(void)
    {
        std::transform(
            begin(),
            end(),
            begin(),
            static_cast<int(*)(int)>(::tolower) );

        return *this;
    }

    //-----------------------------------------------------------------------
    String String::toUpperCase(void)
    {
        std::transform(
            begin(),
            end(),
            begin(),
            static_cast<int(*)(int)>(::toupper) );

        return *this;
    }
    //-----------------------------------------------------------------------
    Real String::toReal(void) const
    {
        return (Real)atof(this->c_str());
    }

    bool String::appendFile(const String& filename)
    {
        char buffer[2024];
        std::ifstream *file;
        file = new std::ifstream(filename, std::ios_base::in);
        if( ! file->fail() )
        {
            while(file->getline(buffer, 2024))
            {
                if(buffer[0] != '#')
                    (*this) += String( (char*)buffer ) + "\n";
            }
            file->close();
            delete file;

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    String String::toString(Real val)
    {

        StrStreamType stream;
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(Real val, int precision /*=3*/)
    {

        StrStreamType stream;
        Real scale = pow(10.,precision);
        stream << (Real)((Real)((int)(scale*val)))/scale;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(int val)
    {
        String::StrStreamType stream;
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(unsigned int val)
    {
        String::StrStreamType stream;
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(long val)
    {
        String::StrStreamType stream;
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(unsigned long val)
    {
        String::StrStreamType stream;
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(const Vector3& val)
    {
        String::StrStreamType stream;
        stream << val.x << " " << val.y << " " << val.z;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(const Matrix3& val)
    {
        String::StrStreamType stream;
        stream << val[0][0] << " "
            << val[0][1] << " "
            << val[0][2] << " "
            << val[1][0] << " "
            << val[1][1] << " "
            << val[1][2] << " "
            << val[2][0] << " "
            << val[2][1] << " "
            << val[2][2];
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(bool val)
    {
        if (val)
            return "true";
        else
            return "false";

    }
    //-----------------------------------------------------------------------
    String String::toString(const Matrix4& val)
    {
        String::StrStreamType stream;
        stream
            << val.get(0,0) << " "
            << val.get(0,1) << " "
            << val.get(0,2) << " "
            << val.get(0,3) << " "
            << val.get(1,0) << " "
            << val.get(1,1) << " "
            << val.get(1,2) << " "
            << val.get(1,3) << " "
            << val.get(2,0) << " "
            << val.get(2,1) << " "
            << val.get(2,2) << " "
            << val.get(2,3) << " "
            << val.get(3,0) << " "
            << val.get(3,1) << " "
            << val.get(3,2) << " "
            << val.get(3,3);

        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(const Quaternion& val)
    {
        String::StrStreamType stream;
        stream << val.x << " " << val.y << " " << val.z << " " << val.w;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String String::toString(const Color& val)
    {
        String::StrStreamType stream;
        stream << val.r << " " << val.g << " " << val.b << " " << val.a;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    Real String::parseReal(const String& val)
    {
        return atof(val.c_str());
    }
    //-----------------------------------------------------------------------
    int String::parseInt(const String& val)
    {
        return atoi(val.c_str());
    }
    //-----------------------------------------------------------------------
    unsigned int String::parseUnsignedInt(const String& val)
    {
        return atoi(val.c_str());
    }
    //-----------------------------------------------------------------------
    long String::parseLong(const String& val)
    {
        return atol(val.c_str());
    }
    //-----------------------------------------------------------------------
    unsigned long String::parseUnsignedLong(const String& val)
    {
        return atol(val.c_str());
    }
    //-----------------------------------------------------------------------
    bool String::parseBool(const String& val)
    {
        if (val == "true")
            return true;
        else
            return false;
    }
    //-----------------------------------------------------------------------
    Vector3 String::parseVector3(const String& val)
    {
        // Split on space
        std::vector<String> vec = val.split();

        if (vec.size() != 3)
        {
            return Vector3::ZERO;
        }
        else
        {
            return Vector3(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]));
        }

    }
    //-----------------------------------------------------------------------
    Matrix3 String::parseMatrix3(const String& val)
    {
        // Split on space
        std::vector<String> vec = val.split();

        if (vec.size() != 9)
        {
            return Matrix3::IDENTITY;
        }
        else
        {
            return Matrix3(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]),
                parseReal(vec[3]),parseReal(vec[4]),parseReal(vec[5]),
                parseReal(vec[6]),parseReal(vec[7]),parseReal(vec[8]));
        }
    }
    //-----------------------------------------------------------------------
    Matrix4 String::parseMatrix4(const String& val)
    {
        // Split on space
        std::vector<String> vec = val.split();

        if (vec.size() != 16)
        {
            return Matrix4::IDENTITY;
        }
        else
        {
            return Matrix4(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), parseReal(vec[3]),
                parseReal(vec[4]),parseReal(vec[5]), parseReal(vec[6]), parseReal(vec[7]),
                parseReal(vec[8]),parseReal(vec[9]), parseReal(vec[10]), parseReal(vec[11]),
                parseReal(vec[12]),parseReal(vec[13]), parseReal(vec[14]), parseReal(vec[15]));
        }
    }
    //-----------------------------------------------------------------------
    Quaternion String::parseQuaternion(const String& val)
    {
        // Split on space
        std::vector<String> vec = val.split();

        if (vec.size() != 4)
        {
            return Quaternion::IDENTITY;
        }
        else
        {
            return Quaternion(parseReal(vec[3]), parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]));
        }
    }
    //-----------------------------------------------------------------------
    Color String::parseColor(const String& val)
    {
        // Split on space
        std::vector<String> vec = val.split();

        if (vec.size() == 4)
        {
            return Color(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), parseReal(vec[3]));
        }
        else if (vec.size() == 3)
        {
            return Color(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), 1.0f);
        }
        else
        {
            return Color::BLACK;
        }
    }
    //-----------------------------------------------------------------------
    void String::parse(Real& val) const
    {
      val = String::parseReal(*this);
    }
    void String::parse(int& val) const
    {
      val = String::parseInt(*this);
    }
    void String::parse(uint& val) const
    {
      val = String::parseUnsignedInt(*this);
    }
    void String::parse(bool& val) const
    {
      val = parseBool(*this);
    }
    void String::parse(Vector3& val) const
    {
      val = parseVector3(*this);
    }
    void String::parse(Matrix3& val) const
    {
      val = parseMatrix3(*this);
    }
    void String::parse(Matrix4& val) const
    {
      val = parseMatrix4(*this);
    }
    void String::parse(Quaternion& val) const
    {
      val = parseQuaternion(*this);
    }
    void String::parse(Color& val) const
    {
      val = parseColor(*this);
    }


    QStringList String::toQStringList(const StringArray& sa)
    {
        QStringList qlist;
        for(StringArray::const_iterator it = sa.begin() ; it!=sa.end() ; ++it)
        {
            qlist.push_back(it->c_str());
        }
        return qlist;
    }
}

#endif
