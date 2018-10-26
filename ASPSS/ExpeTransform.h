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



#ifndef _ExpeTransform_h_
#define _ExpeTransform_h_

#include "ExpePrerequisites.h"

#include "ExpeVector3.h"
#include "ExpeVector4.h"
#include "ExpeQuaternion.h"
#include "ExpeMatrix4.h"

namespace Expe
{


struct Transform
{
    Transform(Matrix4& mat)
    {
        position = mat.getTranslation();
        orientation.fromRotationMatrix(mat.extract3x3Matrix());
        scale = mat.getScale();
    }
    Transform(void)
        : position(0., 0., 0.), scale(1., 1., 1.)
    {
    }

    inline Transform& operator = (const Matrix4& mat)
    {
        position = mat.getTranslation();
        orientation.fromRotationMatrix(mat.extract3x3Matrix());
        scale = mat.getScale();
        return *this;
    }

    inline Matrix4 asMatrix4(void) const
    {
        Matrix4 m;
        Matrix3 rot;
        orientation.toRotationMatrix(rot);
        m = Matrix4::IDENTITY;
        Matrix4 r, s;
        r = Matrix4::IDENTITY;
        s = Matrix4::IDENTITY;
        m.setTranslation(position);
        r.setRotation(rot);
        s.setScale(scale);
        m = m * r * s;
        return m;
    }

    inline void setFromLerp(Real s, const Transform& t1,  const Transform& t2)
    {
        position = t1.position.lerp(s, t2.position);
        scale = t1.scale.lerp(s, t2.scale);
        orientation = Quaternion::Slerp(s, t1.orientation, t2.orientation);
    }

    static const Transform IDENTITY;

public:
    Quaternion orientation;
    Vector3 position;
    Vector3 scale;
};


}

#endif

