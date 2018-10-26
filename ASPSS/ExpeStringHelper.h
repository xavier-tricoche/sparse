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



#ifndef _ExpeStringHelper_h_
#define _ExpeStringHelper_h_

#include "ExpePrerequisites.h"
#include <QString>
#include <QByteArray>
#include <iostream>

inline std::ostream& operator << (std::ostream& o, const QString& str)
{
    o << str.toLocal8Bit().data();
    return o;
}

inline std::ostream& operator << (std::ostream& o, const Expe::ByteString& str)
{
    o << str.data();
    return o;
}

namespace Expe
{

/** Template function to automatically convert a string to any vector type.
    In practice, this function can convert a QString a QByteArray (or an Expe::ByteString)
    to any types registered with the Expe::TypeInfo system, including Vector[234]*, Color,
    and even any registered scalar types (float, double, uint, etc.)
    The separator is assumed to be a coma. If the number of elements does not match then the parsing stops and false is returned.
    
*/
template <typename VectorT, class StringT>
inline bool parse_vector(const StringT& str, VectorT& vec)
{
    bool ok = true;
//     if (TypeInfo<VectorT>::NofComponents==1)
//     {
//         vec = str.toDouble(&ok);
//         return ok;
//     }
//     
    QList<StringT> elements = str.split(",");
    if (elements.size() != TypeInfo<VectorT>::NofComponents)
        return false;
    for (uint k=0 ; k<TypeInfo<VectorT>::NofComponents && ok ; ++k)
        vec[k] = elements[k].toDouble(&ok);
    return ok;
}

}

#if 0

typedef 

#ifdef EXPE_QT_SUPPORT
#include <qstringlist.h>
#endif

#include <string>
#include <sstream>

namespace Expe
{
    typedef std::string _StringBase;
}

namespace Expe {

    /** Wrapper object for String to allow DLL export.
        @note
            Derived from std::string.
    */
    class String : public _StringBase
    {
    public:
        typedef std::stringstream StrStreamType;
    public:
        /** Default constructor.
        */
        String() : _StringBase() {}

        String(const String& rhs) : _StringBase( static_cast< const _StringBase& >( rhs ) ) {}

        /** Copy constructor for std::string's.
        */
        String( const _StringBase& rhs ) : _StringBase( rhs ) {}

        /** Copy-constructor for C-style strings.
        */
        String( const char* rhs ) : _StringBase( rhs ) {}

        #ifdef EXPE_QT_SUPPORT
        #ifdef EXPE_USE_QT3
        String( const QString& qstr) : _StringBase( qstr.ascii() ) {}
        #else
        String( const QString& qstr) : _StringBase( qstr.toLocal8Bit() ) {}
        #endif
        
//         operator QString () const { return QString(c_str()); }
        
        #endif

        /** Used for interaction with functions that require the old C-style
            strings.
        */
        operator const char* () const { return c_str(); }

        /** Removes any whitespace characters, be it standard space or
            TABs and so on.
            @remarks
                The user may specify wether they want to trim only the
                beginning or the end of the String ( the default action is
                to trim both).
        */
        void trim( bool left = true, bool right = true );

        /** Returns a StringArray that contains all the substrings delimited
            by the characters in the passed <code>delims</code> argument.
            @param
                delims A list of delimiter characters to split by
            @param
                maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
                parameters is > 0, the splitting process will stop after this many splits, left to right.
        */
        StringArray split( const String& delims = "\t\n ", unsigned int maxSplits = 0) const;
        
        bool endsWith(const String& str) const
        {
            if(str.size()>this->size())
                return false;
            return this->substr(this->size()-str.size(),str.size()) == str;
        }
        
        bool beginsWith(const String& str) const
        {
            if(str.size()>this->size())
                return false;
            return this->substr(0,str.size()) == str;
        }

        /** Upper-cases all the characters in the string.
        */
        String toLowerCase( void );

        /** Lower-cases all the characters in the string.
        */
        String toUpperCase( void );

        /** Converts the contents of the string to a Real.
        @remarks
            Assumes the only contents of the string are a valid parsable Real. Defaults to  a
            value of 0.0 if conversion is not possible.
        */
        Real toReal(void) const;

        bool isEquiv(const String& str) const
        {
            /// \todo : make this more efficient !
            String str0 = *this;
            str0.toLowerCase();
            String str1 = str;
            str1.toLowerCase();
            return str0 == str1;
        }


        /** Template operator for appending another type into the string.
            @remarks
                Because this operator is templated, you can append any value into a string as
                long as there is an operator<<(std::basic_iostream, type) or similar method defined somewhere.
                All the primitive types have this already, and many of the Expe types do too (see Vector3
                for an example).
        */
        template< typename T > String& operator << (T value)
        {
            // Create stringstream based on *this contents
            StrStreamType sstr;
            sstr.str(*this);
            // Seek to end
            sstr.seekp(0, std::ios_base::end);
            // Write
            sstr << value;
            // Assign back
            *this = _StringBase( sstr.str() );

            return *this;
        }

        bool appendFile(const String& filename);

        static const String EMPTY;

