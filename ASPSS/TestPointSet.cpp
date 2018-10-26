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
#include "ExpeVector3.h"
#include "ExpeStringHelper.h"

#include "ExpePointSet.h"


using namespace Expe;

int main(int argc, char* argv[])
{
    std::cout << "Expe::Vector2 type = " << qMetaTypeId<Expe::Vector2>() << "\n";
    std::cout << "Expe::Vector3 type = " << qMetaTypeId<Expe::Vector3>() << "\n";
    std::cout << "Expe::Vector4 type = " << qMetaTypeId<Expe::Vector4>() << "\n";

//     Expe::SmartConvertManager::Initialize();

    PointSet* pPoints = new PointSet(PointSet::DefaultFormat);
    
    for (int i=0 ; i<10000 ; ++i)
    {
        PointSet::PointHandle pt = pPoints->append();
        pt.position() = Vector3(i,i+1,i+2);
        pt.radius() = 0.5*i;
        pt.normal() = 2*Vector3(i,i+1,i+2);
        pt.color() = Rgba::RED;
    }
    
    for (int i=0 ; i<10000 ; ++i)
    {
        PointSet::PointHandle pt = pPoints->at(i);
        std::cout << pt.position() << " , " << pt.radius() << " , " << pt.normal() << " , " << pt.color() << "\n";
    }
    
    // multiplicity test
    {
        PointSet* pPoints = new PointSet(PointSet::Attribute_position
            | PointSet::Attribute_radius
            | 3*PointSet::Attribute_normal
            | PointSet::Attribute_color);
    
        for (int i=0 ; i<100 ; ++i)
        {
            PointSet::PointHandle pt = pPoints->append();
            pt.position() = Vector3(i,i+1,i+2);
            pt.radius() = 0.5*i;
            pt.normal(0) = 2*Vector3(i,i+1,i+2);
            pt.normal(1) = 3*Vector3(i,i+1,i+2);
            pt.normal(2) = 4*Vector3(i,i+1,i+2);
            pt.color() = Rgba::RED;
        }
        
        for (int i=0 ; i<100 ; ++i)
        {
            PointSet::PointHandle pt = pPoints->at(i);
            std::cout << pt.position() << " , " << pt.radius() << " , "
                << pt.normal(0) << " , "
                << pt.normal(1) << " , "
                << pt.normal(2) << " , "
                << pt.color() << "\n";
        }
    }
    
    // test const
    {
//         const PointSet* pConstPoints = pPoints;
//         PointSet::PointHandle pt = pConstPoints->at(0);
//         Vector3 p = pt.position();
//         pt.position() = p;
    }

    return 0;
}
