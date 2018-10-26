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



#include "ExpeCore.h"
#include "ExpeTimer.h"
#include "ExpePointSet.h"
#include <iostream>
#include <vector>
#include <QVector>

using namespace Expe;


/** Good to know, after some experiments the std::vector<>::at(uint) operator is 2 times slower than [] !
    Then if correctly write the IndirectDirect class using (*pData)[i] to implement at, then we get
    almost the same perf:
        Direct   :       time = 1.05875
        QDirect  :       time = 1.22557
        Indirect :       time = 1.11091
        QIndirect :      time = 1.25115
        IIndirect:       time = 1.12693
    For QVector, the at operator is much faster (more than 2 times) since it is const it never check if
    a deep copy has to be performed. Even with at, QVector is still a bit slower.
*/

struct Direct
{
    void reserve(uint n) {data.reserve(n);}
    inline int& at(uint i) {return data[i];}
    inline void push_back(int val) {data.push_back(val);}
    std::vector<int> data;
};

struct UberDirect
{
    UberDirect() : data(PointSet::Attribute_color) {}
    void reserve(uint n) {data.reserve(n);}
    inline uint& at(uint i) {return reinterpret_cast<uint&>(data[i].color());}
    inline void push_back(int val) {data.append(); data.back().color() = val;}
    PointSet data;
};

struct QDirect
{
    void reserve(uint n) {data.reserve(n);}
    inline int at(uint i) {return data.at(i);}
    inline void push_back(int val) {data.push_back(val);}
    QVector<int> data;
};

struct IndirectDirect
{
    IndirectDirect() : pData(new std::vector<int>()) {}
    void reserve(uint n) {pData->reserve(n);}
    inline int& at(uint i) {return (*pData)[i];}
    inline void push_back(int val) {pData->push_back(val);}
    std::vector<int>* pData;
};

struct QIndirectDirect
{
    QIndirectDirect() : pData(new QVector<int>()) {}
    void reserve(uint n) {pData->reserve(n);}
    inline int at(uint i) {return pData->at(i);}
    inline void push_back(int val) {pData->push_back(val);}
    QVector<int>* pData;
};

struct IndirectIndirectDirect
{
    IndirectIndirectDirect() : pData(new Direct) {}
    void reserve(uint n) {pData->reserve(n);}
    inline int& at(uint i) {return pData->at(i);}
    inline void push_back(int val) {pData->push_back(val);}
    Direct* pData;
};

/* some results:
*/

template <class ArrayT>
void testArray(uint n)
{
    Timer timer;
    timer.reset();timer.start();
    uint overall = 0;
    for (uint k=0 ; k<100 ; ++k)
    {
        ArrayT array;
        array.reserve(n);
        for (uint i=0 ; i<n ; )
        {
            array.push_back(i++); array.push_back(i++); array.push_back(i++); array.push_back(i++);
            array.push_back(i++); array.push_back(i++); array.push_back(i++); array.push_back(i++);
            array.push_back(i++); array.push_back(i++); array.push_back(i++); array.push_back(i++);
            array.push_back(i++); array.push_back(i++); array.push_back(i++); array.push_back(i++);
        }
        uint count=0;
        for (uint k=0 ; k<100 ; ++k)
        {
            for (uint i=0 ; i<n ; )
            {
                count += array.at(i++); count += array.at(i++); count += array.at(i++); count += array.at(i++);
                count += array.at(i++); count += array.at(i++); count += array.at(i++); count += array.at(i++);
                count += array.at(i++); count += array.at(i++); count += array.at(i++); count += array.at(i++);
                count += array.at(i++); count += array.at(i++); count += array.at(i++); count += array.at(i++);
            }
        }
        overall += count;
    }
    timer.stop();
    std::cout << "\t time = " << timer.value() << "(" << overall << ")\n";
}

template <class ArrayT>
int testArrayI(uint n)
{
    ArrayT* array = new ArrayT();
    array->reserve(n);
    for (uint i=0 ; i<n ; )
    {
        array->push_back(i++); array->push_back(i++); array->push_back(i++); array->push_back(i++);
        array->push_back(i++); array->push_back(i++); array->push_back(i++); array->push_back(i++);
        array->push_back(i++); array->push_back(i++); array->push_back(i++); array->push_back(i++);
        array->push_back(i++); array->push_back(i++); array->push_back(i++); array->push_back(i++);
    }
    uint count=0;
    for (uint k=0 ; k<100 ; ++k)
    {
        for (uint i=0 ; i<n ; )
        {
            count += array->at(i++); count += array->at(i++); count += array->at(i++); count += array->at(i++);
            count += array->at(i++); count += array->at(i++); count += array->at(i++); count += array->at(i++);
            count += array->at(i++); count += array->at(i++); count += array->at(i++); count += array->at(i++);
            count += array->at(i++); count += array->at(i++); count += array->at(i++); count += array->at(i++);
        }
    }
    return count;
}


int main(int argc, char* argv[])
{
    for (;;)
    {
        std::cout << "Direct   : ";
        testArray<Direct>(160000);
        
        std::cout << "QDirect  : ";
        testArray<QDirect>(160000);
        
        std::cout << "Indirect : ";
        testArray<IndirectDirect>(160000);
        
        std::cout << "QIndirect : ";
        testArray<QIndirectDirect>(160000);
        
        std::cout << "UberDirect: ";
        testArray<UberDirect>(160000);
        
        std::cout << "IIndirect: ";
        testArray<IndirectIndirectDirect>(160000);
        
        std::cout << "\n";
    }
    
    return 0;
}
