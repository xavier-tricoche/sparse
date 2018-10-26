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



#ifndef _ExpeMlsSurface_h_
#define _ExpeMlsSurface_h_

#include "ExpeImplicitSurface.h"
#include "ExpePointSet.h"
#include "ExpeQuickManager.h"
#include "ExpeNeighborhood.h"
#include "ExpeWeightingFunction.h"

namespace Expe
{

/** Abstract class to define meshless Moving Least Squares surfaces.
*/

class MlsSurface : public ImplicitSurface
{
//Q_OBJECT

//Q_PROPERTY(float ProjectionAccuracy READ getProjectionAccuracy WRITE setProjectionAccuracy DESIGNABLE true STORED true);
//Q_PROPERTY(uint MaxNofProjectionIterations READ getMaxNofProjectionIterations WRITE setMaxNofProjectionIterations DESIGNABLE true STORED true);

public:

    MlsSurface(ConstPointSetPtr pPoints);
    
    virtual ~MlsSurface();
    
//     virtual void initSession(void) {}
//     virtual bool project(Vector3& position, Vector3& normal, Color& color) const {return false;}
//     virtual Real potentiel(const Vector3& position) const;
//     virtual Vector3 gradient(const Vector3& position) const;
//     virtual bool isValid(const Vector3& position) const;
//     virtual void flushStatistics(void);

public:

    /** \name Neighorhood
    */
    //@{
    QUICK_NEIGHBORHOOD_MEMBERS(mpInputPoints);
    //@}
    
    /** \name Weighting function
    */
    //@{
    QUICK_WEIGHTINGFUNCTION_MEMBERS;
    //@}

    /** Precision threshold for the iterative projection.
    */
    QUICK_MEMBER(Real,ProjectionAccuracy);
    
    /** Maximal number of iteration for the projection.
    */
    QUICK_MEMBER(uint,MaxNofProjectionIterations);

public:

    ConstPointSetPtr mpInputPoints;
    Real mObjectScale;
};


class MlsSurfaceFactory : public QuickFactory
{
public:
    MlsSurfaceFactory(const QString& type, const QMetaObject* meta=0) : QuickFactory(type, meta) {}
    virtual ~MlsSurfaceFactory() {}
    virtual MlsSurface* create(ConstPointSetPtr pPoints) = 0;
};

template <class T> class MlsSurfaceFactoryCreator : public QuickFactoryCreator<MlsSurfaceFactory,T>
{
public:
    virtual ~MlsSurfaceFactoryCreator() {}
    
    virtual MlsSurface* create(ConstPointSetPtr pPoints)
    {
        return new T(pPoints);
    }
};

//class MlsSurfaceManager : public QuickManager<MlsSurface,MlsSurfaceFactory> {};
typedef QuickManager<MlsSurface,MlsSurfaceFactory> MlsSurfaceManager;

}

#endif

