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



#ifndef _ExpePointSet_h_
#define _ExpePointSet_h_


#include "ExpeCore.h"
#include "ExpeAxisAlignedBox.h"
#include "ExpeSharedPointer.h"
#include "ExpeGeometryObject.h"
#include "ExpeVector2.h"
#include "ExpeVector3.h"
#include "ExpeColor.h"
#include "ExpeUberVector.h"

#ifdef EXPE_GPU_SUPPORT
#include "ExpeGpuUberVector.h"
#endif

namespace Expe
{

#ifndef DOXYGEN

class _PointSetBuiltinData : public UberVectorBaseT<_PointSetBuiltinData>
{
public:
    EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(Vector3f, position, 1);
    EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(float, radius, 2);
    EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(Vector3f, normal, 3);
    EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(Rgba, color, 4);
    EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(Vector2f, texcoord, 5);
    EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(ubyte, materialId, 6);
    EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(ubyte, flags, 7);
	EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(double, scalarradius, 8);
	EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(double, scalar, 9);
	EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(uint, siteid, 10);

    EXPE_UBERVECTOR_BEGIN_BUILTIN_ACCESSORS(16);

        inline EXPE_UBERVECTOR_GETTYPEREF_WITH_CONSTNESS(position) position (void) const
        {
            return *reinterpret_cast<EXPE_UBERVECTOR_GETTYPEPTR_WITH_CONSTNESS(position)>(this->mAddr);
        }

        EXPE_UBERVECTOR_BUILTIN_ACCESSOR(radius);
        EXPE_UBERVECTOR_BUILTIN_ACCESSOR(normal);
        EXPE_UBERVECTOR_BUILTIN_ACCESSOR(color);
        EXPE_UBERVECTOR_BUILTIN_ACCESSOR(texcoord);
        EXPE_UBERVECTOR_BUILTIN_ACCESSOR(materialId);
        EXPE_UBERVECTOR_BUILTIN_ACCESSOR(flags);
		EXPE_UBERVECTOR_BUILTIN_ACCESSOR(scalarradius)
		EXPE_UBERVECTOR_BUILTIN_ACCESSOR(scalar);
		EXPE_UBERVECTOR_BUILTIN_ACCESSOR(siteid);

    EXPE_UBERVECTOR_END_BUILTIN_ACCESSORS
};

#ifdef EXPE_GPU_SUPPORT
class PointSet : public GeometryObject, public GpuUberVectorT< _PointSetBuiltinData >
#else
class PointSet : public GeometryObject, public UberVectorT< _PointSetBuiltinData >
#endif
{
//Q_OBJECT
public:

    #ifdef EXPE_GPU_SUPPORT
    typedef GpuUberVectorT< _PointSetBuiltinData > Super;
    #else
    typedef UberVectorT< _PointSetBuiltinData > Super;
    #endif

    static const AttributeList DefaultFormat;

    PointSet(const AttributeList& al = DefaultFormat)
        : Super(al)
    {}
    
    PointSet(const Format& f)
        : Super(f)
    {}

    virtual ~PointSet()
    {}

    typedef ElementHandle PointHandle;
    typedef ConstElementHandle ConstPointHandle;

    PointSet* clone(void) const;
    AxisAlignedBox computeAABB(void) const;

protected:

};

#else

class PointSet : public Geometry
{
public:

    /// Default point set format
    static const AttributeList DefaultFormat;

    PointSet(const AttributeList& al = DefaultFormat);

    /** Allow to access the attributes of a point.
        A PointHandle has a behavior close a reference but the = operator behaves like with pointers.
        To copy the attributes of a point to another use the copyFrom method.
        \code
            PointSet::PointHandle pt;
            pt = pPoints->at(i);
            // pt is now a reference to the i-th element
            pt = pPoints->at(j);
            // pt is now a reference to the j-th element
            pt.copyFrom(pPoints->at(i));
            // the j-th element is now equal to the i-th
        \endcode
    */
    struct PointHandle
    {
        Vector3& position();
        Vector3& position(uint i);
        uint position_count() const;
        
        <attributetype>& <attributename>();
        <attributetype>& <attributename>(uint i);
        uint <attributename>_count() const;
        
        <attributetype>& attribute(const AttributeHandle& ah);
        <attributetype>& attribute(const AttributeHandle& ah,uint i);
        uint attribute_count(const AttributeHandle& ah) const;
    };
    
    /** See PointHandle.
    */
    struct ConstPointHandle
    {
        const Vector3& position() const;
        const Vector3& position(uint i) const;
        uint position_count() const;
        
        const <attributetype>& <attributename>() const;
        const <attributetype>& <attributename>(uint i) const;
        uint <attributename>_count() const;
        
        const <attributetype>& attribute(const AttributeHandle& ah) const;
        const <attributetype>& attribute(const AttributeHandle& ah,uint i) const;
        uint attribute_count(const AttributeHandle& ah) const;
    };


};

#endif

typedef SharedPointer<PointSet> PointSetPtr;
typedef ConstSharedPointer<PointSet> ConstPointSetPtr;

}

#endif

