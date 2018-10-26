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
#include "ExpeVector2.h"
#include "ExpeVector3.h"
#include "ExpeVector4.h"
#include "ExpeStringHelper.h"

namespace Expe
{


// template <> inline Vector2 smart_cast<>(const Real& valueIn, bool* ok) {if(ok) *ok=true; return Vector2(valueIn, valueIn);}
// template <> inline Vector2 smart_cast<>(const Vector3& v, bool* ok) {if(ok) *ok=true; return Vector2(v.x, v.y);}
// template <> inline Vector2 smart_cast<>(const Vector4& v, bool* ok) {if(ok) *ok=true; return Vector2(v.x, v.y);}
// 
// template <> inline Vector3 smart_cast<>(const Real& valueIn, bool* ok) {if(ok) *ok=true; return Vector3(valueIn, valueIn, valueIn);}
// template <> inline Vector3 smart_cast<>(const Vector2& v, bool* ok) {if(ok) *ok=true; return Vector3(v.x, v.y, 0.f);}
// template <> inline Vector3 smart_cast<>(const Vector4& v, bool* ok) {if(ok) *ok=true; return Vector3(v.x, v.y, v.z);}
// 
// template <> inline Vector4 smart_cast<>(const Real& valueIn, bool* ok) {if(ok) *ok=true; return Vector4(valueIn, valueIn, valueIn, valueIn);}
// template <> inline Vector4 smart_cast<>(const Vector2& v, bool* ok) {if(ok) *ok=true; return Vector4(v.x, v.y, 0.f, 1.f);}
// template <> inline Vector4 smart_cast<>(const Vector3& v, bool* ok) {if(ok) *ok=true; return Vector4(v.x, v.y, v.z, 1.f);}
// 
// template <> inline Color smart_cast<>(const Rgba& i,    bool* ok) {if(ok) *ok=true; Color c; c.fromRGBA(i); return c;} // int to RGBA
// template <> inline Color smart_cast<>(const Real& val,  bool* ok) {if(ok) *ok=true; return Color(val, val, val, 1.f);} // luminance
// template <> inline Color smart_cast<>(const Vector2& v, bool* ok) {if(ok) *ok=true; return Color(v.x, v.x, v.x, v.y);} // luminance,alpha
// template <> inline Color smart_cast<>(const Vector3& v, bool* ok) {if(ok) *ok=true; return Color(v.x, v.y, v.z, 1.f);} // RGB
// template <> inline Color smart_cast<>(const Vector4& v, bool* ok) {if(ok) *ok=true; return Color(v.x, v.y, v.z, v.w);} // RGBA
// template <> inline Color smart_cast<>(const QColor&  c, bool* ok) {if(ok) *ok=true; return Color(c);} // auto


}

using namespace Expe;

void testQDebugStreamOperators(void)
{
    std::cout << "\n\ntestQDebugStreamOperators:\n";
    qDebug() << "QDEBUG: Vector4f(1.125, -9.25, 12.68) == " << Vector4f(1.125, -9.25, 12.68);
    qDebug() << "QDEBUG: Vector4i(1, -9, 12) == " << Vector4i(1, -9, 12);
    qDebug() << "QDEBUG: Vector3d(1.125, -9.25, 12.68) == " << Vector3d(1.125, -9.25, 12.68);
    qDebug() << "QDEBUG: Vector2f(1.125, -9.25) == " << Vector2f(1.125, -9.25);
    std::cout << "Done.\n\n";
}

void testStdStreamOperators(void)
{
    std::cout << "\n\ntestStdStreamOperators:\n";
    std::cout << "std::cout: Vector4f(1.125, -9.25, 12.68) == " << Vector4f(1.125, -9.25, 12.68) << "\n";
    std::cout << "std::cout: Vector4i(1, -9, 12) == " << Vector4i(1, -9, 12) << "\n";
    std::cout << "std::cout: Vector3d(1.125, -9.25, 12.68) == " << Vector3d(1.125, -9.25, 12.68) << "\n";
    std::cout << "std::cout: Vector2f(1.125, -9.25) == " << Vector2f(1.125, -9.25) << "\n";
    std::cout << "Done.\n\n";
}

void testVectorCast(void)
{
    std::cout << "\n\ntestVectorCast:\n";
    
    Vector4f v4f1(1.125, -9.25, 12.68);
    Vector2f v2f1(1.125, -9.25);
    Vector4d v4d1 = vector_cast<Vector4d>(v4f1);
    Vector4f v4f2 = vector_cast<Vector4f>(v4f1);
    Vector3d v3d1 = vector_cast<Vector3d>(v4f1);
    Vector4i v4i1 = vector_cast<Vector4i>(v2f1);
    Vector4  v4f3 = vector_cast<Vector4>(v4f1);
    
    std::cout << v4f1 << " => " << v4d1 << "\n";
    std::cout << v4f1 << " => " << v4f2 << "\n";
    std::cout << v4f1 << " => " << v3d1 << "\n";
    std::cout << v2f1 << " => " << v4i1 << "\n";
    std::cout << 12.5 << " => " << vector_cast<Vector4f>(12.5) << "\n";
    std::cout << 12.5 << " => " << vector_cast<Vector3f>(12.5) << "\n";
    std::cout << 12.5 << " => " << vector_cast<Vector2f>(12.5) << "\n";

    if ( MTP::compare_type<Vector4f,Vector4d>::result)
        std::cout << "Vector4f==Vector4d\n";
    else
        std::cout << "Vector4f!=Vector4d\n";
    std::cout << "Done.\n\n";
}

int main(int argc, char* argv[])
{
    std::cout << "Expe::Vector2f type = " << qMetaTypeId<Expe::Vector2f>() << "\n";
    std::cout << "Expe::Vector2d type = " << qMetaTypeId<Expe::Vector2d>() << "\n";
    std::cout << "Expe::Vector3f type = " << qMetaTypeId<Expe::Vector3f>() << "\n";
    std::cout << "Expe::Vector3d type = " << qMetaTypeId<Expe::Vector3d>() << "\n";
    std::cout << "Expe::Vector4f type = " << qMetaTypeId<Expe::Vector4f>() << "\n";
    std::cout << "Expe::Vector4d type = " << qMetaTypeId<Expe::Vector4d>() << "\n";
    
    testStdStreamOperators();
    testQDebugStreamOperators();
    testVectorCast();
    
    // test a fun stuff:
    typedef GetVector<TypeInfo<Vector3f>::NofComponents+1,
        TypeInfo<TypeInfo<Vector4f>::ComponentType>::IntegerType>::Type MyNewVector4i;
    // check if we indeed get the Vector4i type:
    std::cout << qMetaTypeId<MyNewVector4i>() << " == " << qMetaTypeId<Expe::Vector4i>() << "\n";
    
    return 0;
}
