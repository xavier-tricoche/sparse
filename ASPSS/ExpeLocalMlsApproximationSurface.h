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



#ifndef _ExpeLocalMlsApproximationSurface_h_
#define _ExpeLocalMlsApproximationSurface_h_

#include "ExpeMlsSurface.h"

namespace Expe
{

/** Abstract class for Point Set Surfaces defined from local MLS approximations.
*/

class LocalMlsApproximationSurface : public MlsSurface
{
/*
Q_OBJECT

Q_ENUMS(ProjectionMode);

Q_PROPERTY(bool StartProjWithClosest READ getStartProjWithClosest WRITE setStartProjWithClosest DESIGNABLE true STORED true);
Q_PROPERTY(ProjectionMode ProjectionMode READ getProjectionMode WRITE setProjectionMode DESIGNABLE true STORED true);
Q_PROPERTY(bool CheckRestrictedDomain READ getCheckRestrictedDomain WRITE setCheckRestrictedDomain DESIGNABLE true STORED true);
Q_PROPERTY(uint DomainMinNofSamples READ getDomainMinNofSamples WRITE setDomainMinNofSamples DESIGNABLE true STORED true);
Q_PROPERTY(float DomainRadiusScale READ getDomainRadiusScale WRITE setDomainRadiusScale DESIGNABLE true STORED true);
Q_PROPERTY(float DomainNormalScale READ getDomainNormalScale WRITE setDomainNormalScale DESIGNABLE true STORED true);
*/

public:

    /** Projection mode.
        See Alexa & Adamson, PBG 2004 for the details on the difference between the projection operators.
    */
    enum ProjectionMode {
        PM_Basic,           ///< iteratively project the current evaluation point.
        PM_AlmostOrtho,     ///< iteratively project the initial point.
        PM_Ortho            ///< iteratively project the initial point in the gradient direction.
    };

    LocalMlsApproximationSurface(ConstPointSetPtr pPoints);
    
    virtual ~LocalMlsApproximationSurface();
    
    virtual void initSession(void);
    
    virtual bool project(Vector3& position, Vector3& normal, Color& color) const;
    
    virtual Real potentiel(const Vector3& position) const;
    
    virtual Vector3 gradient(const Vector3& position) const;
    
    virtual bool isValid(const Vector3& position) const;
    
    /** Output some statistics.
    */
    virtual void flushStatistics(void);

public:

    /** Starts the projection using the closest sample instead of the initial point.
    */
    QUICK_MEMBER(bool,StartProjWithClosest);
    
    /** Gets/sets the projection mode.
        \see ProjectionMode
    */
    QUICK_MEMBER(ProjectionMode,ProjectionMode);
    
    QUICK_MEMBER(bool,CheckRestrictedDomain);
    QUICK_MEMBER(uint,DomainMinNofSamples);
    QUICK_MEMBER(Real,DomainRadiusScale);
    QUICK_MEMBER(Real,DomainNormalScale);

protected:

    /** Computes the local approximation.
    */
    virtual bool fit(const Vector3& position) = 0;
    
    /** Computes the potentiel value.
        In other words, it computes the distance to the local approximation.
    */
    virtual Real evalPotentiel(const Vector3& position) = 0;
    
    /** Eval the MLS gradient.
       (that is different from the gradient of the local approximation)
    */
    virtual Vector3 evalMlsGradient(const Vector3& position) = 0;
    
    /** Does the projection onto the local approximation.
        \param position in and out argument corresponding to the point to project.
        \param normal returns an approximate normal: direction of the projection that usually corresponds to the gradient of the local approximation.
        \param delta returns the displacement which actually should corresponds to the potentiel value.
        \return false if the projection failed.
    */
    virtual bool doProjection(Vector3& position, Vector3& normal, Real& delta) = 0;

protected:

    mutable Vector3 mCachedPosition;

};

}

#endif

 
