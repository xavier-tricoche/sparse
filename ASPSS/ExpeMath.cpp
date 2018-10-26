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


 
#include "ExpeMath.h"
#include <math.h>
#include <iostream>
#include "ExpeStaticInitializer.h"
#include "ExpeLogManager.h"

#include <QMetaType>

Q_DECLARE_METATYPE(Expe::Real);

namespace Expe
{

namespace Math
{

Real* msCosTable = 0;
Real* msSinTable = 0;
Real* msTanTable = 0;

Real* msACosTable = 0;
Real* msATanTable = 0;
Real* msASinTable = 0;

uint* msSqrtTable = 0;

void buildTables()
{
    LOG_DEBUG_MSG(5,"buildTrigTables");
    //msTrigTableSize = tableSize;
    //msTrigTableFactor = msTrigTableSize / Math::TWO_PI;
    
//     msATrigTableOffset = -msTrigTableSize/2;
//     msATrigTableFactor = 0.5*msTrigTableSize;
    
    msCosTable = new Real[EXPE_TRIGO_TAB_SIZE];
    msSinTable = new Real[EXPE_TRIGO_TAB_SIZE];
    msTanTable = new Real[EXPE_TRIGO_TAB_SIZE];
    msACosTable = new Real[EXPE_TRIGO_TAB_SIZE + 2*EXPE_ATRIGO_GUARD_SIZE];
    msASinTable = new Real[EXPE_TRIGO_TAB_SIZE + 2*EXPE_ATRIGO_GUARD_SIZE];
    msATanTable = new Real[2*EXPE_ATAN_TAB_SIZE];
    
    
    // Build trig lookup tables
    // Could get away with building only PI sized Sin table but simpler this
    // way. Who cares, it'll ony use an extra 8k of memory anyway and I like
    // simplicity.
    Real angle;
    for (int i = 0; i < EXPE_TRIGO_TAB_SIZE; ++i)
    {
        angle = Math::TWO_PI * i / (Real)(EXPE_TRIGO_TAB_SIZE);
        msCosTable[i] = cos(angle);
        msSinTable[i] = sin(angle);
        msTanTable[i] = tan(angle);
    }
    
    Real x;
    for (int i = 0; i < EXPE_TRIGO_TAB_SIZE + 2*EXPE_ATRIGO_GUARD_SIZE; ++i)
    {
        //x = (2.*Real(i) / Real(msTrigTableSize)) - 1.;
        x = (Real)(i-EXPE_TRIGO_TAB_SIZE2-EXPE_ATRIGO_GUARD_SIZE)/(Real)(EXPE_TRIGO_TAB_SIZE2);
        //x = (Real)(i-2048-256)/2048.f;
        if(x<-1.) x = -1.;
        if(x>1.) x = 1.;
        msACosTable[i] = acos(x);
        msASinTable[i] = asin(x);
    }

    for(int i=-EXPE_ATAN_TAB_SIZE ; i<EXPE_ATAN_TAB_SIZE ; ++i)
    {
        x = (Real)i/EXPE_ATAN_SCALE;
        msATanTable[EXPE_ATAN_TAB_SIZE + i] = atan(x);
    }

    msSqrtTable = new uint[0x10000];

    typedef union _FastSqrtUnion
    {
        float f;
        uint i;
    } FastSqrtUnion;
    
    FastSqrtUnion s;
    
    for (uint i = 0; i <= 0x7FFF; i++)
    {
        // Build a float with the bit pattern i as mantissa
        //  and an exponent of 0, stored as 127
        
        s.i = (i << 8) | (0x7F << 23);
        s.f = (float)sqrt(s.f);
        
        // Take the square root then strip the first 7 bits of
        //  the mantissa into the table
        
        msSqrtTable[i + 0x8000] = (s.i & 0x7FFFFF);
        
        // Repeat the process, this time with an exponent of 1, 
        //  stored as 128
        
        s.i = (i << 8) | (0x80 << 23);
        s.f = (float)sqrt(s.f);
        
        msSqrtTable[i] = (s.i & 0x7FFFFF);
    }
}



EXPE_INIT_CODE(Math) {
     qRegisterMetaType<Real>("Real");
     buildTables();
     
//      std::cout << "- QMetaType::type(\"Real\") = " << QMetaType::type("Real") << "\n";
//      std::cout << "- qMetaType<Real>()         = " << qMetaTypeId<Real>() << "\n";
}

}

}
