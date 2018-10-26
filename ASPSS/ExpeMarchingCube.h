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



#ifndef _ExpeMarchingCube_h_
#define _ExpeMarchingCube_h_

#include "ExpePrerequisites.h"

#include "ExpeMesh.h"
#include "ExpeAxisAlignedBox.h"
#include <QObject>

namespace Expe
{

class ImplicitSurface;
class HalfedgeConnectivity;


/** Marching cube implementation.
*/
class MarchingCube : public QObject
{

public:

    MarchingCube(void);
    ~MarchingCube(void);
    
    /** Basically, does the job in an automatic way.
        \return false if any error occurs.
        \warning before calling this function you must first specify the underlying surface as well as the reconstruction domain.
        \note A fine tunned reconstruction can be performed using the low level reconstruction API.
        \par
        This function basically performs the following operations:
        \code
            _polygonize();
            _fillGaps();
            _optimizeCreaseAndValley();
            for k=1 to 4 do
                _relaxation();
                _optimizeValence();
            done
            _projection();
        \endcode
    */
    bool doReconstruction(void);
    
    /** return the reconstructed mesh.
    */
    MeshPtr getMesh(void) {return mpMesh;}
    
    
    /// \name Marching-cube setup
    //@{

    /** Specifies the underlying implicit surface.
        \warning this is mandatory before starting the reconstruction, of course ;)
    */
    void setSurface(const ImplicitSurface* pSurface) {mpSurface = pSurface;}
    
    /** Specifies the reconstruction domain.
        \warning this is mandatory before starting the reconstruction
    */
    void setAABB(const AxisAlignedBox& aabb) {mAABB = aabb;}
    
    /** Specifies the iso value to extract (default=0)
    */
    QUICK_MEMBER(Real,IsoValue);
//     void setIsoValue(Real value) {mIsoValue = value;}
//     Real getIsoValue(void) { return mIsoValue; }
    
    /** Specifies the resolution of the grid.
        Note that the grid is not instancied, so feel free to use
        a high resolution (>1k) if you want...
        The default grid resolution is 150.
    */
    QUICK_MEMBER(uint,Resolution);
//     void setResolution(uint r) {mResolution = r;}
//     Real getResolution(void) { return mResolution; }
    
    /** Define the threshold for automatic vertex clustering during the marching cube.
        This value multiplied by the width of a cell define the minimal distance between two vertices.
        A very small value (e.g. 1e-9) leads to a standard marching cube.
        On the other hand, a large value (e.g. 0.4) leads to a much more regular mesh
        which unfortunately may contains some gaps and "T" or crease configurations.
        These artifacts can be removed using _fillGaps, _optimizeCreaseAndValley, _removeT and _relaxation.
        The default value is 0.4.
    */
    QUICK_MEMBER(Real,ClusteringThreshold);
//     void setClusteringThresold(Real value) {mClusteringThresold = value;}
//     Real getClusteringThresold(void) { return mClusteringThresold; }
    //@}
    

    /** \name Low level reconstruction API
        This set of function allow to perform his/her own reconstruction or to perform additionnal
        optimizations after the default/automatic reconstruction algorithm.
    */
    //@{
    
    /** Simply does the marching cube, no more.
    */
    bool _polygonize(void);
    
    /** Fill small gaps (triangle and quads) which are due to the marching approximation.
    */
    void _fillGaps(void);
    
    /** Optimize the mesh connectivity (edge flip) to better match the features.
        This function perform a single iteration.
    */
    void _optimizeCreaseAndValley(void);
    
    /** Move each vertices toward their respective center of gravity (computed from the one ring neighborhood).
        The vertices are kept onto the surface as most as possible by moving along their respective tangent plane.
        This function perform a single iteration.
        Afater, you might call _projection() to indeed project the vertices onto the surface.
        \see _projection()
    */
    void _relaxation(void);
    
    /** Optimize the mesh connectivity (edge flip) to improve the vertex valence while trying to preserve features.
        This function perform a single iteration.
    */
    void _optimizeValence(void);
    
    /** Compute the normals of the vertices from the scalar field.
    */
    void _computeNormalsFromNumericGradient(void);
    
    /** Performs an accurate projection of each vertex onto the underlying surface.
        This usually leads to more accurate reconstruction.
    */
    void _projection(void);
    
    /** Remove "T configuration" by edge flipping.
        Usually, most of these ill-conditionned cases are removed by _optimizeCreaseAndValley and totally removed by _relaxation.
        Here we call a "T configuration", a very thin face with one very highly open angle:
        \code
         /     |     \
        O------O------O
        \------------/
         \          /
        \endcode
        
    */
    void _removeT(void);
    
    //@}
    
    void printStats(void) const;

protected:

    HalfedgeConnectivity& editConnectivity(void);

protected:

    /** represent a vertices of the lattice
    */
    struct GridElement
    {
        Vector3 position;
        Real value;
    };

    AxisAlignedBox mAABB;
//     Real mIsoValue;
//     uint mResolution;
    
    const ImplicitSurface* mpSurface;

    inline Vector3 interpolEdge(const GridElement& v1, const GridElement& v2);
    
    MeshPtr mpMesh;
    HalfedgeConnectivity* mConnectivity;

    Real mClosestEpsilon;
    
    static int msEdgeTable[256];
    static int msTriTable[256][16];
    
    struct McStatistics;
    McStatistics* mStats;
};

}

#endif

