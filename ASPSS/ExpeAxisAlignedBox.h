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



#ifndef _ExpeAxisAlignedBox_h_ 
#define _ExpeAxisAlignedBox_h_

#include "ExpePrerequisites.h"

#include "ExpeVector3.h"

namespace Expe
{

class AxisAlignedBox
{
public:

    AxisAlignedBox(const Vector3& p)
        : mMinimum(p), mMaximum(p)
    {
    }
    
    AxisAlignedBox(const Vector3& min, const Vector3& max)
        : mMinimum(min), mMaximum(max)
    {
    }
    
    AxisAlignedBox()
        : mMinimum(1e99, 1e99, 1e99), mMaximum(-1e99, -1e99, -1e99) /*, mIsNull(true)*/
    {
    }
    
    ~AxisAlignedBox()
    {
    }

    inline bool isEmpty(void) const
    {
        return !(mMaximum>mMinimum);
    }
    
    inline bool contains(const Vector3& p) const
    {
        return (mMinimum<=p) && (p<=mMaximum);
    }
    
    inline bool contains(const AxisAlignedBox& aab) const
    {
        return (mMinimum<=aab.min()) && (aab.max()<=mMaximum);
    }

    inline void extend(const Vector3& p)
    {
        mMinimum.makeFloor(p);
        mMaximum.makeCeil(p);
    }
    
    inline void extend(const AxisAlignedBox& aab)
    {
        mMinimum.makeFloor(aab.mMinimum);
        mMaximum.makeCeil(aab.mMaximum);
    }
    inline void extend(const Vector3& min, const Vector3& max)
    {
        mMinimum.makeFloor(min);
        mMaximum.makeCeil(max);
    }
    
    inline void clamp(const AxisAlignedBox& aab)
    {
        mMinimum.makeCeil(aab.mMinimum);
        mMaximum.makeFloor(aab.mMaximum);
    }
    inline void clamp(const Vector3& min, const Vector3& max)
    {
        mMinimum.makeCeil(min);
        mMaximum.makeFloor(max);
    }

    inline void setMinimum(const Vector3& min)
    {
        mMinimum = min;
    }

    inline const Vector3& getMinimum(void) const
    {
        return mMinimum;
    }
    inline const Vector3& min(void) const
    {
        return mMinimum;
    }
    inline Vector3& min(void)
    {
        return mMinimum;
    }

    
    inline void setMaximum(const Vector3& max)
    {
        mMaximum = max;
    }

    inline const Vector3& getMaximum(void) const
    {
        return mMaximum;
    }
    inline const Vector3& max(void) const
    {
        return mMaximum;
    }
    inline Vector3& max(void)
    {
        return mMaximum;
    }
    
    /** Returns the distance to the box (0 if the point is inside)
    */
    inline Real distanceTo(const Vector3& p) const
    {
        return Math::Sqrt(squaredDistanceTo(p));
    }
    
    inline Real squaredDistanceTo(const Vector3& p) const
    {
        Real dist2 = 0.;
        Real aux;
        for (uint k=0 ; k<3 ; ++k)
        {
            if ( (aux = (p[k]-mMinimum[k]))<0. )
                dist2 += aux*aux;
            else if ( (aux = (mMaximum[k]-p[k]))<0. )
                dist2 += aux*aux;
        }
        return dist2;
    }

protected:

    Vector3 mMinimum, mMaximum;
    /*bool mIsNull;*/
};

}

#endif