        //inline friend Vector3 operator * ( Real fScalar, const Vector3& rkVector )

//         template< typename T > String operator + ( T value )
//         {
//             return *this + String::toString( value );
//         }

    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    public:

        //--------------------------------------------------------------------------------
        // parsing
        //--------------------------------------------------------------------------------

        void parse(Real& val) const;
        void parse(int& val) const;
        void parse(uint& val) const;
        void parse(bool& val) const;
        void parse(Vector3& val) const;
        void parse(Matrix3& val) const;
        void parse(Matrix4& val) const;
        void parse(Quaternion& val) const;
        void parse(Color& val) const;
//         template< typename T > void parse(T& val) const
//         {
//           std::cerr << "String::parse() : unable to parse this type of data : " << typeid(T).name() << std::endl;
//           std::cerr << "String::parse() : return the default value, you probaly should write your own parsing methods !" << std::endl;
//           val = T();
//         }

        //--------------------------------------------------------------------------------
        // static "type" to String conversion
        //--------------------------------------------------------------------------------


        /** Converts a Real to a String. Trivial format, full precision.*/
        static String toString(Real val);
        /** Converts a Real to a String. Trivial format, user precision.*/
        static String toString(Real val, int precision);
        /** Converts an int to a String. */
        static String toString(int val);
        /** Converts an int to a String. */
        static String toString(unsigned int val);
        /** Converts a long to a String. */
        static String toString(long val);
        /** Converts a long to a String. */
        static String toString(unsigned long val);
        /** Converts a boolean to a String. */
        static String toString(bool val);
        /** Converts a Vector3 to a String.
        @remarks
            Format is "x y z" (i.e. 3x Real values, space delimited)
        */
        static String toString(const Vector3& val);
        /** Converts a Matrix3 to a String.
        @remarks
            Format is "00 01 02 10 11 12 20 21 22" where '01' means row 0 column 1 etc.
        */
        static String toString(const Matrix3& val);
        /** Converts a Matrix4 to a String.
        @remarks
            Format is "00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33" where
            '01' means row 0 column 1 etc.
        */
        static String toString(const Matrix4& val);
        /** Converts a Quaternion to a String.
        @remarks
            Format is "x y z w" (i.e. 4x Real values, space delimited)
        */
        static String toString(const Quaternion& val);
        /** Converts a Color to a String.
        @remarks
            Format is "r g b a" (i.e. 4x Real values, space delimited).
        */
        static String toString(const Color& val);






        //--------------------------------------------------------------------------------
        // static String to "type" conversion
        //--------------------------------------------------------------------------------

        /** Converts a String to a Real.
        @returns
            0.0 if the value could not be parsed, otherwise the Real version of the String.
        */
        static Real parseReal(const String& val);
        /** Converts a String to a whole number.
        @returns
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static int parseInt(const String& val);
        /** Converts a String to a whole number.
        @returns
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static unsigned int parseUnsignedInt(const String& val);
        /** Converts a String to a whole number.
        @returns
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static long parseLong(const String& val);
        /** Converts a String to a whole number.
        @returns
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static unsigned long parseUnsignedLong(const String& val);
        /** Converts a String to a boolean.
        @remarks
            Accepts 'true' or 'false' as input.
        */
        static bool parseBool(const String& val);
        /** Parses a Vector3 out of a String.
        @remarks
            Format is "x y z" ie. 3 Real components, space delimited. Failure to parse returns
            Vector3::ZERO.
        */
        static Vector3 parseVector3(const String& val);
        /** Parses a Matrix3 out of a String.
        @remarks
            Format is "00 01 02 10 11 12 20 21 22" where '01' means row 0 column 1 etc.
            Failure to parse returns Matrix3::IDENTITY.
        */
        static Matrix3 parseMatrix3(const String& val);
        /** Parses a Matrix4 out of a String.
        @remarks
            Format is "00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33" where
            '01' means row 0 column 1 etc. Failure to parse returns Matrix4::IDENTITY.
        */
        static Matrix4 parseMatrix4(const String& val);
        /** Parses a Quaternion out of a String.
        @remarks
            Format is "x y z w" (i.e. 4x Real values, space delimited).
            Failure to parse returns Quaternion::IDENTITY.

        */
        static Quaternion parseQuaternion(const String& val);
        /** Parses a Color out of a String.
        @remarks
            Format is "r g b a" (i.e. 4x Real values, space delimited), or "r g b" which implies
            an alpha value of 1.0 (opaque). Failure to parse returns ColourValue::Black.
        */
        static Color parseColor(const String& val);

	#ifdef EXPE_QT_SUPPORT
        static QStringList toQStringList(const StringArray& sa);
	#endif
	
	/** Build a StringArray from the key value a map.
	*/
	template <class Map> static StringArray asStringArray(const Map& m)
	{
	   StringArray strList;
	   for(typename Map::const_iterator it=m.begin() ; it!=m.end() ; ++it)
	   {
	       strList.push_back(it->first);
	   }
	   return strList;
	}
    };

}

#include <qmetatype.h>
Q_DECLARE_METATYPE(Expe::String);

#endif


#endif

