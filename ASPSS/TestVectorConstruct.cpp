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
#include "ExpeString.h"


using namespace Expe;


template <typename T> class VVector2Implementation
{
public:

    typedef T ScalarT;
    typedef typename Expe::TypeInfo<ScalarT>::FloatingPointType FloatT;
    typedef VVector2Implementation<ScalarT> Vector2T;

    ScalarT x, y;
    
public:

    inline VVector2Implementation() {}

    inline VVector2Implementation(ScalarT _x, ScalarT _y)
        : x(_x), y(_y)
    {}

    inline VVector2Implementation(const ScalarT* const coords)
        : x(coords[0]), y(coords[1])
    {}

    inline VVector2Implementation(const Vector2T& v)
        : x(v.x), y(v.y)
    {}
    
    inline Vector2T& operator = (const Vector2T& v)
    {
        x = v.x;
        y = v.y;
        return *static_cast<Vector2T*>(this);
    }
    
};



class VVector2 : public VVector2Implementation<Real>
{

typedef VVector2Implementation<Real> Super;

public:
    
    inline VVector2()
    {}

    inline VVector2(Real _x, Real _y)
        : Super(_x,_y)
    {}
    
    inline VVector2(const Real* const coords)
        : Super(coords)
    {}
    
    inline VVector2(const VVector2& v)
        : Super(v)
    {
    }
};

class VVector2i : public VVector2Implementation<Int>
{
typedef VVector2Implementation<Int> Super;

public:
    inline VVector2i()
    {}

    inline VVector2i(int _x, int _y)
//         : Super(_x,_y)
    {x=_x;y=_y;}
//     
//     inline VVector2i(const int* const coords)
//         : Super(coords)
//     {}
//     
//     inline VVector2i(const Vector2i& vec)
//         : Super(vec)
//     {}

};

int main(int argc, char* argv[])
{
    std::cout << "Expe::Vector2 type = " << qMetaTypeId<Expe::Vector2>() << "\n";
    std::cout << "Expe::Vector3 type = " << qMetaTypeId<Expe::Vector3>() << "\n";
    std::cout << "Expe::Vector4 type = " << qMetaTypeId<Expe::Vector4>() << "\n";

//     Vector3 vf0,vf1;
//     Vector3i vfi0,vfi1;
    
    // does not compile 
    // vf0 = vfi0;
    
    // does not compmile
    // vfi0 = vf0;
    
//     {Vector3 aux(vfi0);}
    
//     {Vector3i aux(vf0);}
    
    
    
    VVector2 vf0,vf1;
    VVector2i vfi0,vfi1;
    
    // does not compile 
    vf0 = vfi0;
    
    // does not compmile
    vfi0 = vf0;
    
    {VVector2 aux(vfi0);}
    
    {VVector2i aux(vf0);}

    return 0;
}
