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



#include "ExpeVector2.h"
#include "ExpeStringHelper.h"
#include <QMetaProperty>
#include "ExpeTimer.h"

#include "ExpeSmartCast.h"


namespace Expe {

void testsmart_cast(void)
{
    QVariant v1 = QVariant::fromValue(10);
    QVariant v2 = QVariant::fromValue(Vector2(1,2));
    QVariant v3 = QVariant::fromValue(Vector3(3,4,5));
    QVariant v4 = QVariant::fromValue(Vector4(6,7,8,9));
    
    std::cout << smart_cast<Vector3>(Vector2(1,2)) << "\n";
    
    std::cout << "\n\n===== testsmart_cast =====\n\n";
    std::cout << "v1         = " << v1.value<Real>() << "\n";
    std::cout << "v1 as vec2 = " << smart_cast<Vector2>(v1) << "\n";
    std::cout << "v1 as vec3 = " << smart_cast<Vector3>(v1) << "\n";
    std::cout << "v1 as vec4 = " << smart_cast<Vector4>(v1) << "\n";
    std::cout << "\n";
    std::cout << "v2         = " << v2.value<Vector2>() << "\n";
    std::cout << "v2 as vec2 = " << smart_cast<Vector2>(v2) << "\n";
    std::cout << "v2 as vec3 = " << smart_cast<Vector3>(v2) << "\n";
    std::cout << "v2 as vec4 = " << smart_cast<Vector4>(v2) << "\n";
    std::cout << "\n";
    std::cout << "v3         = " << v3.value<Vector3>() << "\n";
    std::cout << "v3 as vec2 = " << smart_cast<Vector2>(v3) << "\n";
    std::cout << "v3 as vec3 = " << smart_cast<Vector3>(v3) << "\n";
    std::cout << "v3 as vec4 = " << smart_cast<Vector4>(v3) << "\n";
    std::cout << "\n";
    std::cout << "v4         = " << v4.value<Vector4>() << "\n";
    std::cout << "v4 as vec2 = " << smart_cast<Vector2>(v4) << "\n";
    std::cout << "v4 as vec3 = " << smart_cast<Vector3>(v4) << "\n";
    std::cout << "v4 as vec4 = " << smart_cast<Vector4>(v4) << "\n";
    std::cout << "\n=============================\n\n";
    
    
//     std::cout << "\n\n===== testsmart_castMap =====\n\n";
//     std::cout << "v1         = " << v1.value<Real>() << "\n";
//     std::cout << "v1 as vec2 = " << smart_castMap<Vector2>(v1) << "\n";
//     std::cout << "v1 as vec3 = " << smart_castMap<Vector3>(v1) << "\n";
//     std::cout << "v1 as vec4 = " << smart_castMap<Vector4>(v1) << "\n";
//     std::cout << "\n";
//     std::cout << "v2         = " << v2.value<Vector2>() << "\n";
//     std::cout << "v2 as vec2 = " << smart_castMap<Vector2>(v2) << "\n";
//     std::cout << "v2 as vec3 = " << smart_castMap<Vector3>(v2) << "\n";
//     std::cout << "v2 as vec4 = " << smart_castMap<Vector4>(v2) << "\n";
//     std::cout << "\n";
//     std::cout << "v3         = " << v3.value<Vector3>() << "\n";
//     std::cout << "v3 as vec2 = " << smart_castMap<Vector2>(v3) << "\n";
//     std::cout << "v3 as vec3 = " << smart_castMap<Vector3>(v3) << "\n";
//     std::cout << "v3 as vec4 = " << smart_castMap<Vector4>(v3) << "\n";
//     std::cout << "\n";
//     std::cout << "v4         = " << v4.value<Vector4>() << "\n";
//     std::cout << "v4 as vec2 = " << smart_castMap<Vector2>(v4) << "\n";
//     std::cout << "v4 as vec3 = " << smart_castMap<Vector3>(v4) << "\n";
//     std::cout << "v4 as vec4 = " << smart_castMap<Vector4>(v4) << "\n";
//     std::cout << "\n=============================\n\n";
}

void testsmart_castPerf(void)
{
    QVariant v1 = QVariant::fromValue(10);
    QVariant v2 = QVariant::fromValue(Vector2(1,2));
    QVariant v3 = QVariant::fromValue(Vector3(3,4,5));
    QVariant v4 = QVariant::fromValue(Vector4(6,7,8,9));
    
    Vector2 av2;
    Vector3 av3;
    Vector4 av4;
    
    av2 = Vector2(0,0);
    av3 = Vector3(0,0,0);
    av4 = Vector4(0,0,0);
    
    std::cout << smart_cast<Vector3>(Vector2(1,2)) << "\n";
    
    Timer timer;
    timer.reset();
    timer.start();
    std::cout << "\n\n===== testsmart_cast =====\n\n";
    for (uint i=0 ; i<10000000 ; ++i)
    {
//         asm ("#BEGIN LOOP");
        av2 += smart_cast<Vector2>(v1);
        av3 += smart_cast<Vector3>(v1);
        av4 += smart_cast<Vector4>(v1);
        
        av2 += smart_cast<Vector2>(v2);
        av3 += smart_cast<Vector3>(v2);
        av4 += smart_cast<Vector4>(v2);
        
        av2 += smart_cast<Vector2>(v3);
        av3 += smart_cast<Vector3>(v3);
        av4 += smart_cast<Vector4>(v3);
        
        av2 += smart_cast<Vector2>(v4);
        av3 += smart_cast<Vector3>(v4);
        av4 += smart_cast<Vector4>(v4);
//         asm ("#END LOOP");
    }
    timer.stop();
    std::cout << "Time = " << timer.value() << "\n";
    std::cout << "\n=============================\n";
    std::cout << av2 << ", " << av3 << ", " << av4 << "\n";
    
    float   tv1 = 10;
    Vector2 tv2 = Vector2(1,2);
    Vector3 tv3 = Vector3(3,4,5);
    Vector4 tv4 = Vector4(6,7,8,9);
    
    
    av2 = Vector2(0,1);
    av3 = Vector3(2,3,4);
    av4 = Vector4(5,6,7);
    
    timer.reset();
    timer.start();
    std::cout << "\n===== testDirect =====\n\n";
    for (uint i=0 ; i<10000000 ; ++i)
    {
//         asm ("#BEGIN LOOP2");
        av2 += smart_cast<Vector2>(tv1);
        av3 += smart_cast<Vector3>(tv1);
        av4 += smart_cast<Vector4>(tv1);
        
        av2 += smart_cast<Vector2>(tv2);
        av3 += smart_cast<Vector3>(tv2);
        av4 += smart_cast<Vector4>(tv2);
        
        av2 += smart_cast<Vector2>(tv3);
        av3 += smart_cast<Vector3>(tv3);
        av4 += smart_cast<Vector4>(tv3);
        
        av2 += smart_cast<Vector2>(tv4);
        av3 += smart_cast<Vector3>(tv4);
        av4 += smart_cast<Vector4>(tv4);
//         av3 = av3 + tv3*av3.y + tv1;
//         asm ("#END LOOP2");
    }
    timer.stop();
    std::cout << "Time = " << timer.value() << "\n";
    std::cout << "\n=============================\n";
    
    std::cout << av2 << ", " << av3 << ", " << av4 << "\n";
}

}


using namespace Expe;

int main(int argc, char* argv[])
{
    std::cout << "Expe::Vector2 type = " << qMetaTypeId<Expe::Vector2>() << "\n";
    std::cout << "Expe::Vector3 type = " << qMetaTypeId<Expe::Vector3>() << "\n";
    std::cout << "Expe::Vector4 type = " << qMetaTypeId<Expe::Vector4>() << "\n";
    
    Expe::Vector2 v2(1,2);
    Expe::Real z0 = 1.23456789;
    Expe::Real z1 = 1.23456789;
    Expe::Real z2 = 1.23456789;
    Expe::Vector3 v3 = Expe::Vector3(v2);
    std::cout << v3.x << " " << v3.y << " " << v3.z /*<< " " << v3.w*/ << " " << ((float*)v2)[2] << "\n";
//     Expe::Vector4 v4 = Expe::Vector4(v2);
    
    Expe::SmartCastManager::Initialize();
    
    QVariant a = QVariant::fromValue<float>(12.45f);
    //a.convert(QVariant::Type(qMetaTypeId<Expe::Vector3>()));
    std::cout << a.value<Vector3>().x << "\n";

    Expe::testsmart_cast();
    Expe::testsmart_castPerf();
   

    return 0;
}
