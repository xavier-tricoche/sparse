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



#include <string>
#include <QString>
#include <QByteArray>
#include "ExpeTimer.h"
#include <map>
#include <QMap>
#include <vector>
#include <iostream>
#include "ExpeStringHelper.h"

using namespace Expe;

/* some results:
    QString   :     Time = 1.92505
    StdString :     Time = 2.1861
    QByteArray:     Time = 2.19451
*/

template <class StringMapT, class StringT>
inline void insertInMap(StringMapT& theMap, const StringT& key, const StringT& value)
{
    typename StringMapT::iterator it=theMap.find(key);
    if (it==theMap.end())
        theMap[key] = value;
}

template <class StringMapT, class StringT>
inline const StringT& findInMap(StringMapT& theMap, const StringT& key)
{
    typename StringMapT::iterator it=theMap.find(key);
    if (it!=theMap.end())
        return it->second;
//         return *it;
    return key;
}

template <class StringT>
void testMapStr(void)
{
    typedef std::map<StringT,StringT> StringMap;
//     typedef QMap<StringT,StringT> StringMap;
    StringMap aMap;
    Timer timer;
    timer.start();
    
    typename std::vector<StringT> strList;
    strList.push_back("September"); strList.push_back("May_friday"); strList.push_back("October_wednesfay"); strList.push_back("June_sahtuday");
    strList.push_back("March_wednesday"); strList.push_back("March"); strList.push_back("March_wednesday"); strList.push_back("March_wednesday");
    strList.push_back("March_wednesday"); strList.push_back("March_wednesdry"); strList.push_back("Decemyer_friday"); strList.push_back("August_monday");
    strList.push_back("October"); strList.push_back("March"); strList.push_back("June_satyrday"); strList.push_back("February_tuesday");
    
    for (int k=0 ; k<1000 ; ++k)
    {
        insertInMap<StringMap,StringT>(aMap,"January","1");
        insertInMap<StringMap,StringT>(aMap,"February","2");
        insertInMap<StringMap,StringT>(aMap,"March","3");
        insertInMap<StringMap,StringT>(aMap,"April","4");
        insertInMap<StringMap,StringT>(aMap,"May","5");
        insertInMap<StringMap,StringT>(aMap,"June","6");
        insertInMap<StringMap,StringT>(aMap,"Jully","7");
        insertInMap<StringMap,StringT>(aMap,"August","8");
        insertInMap<StringMap,StringT>(aMap,"September","9");
        insertInMap<StringMap,StringT>(aMap,"October","10");
        insertInMap<StringMap,StringT>(aMap,"Novenber","11");
        insertInMap<StringMap,StringT>(aMap,"December","12");
        
        insertInMap<StringMap,StringT>(aMap,"January_monday","1");
        insertInMap<StringMap,StringT>(aMap,"February_tuesday","2");
        insertInMap<StringMap,StringT>(aMap,"March_wednesday","3");
        insertInMap<StringMap,StringT>(aMap,"April_thursday","4");
        insertInMap<StringMap,StringT>(aMap,"May_friday","5");
        insertInMap<StringMap,StringT>(aMap,"June_saturday","6");
        insertInMap<StringMap,StringT>(aMap,"Jully_sunday","7");
        insertInMap<StringMap,StringT>(aMap,"August_monday","8");
        insertInMap<StringMap,StringT>(aMap,"September_tuesday","9");
        insertInMap<StringMap,StringT>(aMap,"October_wednesday","10");
        insertInMap<StringMap,StringT>(aMap,"Novenber_thursday","11");
        insertInMap<StringMap,StringT>(aMap,"December_friday","12");
    }

    StringT value;
    for (int k=0 ; k<200000 ; ++k)
    {
        value = "";
        for (int i=0 ; i<strList.size() ; ++i)
            value += findInMap<StringMap,StringT>(aMap, strList[i]);
    }
    
    timer.stop();
    std::cout << "\tTime = " << timer.value() << "\n";
}


int main(int argc, char* argv[])
{
    for (;;)
    {
        std::cout << "QString   : "; testMapStr<QString>();
        std::cout << "StdString : "; testMapStr<std::string>();
        std::cout << "QByteArray: "; testMapStr<QByteArray>();
    }
    
    return 0;
}
