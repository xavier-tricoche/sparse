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




#include "ExpeMarchingCube.h"

#include "ExpeCliProgressBar.h"
#include "ExpeImplicitSurface.h"
#include "ExpeMesh.h"
#include "ExpeQueryGrid.h"
#include "ExpeHalfedgeConnectivity.h"
#include "ExpeHalfedgeIterators.h"
#include "ExpeLogManager.h"
#include "ExpeTimer.h"
#include "ExpeStringHelper.h"

namespace Expe
{

struct MarchingCube::McStatistics
{
    Timer marchingcubeTimer;
    Timer connectivityTimer;
    Timer relaxationTimer;
    Timer gapsfillingTimer;
    Timer projectionTimer;
    Timer totalTimer;
    uint nofFilledGaps;
    void reset(void)
    {
        marchingcubeTimer.reset();
        connectivityTimer.reset();
        relaxationTimer.reset();
        gapsfillingTimer.reset();
        totalTimer.reset();
        projectionTimer.reset();
        nofFilledGaps = 0;
    }
};

MarchingCube::MarchingCube(void)
{
    mResolution = 150;
    mpSurface = 0;
    mClosestEpsilon = 0.;
    mConnectivity = 0;
    mClusteringThreshold = 0.4;
    mStats = new McStatistics();
}

MarchingCube::~MarchingCube(void)
{
    delete mConnectivity;
    delete mStats;
}

inline Vector3 MarchingCube::interpolEdge(const GridElement& v1, const GridElement& v2)
{
    Real epsilon = 1e-4;
    
    if (fabs(mIsoValue-v1.value) < epsilon)
        return v1.position;
    if (fabs(mIsoValue-v2.value) < epsilon)
        return v2.position;
    if (fabs(v1.value-v2.value) < epsilon)
        return v1.position;
    
    Real a = (mIsoValue - v1.value) / (v2.value - v1.value);
    return v1.position + a * (v2.position - v1.position);
}

bool MarchingCube::doReconstruction(void)
{
    mStats->reset();
    mStats->totalTimer.start();
    
    if (!_polygonize())
        return false;

//     editConnectivity().dumpAsOFF("mcdump_raw.off");
    
    _fillGaps();
    _optimizeCreaseAndValley();
    
//     editConnectivity().dumpAsOFF("mcdump_filled.off");
    
    mStats->relaxationTimer.start();
    for (uint k=0 ; k<4 ; ++k)
    {
        _relaxation();
        _optimizeValence();
    }
    mStats->relaxationTimer.stop();
//     editConnectivity().dumpAsOFF("mcdump_relaxation.off");
    _projection();
    
    mStats->totalTimer.stop();
    
    return true;
}

bool MarchingCube::_polygonize()
{
    mStats->marchingcubeTimer.start();
    
    // works per block of maximal size 64
    static const int maxBlockSize = 64;
    // precomputed offsets to access the cell corners
    static const int offsets[8] = {
        0,
        1,
        1+maxBlockSize*maxBlockSize,
        maxBlockSize*maxBlockSize,
        maxBlockSize,
        1+maxBlockSize,
        1+maxBlockSize+maxBlockSize*maxBlockSize,
        maxBlockSize+maxBlockSize*maxBlockSize};

    Vector3 diag = mAABB.max() - mAABB.min();
    
    if (   (mpSurface==0)
        || (diag.x<=0.)
        || (diag.y<=0.)
        || (diag.z<=0.)
        || (mResolution==0))
    {
        return false;
    }
    
    GridElement grid[maxBlockSize*maxBlockSize*maxBlockSize];
    
    // start a new mesh
    DESTROY_PTR(mConnectivity);
    mpMesh = new Mesh();
    mpMesh->createSubMesh("mc");
    mpMesh->editVertices().reserve(mResolution*mResolution);
    SubMesh* pSubMesh = mpMesh->editSubMesh(0);
    
    QueryGrid* closestGrid = new QueryGrid(&(mpMesh->editVertices()), 7, diag.maxComponent(), false);
    closestGrid->setMaxNofNeighbors(1);
    
    Real step = diag.maxComponent()/Real(mResolution);
    mClosestEpsilon = mClusteringThreshold*step;
    
    uint nofCells[3];
    uint nofBlocks[3];
    
    for (uint k=0 ; k<3 ; ++k)
    {
        nofCells[k] = int(diag[k]/step)+2;
        nofBlocks[k] = nofCells[k]/maxBlockSize + ( (nofCells[k]%maxBlockSize)==0 ? 0 : 1);
    }
    
    LOG_MESSAGE("Raw marching reconstruction...");
    CliProgressBarT<int> progressBar(0,Math::Max<uint>(nofBlocks[0]*nofBlocks[1]*nofBlocks[2]-1,1));

    // for each macro block
    uint bi[3]; // block id
    for(bi[2]=0 ; bi[2]<nofBlocks[2] ; ++bi[2])
    for(bi[1]=0 ; bi[1]<nofBlocks[1] ; ++bi[1])
    for(bi[0]=0 ; bi[0]<nofBlocks[0] ; ++bi[0])
    {
        // compute the size of the local grid
        uint gridSize[3];
        for (uint k=0 ; k<3 ; ++k)
        {
            gridSize[k] = Math::Min<int>(maxBlockSize, nofCells[k]-(maxBlockSize-1)*bi[k]);
        }
        Vector3 origin = mAABB.min() + step * (maxBlockSize-1) * Vector3(bi[0],bi[1],bi[2]);
        
        // fill the grid
        uint ci[3]; // local cell id
        
        // for each corner...
        for(ci[0]=0 ; ci[0]<gridSize[0] ; ++ci[0])
        for(ci[1]=0 ; ci[1]<gridSize[1] ; ++ci[1])
        for(ci[2]=0 ; ci[2]<gridSize[2] ; ++ci[2])
        {
            GridElement& el = grid[(ci[2]*maxBlockSize + ci[1])*maxBlockSize + ci[0]];
            el.position = origin+step*Vector3(ci[0],ci[1],ci[2]);
            el.value = mpSurface->potentiel(el.position);
        }
        
        // polygonize the grid (marching cube)
        // for each cell...
        for(ci[0]=0 ; ci[0]<gridSize[0]-1 ; ++ci[0])
        for(ci[1]=0 ; ci[1]<gridSize[1]-1 ; ++ci[1])
        for(ci[2]=0 ; ci[2]<gridSize[2]-1 ; ++ci[2])
        {
            uint cellId = ci[0]+maxBlockSize*(ci[1]+maxBlockSize*ci[2]);
            // FIXME check if one corner is outside the surface definition domain
            /*bool out = grid[cellId+offsets[0]].value==1e6 || grid[cellId+offsets[1]].value==1e6
                    || grid[cellId+offsets[2]].value==1e6 || grid[cellId+offsets[3]].value==1e6
                    || grid[cellId+offsets[4]].value==1e6 || grid[cellId+offsets[5]].value==1e6
                    || grid[cellId+offsets[6]].value==1e6 || grid[cellId+offsets[7]].value==1e6;*/
            bool out = grid[cellId+offsets[0]].value>=1e6 || grid[cellId+offsets[1]].value>=1e6
                    || grid[cellId+offsets[2]].value>=1e6 || grid[cellId+offsets[3]].value>=1e6
                    || grid[cellId+offsets[4]].value>=1e6 || grid[cellId+offsets[5]].value>=1e6
                    || grid[cellId+offsets[6]].value>=1e6 || grid[cellId+offsets[7]].value>=1e6;
            
            if (!out)
            {
                // compute the mask
                int mask = 0;
                if (grid[cellId+offsets[0]].value <= mIsoValue) mask |= 1;
                if (grid[cellId+offsets[1]].value <= mIsoValue) mask |= 2;
                if (grid[cellId+offsets[2]].value <= mIsoValue) mask |= 4;
                if (grid[cellId+offsets[3]].value <= mIsoValue) mask |= 8;
                if (grid[cellId+offsets[4]].value <= mIsoValue) mask |= 16;
                if (grid[cellId+offsets[5]].value <= mIsoValue) mask |= 32;
                if (grid[cellId+offsets[6]].value <= mIsoValue) mask |= 64;
                if (grid[cellId+offsets[7]].value <= mIsoValue) mask |= 128;

                if (msEdgeTable[mask] != 0)
                {
                    Vector3 edges[12];

                    if (msEdgeTable[mask] & 1)
                        edges[0] = interpolEdge(grid[cellId+offsets[0]],grid[cellId+offsets[1]]);
                    if (msEdgeTable[mask] & 2)
                        edges[1] = interpolEdge(grid[cellId+offsets[1]],grid[cellId+offsets[2]]);
                    if (msEdgeTable[mask] & 4)
                        edges[2] = interpolEdge(grid[cellId+offsets[2]],grid[cellId+offsets[3]]);
                    if (msEdgeTable[mask] & 8)
                        edges[3] = interpolEdge(grid[cellId+offsets[3]],grid[cellId+offsets[0]]);
                    if (msEdgeTable[mask] & 16)
                        edges[4] = interpolEdge(grid[cellId+offsets[4]],grid[cellId+offsets[5]]);
                    if (msEdgeTable[mask] & 32)
                        edges[5] = interpolEdge(grid[cellId+offsets[5]],grid[cellId+offsets[6]]);
                    if (msEdgeTable[mask] & 64)
                        edges[6] = interpolEdge(grid[cellId+offsets[6]],grid[cellId+offsets[7]]);
                    if (msEdgeTable[mask] & 128)
                        edges[7] = interpolEdge(grid[cellId+offsets[7]],grid[cellId+offsets[4]]);
                    if (msEdgeTable[mask] & 256)
                        edges[8] = interpolEdge(grid[cellId+offsets[0]],grid[cellId+offsets[4]]);
                    if (msEdgeTable[mask] & 512)
                        edges[9] = interpolEdge(grid[cellId+offsets[1]],grid[cellId+offsets[5]]);
                    if (msEdgeTable[mask] & 1024)
                        edges[10] = interpolEdge(grid[cellId+offsets[2]],grid[cellId+offsets[6]]);
                    if (msEdgeTable[mask] & 2048)
                        edges[11] = interpolEdge(grid[cellId+offsets[3]],grid[cellId+offsets[7]]);
                
                    for (int i=0 ; msTriTable[mask][i]!=-1 ; i+=3)
                    {
                        Index auxId[3];
                        uint countAddedVertex = 0;
                        for (int j=0;j<3;++j)
                        {
                            const Vector3& p = edges[msTriTable[mask][i+j]];
                            closestGrid->doQueryBall(p,mClosestEpsilon);
                            if (closestGrid->getNofFoundNeighbors()==1)
                            {
                                // the vertex already exist
                                auxId[j] = closestGrid->getNeighborId(0);
                            }
                            else
                            {
                                // add a new vertex
                                auxId[j] = mpMesh->getNofVertices();
                                countAddedVertex++;
                                mpMesh->addVertex(p);
                                closestGrid->insert(mpMesh->getNofVertices()-1);
                            }
                        }
                        if (auxId[0]!=auxId[1] && auxId[1]!=auxId[2] && auxId[2]!=auxId[0])
                        {
                            Mesh::FaceHandle face = pSubMesh->createFace(3,Mesh::None);
                            for (uint j=0;j<3;++j)
                                face.vertexId(j) = auxId[j];
                        }
                        else
                        {
                            for (uint k=0 ; k<countAddedVertex ; ++k)
                            {
                                closestGrid->remove(mpMesh->getNofVertices()-1);
                                mpMesh->editVertices().pop_back();
                            }
                            LOG_DEBUG_MSG(7,"MarchingCube: skip degenerated face");
                        }
                    }
                }
            }
        }
        progressBar.update(bi[0]+nofBlocks[0]*(bi[1]+nofBlocks[1]*bi[2]));
    }
    mStats->marchingcubeTimer.stop();
    return true;
}

void MarchingCube::_computeNormalsFromNumericGradient(void)
{
    LOG_MESSAGE("MarchingCube - Compute the normals from the numeric gradient...");
    uint nb = mpMesh->getNofVertices();
    Real eps = 0.05 * (mAABB.max() - mAABB.min()).maxComponent()/Real(mResolution);
    CliProgressBarT<int> progressBar(0,nb-1);
    for(uint i=0 ; i<nb ; i++)
    {
        mpMesh->vertex(i).normal() = mpSurface->numericalGradient(mpMesh->vertex(i).position(), eps).normalized();
        progressBar.update(i);
    }
}

void MarchingCube::_projection(void)
{
    if ( (!mpSurface) || (!mpMesh) )
        return;
    
    LOG_MESSAGE("MarchingCube - Accurate projection...");
    mStats->projectionTimer.start();
    Vector3 position;
    Vector3 normal;
    Color c;
    Real step = (mAABB.max() - mAABB.min()).maxComponent()/Real(mResolution);
    bool hasNormal = mpMesh->getVertices().hasAttribute(Mesh::VertexList::Attribute_normal);
    
    HalfedgeConnectivity hec = editConnectivity();

    CliProgressBarT<int> progressBar(0,mpMesh->getNofVertices()-1);
    for(uint i=0 ; i<mpMesh->getNofVertices() ; i++)
    {
        if (hec.getVertexStatus(i).isDeleted())
            continue;
        
        Mesh::VertexHandle v = mpMesh->vertex(i);
        position = v.position();
        bool remove = false;
        if (mpSurface->project(position, normal, c))
        {
            Real d = position.distanceTo(v.position());
            if (d>step)
            {
                remove = true;
            }
            else
            {
                v.position() = position;
                if (hasNormal)
                    v.normal() = normal;
            }
        }
        else
        {
            LOG_DEBUG_MSG(5,"MarchingCube::_projection: projection failed");
            remove = true;
        }
        if (remove)
        {
            hec.deleteVertex(i);
        }
        progressBar.update(i);
    }
    hec.garbage_collection();
    mStats->projectionTimer.stop();
}


namespace {
double _cosAngle(const Vector3d* p, uint i0, uint i1, uint i2)
{
    return (p[i1]-p[i0]).normalized().dot((p[i2]-p[i0]).normalized());
}
double _angle(const Vector3d* p, uint i0, uint i1, uint i2)
{
    return Math::ACos( Math::Clamp(_cosAngle(p,i0,i1,i2), -0.99, 0.99) );
}

Vector3 _computeNormal(const Vector3d* p, uint i0, uint i1, uint i2)
{
    return vector_cast<Vector3>((p[i1] - p[i0]).cross(p[i2] - p[i0]).normalized());
}
}

void MarchingCube::_optimizeValence(void)
{
    if (!mpMesh)
        return;
    HalfedgeConnectivity& hec = editConnectivity();
    
    for (uint i=0 ; i<hec.getNofEdges() ; ++i)
    {
        HEC::EdgeId eid(i);
        if ((!hec.getEdgeStatus(eid).isDeleted()) && hec.isFlipOk(eid))
        {
            // check if an edge flip would improve the valence
            // compute the initial valence of each vertex
            HEC::EdgeHandle e = hec.editEdge(eid);
            HEC::VertexId vids[4];
            vids[0] = e.halfedges[0].toVertexId;
            vids[1] = e.halfedges[1].toVertexId;
            vids[2] = hec.toVertexId(e.halfedges[0].nextHalfedgeId);
            vids[3] = hec.toVertexId(e.halfedges[1].nextHalfedgeId);
            
            Vector3d p[4];
            for (uint k=0 ; k<4 ; ++k)
                p[k] = vector_cast<Vector3d>(hec.getVertex(vids[k]).position());

            /* the following test aims to avoid these cases:
                       O
                     / /
                   /  /
                /    /
                O---O
                \   \
                  \  \
                    \ \
                      O
            */
            if (   (_angle(p,0,2,1)+_angle(p,0,1,3) > Math::PI)
                || (_angle(p,1,0,2)+_angle(p,1,3,0) > Math::PI) )
            {
                continue;
            }
            
            // compute the valence errors
            uint vals[4];
            uint initError = 0;
            for (uint k=0 ; k<4 ; ++k)
            {
                vals[k] = hec.computeValence(vids[k]);
                initError += (vals[k]-6)*(vals[k]-6);
            }
            vals[0]--; vals[1]--; vals[2]++; vals[3]++;
            uint flipError = 0;
            for (uint k=0 ; k<4 ; ++k)
                flipError += (vals[k]-6)*(vals[k]-6);
            
            // crease and valley stuff
            HEC::FaceId fa1 = hec.faceId(hec.oppositeHalfedgeId(e.halfedges[0].nextHalfedgeId));
            HEC::FaceId fa2 = hec.faceId(hec.oppositeHalfedgeId(hec.nextHalfedgeId(e.halfedges[0].nextHalfedgeId)));
            HEC::FaceId fb1 = hec.faceId(hec.oppositeHalfedgeId(e.halfedges[1].nextHalfedgeId));
            HEC::FaceId fb2 = hec.faceId(hec.oppositeHalfedgeId(hec.nextHalfedgeId(e.halfedges[1].nextHalfedgeId)));
            
            Real initNerr=0., flipNerr=0.;
            if ( fa1.isValid() && fa2.isValid() && fb1.isValid() && fb2.isValid() )
            {
                Vector3 nfa1 = hec.computeNormal(fa1);
                Vector3 nfa2 = hec.computeNormal(fa2);
                Vector3 nfb1 = hec.computeNormal(fb1);
                Vector3 nfb2 = hec.computeNormal(fb2);
                
                Vector3 nfa = hec.computeNormal(e.halfedges[0].faceId);
                Vector3 nfb = hec.computeNormal(e.halfedges[1].faceId);
                
                Vector3 nfabis = _computeNormal(p,1,3,2);
                Vector3 nfbbis = _computeNormal(p,0,2,3);
                
                // dilema, should we use the max of the angle or the average... ?
                initNerr = Math::Max(Math::Max(acos(nfa.dot(nfa1)), acos(nfa.dot(nfa2))), Math::Max(acos(nfb.dot(nfb1)), acos(nfb.dot(nfb2))));
                flipNerr = Math::Max(Math::Max(acos(nfabis.dot(nfa2)), acos(nfabis.dot(nfb1))), Math::Max(acos(nfbbis.dot(nfa1)), acos(nfbbis.dot(nfb2))));

//                 initNerr = acos(nfa.dot(nfa1)) + acos(nfa.dot(nfa2)) + acos(nfb.dot(nfb1)) + acos(nfb.dot(nfb2));
//                 flipNerr = acos(nfabis.dot(nfa2)) + acos(nfabis.dot(nfb1)) + acos(nfbbis.dot(nfa1)) + acos(nfbbis.dot(nfb2));

                if ((!Math::isFinite(initNerr)) || (!Math::isFinite(flipNerr)))
                {
                    if (flipError<initError)
                        hec.flip(eid);
                }
                else  if (flipError<initError && (flipNerr<2.*initNerr)) // take care to not increase the local curvature too much
                {
                    hec.flip(eid);
                }
            }
            else if (flipError<initError)
            {
                    hec.flip(eid);
            }
        }
    }
}

void MarchingCube::_removeT(void)
{
    if (!mpMesh)
        return;
    
    HalfedgeConnectivity& hec = editConnectivity();

    for (HEC::FaceList::iterator f_it(hec.editFaces().begin()), f_end(hec.editFaces().end()) ; f_it!=f_end ; ++f_it)
    {
        if (!hec.getFaceStatus(f_it.id()).isDeleted())
        {
            HEC::HalfedgeId hid[3];
            Vector3d v[3];
            HEC::HalfedgeId h = hec.halfedgeId(f_it.id());
            for (uint k=0 ; k<3 ; ++k)
            {
                hid[k] = h;
                v[k] = vector_cast<Vector3d>(hec.getVertex(hec.toVertexId(h)).position());
                h = hec.nextHalfedgeId(h);
            }
            for (uint k=0 ; k<3 ; ++k)
            {
                double dp = (v[(k+1)%3]-v[k]).normalized().dot((v[(k+2)%3]-v[k]).normalized());
                if (dp<-0.98)
                {
                    HEC::EdgeId eid(hec.edgeId(hid[(k+2)%3]));
                    if (hec.isFlipOk(eid))
                    {
                        hec.flip(eid);
                    }
                }
            }
        }
    }
}

void MarchingCube::_optimizeCreaseAndValley(void)
{
    if (!mpMesh)
        return;
    HalfedgeConnectivity& hec = *mConnectivity;
    
    for (uint i=0 ; i<hec.getNofEdges() ; ++i)
    {
        HEC::EdgeId eid(i);
        if ((!hec.getEdgeStatus(eid).isDeleted()) && hec.isFlipOk(eid))
        {
            HEC::EdgeHandle e = hec.editEdge(eid);
            HEC::VertexId vids[4];
            vids[0] = e.halfedges[0].toVertexId;
            vids[1] = e.halfedges[1].toVertexId;
            vids[2] = hec.toVertexId(e.halfedges[0].nextHalfedgeId);
            vids[3] = hec.toVertexId(e.halfedges[1].nextHalfedgeId);
            Vector3d p[4];
            for (uint k=0 ; k<4 ; ++k)
                p[k] = vector_cast<Vector3d>(hec.getVertex(vids[k]).position());
            
            // compute the angles with adjacent faces
            HEC::FaceId fa1 = hec.faceId(hec.oppositeHalfedgeId(e.halfedges[0].nextHalfedgeId));
            HEC::FaceId fa2 = hec.faceId(hec.oppositeHalfedgeId(hec.nextHalfedgeId(e.halfedges[0].nextHalfedgeId)));
            HEC::FaceId fb1 = hec.faceId(hec.oppositeHalfedgeId(e.halfedges[1].nextHalfedgeId));
            HEC::FaceId fb2 = hec.faceId(hec.oppositeHalfedgeId(hec.nextHalfedgeId(e.halfedges[1].nextHalfedgeId)));
            
            Real initNerr=0., flipNerr=0.;
            if ( fa1.isValid() && fa2.isValid() && fb1.isValid() && fb2.isValid() )
            {
                Vector3 nfa1 = hec.computeNormal(fa1);
                Vector3 nfa2 = hec.computeNormal(fa2);
                Vector3 nfb1 = hec.computeNormal(fb1);
                Vector3 nfb2 = hec.computeNormal(fb2);
                
                Vector3 nfa = hec.computeNormal(e.halfedges[0].faceId);
                Vector3 nfb = hec.computeNormal(e.halfedges[1].faceId);
                
                Vector3 nfabis = _computeNormal(p,1,3,2);
                Vector3 nfbbis = _computeNormal(p,0,2,3);
                
                // dilema, should we use the max of the angle or the average... ?
                
                //initNerr = Math::Max(Math::Max(acos(nfa.dot(nfa1)), acos(nfa.dot(nfa2))), Math::Max(acos(nfb.dot(nfb1)), acos(nfb.dot(nfb2))));
                //flipNerr = Math::Max(Math::Max(acos(nfabis.dot(nfa2)), acos(nfabis.dot(nfb1))), Math::Max(acos(nfbbis.dot(nfa1)), acos(nfbbis.dot(nfb2))));
                
                initNerr = acos(nfa.dot(nfa1)) + acos(nfa.dot(nfa2)) + acos(nfb.dot(nfb1)) + acos(nfb.dot(nfb2));
                flipNerr = acos(nfabis.dot(nfa2)) + acos(nfabis.dot(nfb1)) + acos(nfbbis.dot(nfa1)) + acos(nfbbis.dot(nfb2));
                
                if ( ((!Math::isFinite(initNerr)) && Math::isFinite(flipNerr))
                    || ( Math::isFinite(initNerr) && Math::isFinite(flipNerr) && (flipNerr < 0.85*initNerr) && (initNerr>0.5) )
                )
                {
                    hec.flip(eid);
                }
            }
        }
    }
}

void MarchingCube::_fillGaps(void)
{
    mStats->gapsfillingTimer.start();
    
    if (!mpMesh)
        return;

    HalfedgeConnectivity& hec = editConnectivity();
    
    // first let us remove the faces having two boundary edges
    std::vector<HEC::FaceId> deleteFaceIds;
    for (HEC::FaceList::iterator f_it(hec.editFaces().begin()), f_end(hec.editFaces().end()) ; f_it!=f_end ; ++f_it)
    {
        if (!hec.getFaceStatus(f_it.id()).isDeleted())
        {
            uint countBoundaries = 0;
            HEC::HalfedgeId h = hec.halfedgeId(f_it.id());
            for (uint k=0 ; k<3 ; ++k)
            {
                if (hec.isBoundary(hec.edgeId(h)))
                    countBoundaries++;
                h = hec.nextHalfedgeId(h);
            }
            if (countBoundaries>=2) // at the same time let us remove isolated triangles...
                deleteFaceIds.push_back(f_it.id());
        }
    }
    
    if (!deleteFaceIds.empty())
    {
        for (std::vector<HEC::FaceId>::iterator it(deleteFaceIds.begin()), itend(deleteFaceIds.end()) ; it!=itend ; ++it)
            hec.deleteFace(*it);
        hec.garbage_collection();
    }

    for (uint i=0 ; i<hec.getNofEdges() ; ++i)
    {
        HEC::EdgeId eid(i);
        if (!hec.getEdgeStatus(eid).isDeleted())
        {
            HEC::HalfedgeId hid = hec.halfedgeId(eid,1);
            if (!hec.isBoundary(hid))
            {
                hid = hec.halfedgeId(eid,0);
                if (!hec.isBoundary(hid))
                    continue;
            }
            int count = 0;
            HEC::HalfedgeId ref = hid;
            do {
                count++;
                hid = hec.nextHalfedgeId(hid);
            } while (hid!=ref);
            
            if (count>=3 && count<=10)
            {
                bool failed = false;
                while ( (count!=2) && (!failed))
                {
                    // try to add one triangle
                    HEC::HalfedgeId ref = hid;
                    do
                    {
                        IndexArray vertexIds;
                        for (uint j=0;j<3;++j)
                        {
                            vertexIds.push_back(hec.toVertexId(hid));
                            hid = hec.nextHalfedgeId(hid);
                        }
                        if (hec.createFace(vertexIds).isValid())
                        {
                            count--;
                            break;
                        }
                    } while (ref!=hid);
                    if ((ref==hid) && count!=2)
                        failed = true;
                }
            }
        }
    }

    mStats->gapsfillingTimer.stop();
    
    #ifdef EXPE_DEBUG
    if (mConnectivity->checkIntegrity()) { LOG_DEBUG_MSG(9,"MarchingCube::_fillGaps: checkIntegrity OK"); }
    else { LOG_ERROR("MarchingCube::_fillGaps: checkIntegrity FAILED"); }
    #endif
}

void MarchingCube::_relaxation(void)
{
    if ( (!mpMesh) || (!mpSurface) )
        return;
    HalfedgeConnectivity& hec = editConnectivity();
    
    Color dummy_c;
    for (uint i=0 ; i<hec.getNofVertices() ; ++i)
    {
        HEC::VertexHandle v = hec.editVertex(i);
        Vector3 p1, p2;
        Vector3 cog(Vector3::ZERO);
        uint valence = 0;
        Vector3 normal(Vector3::ZERO);
        bool nok = true;
        for (HEC::VertexCirculator vv_it(&hec,HEC::VertexId(i)) ; vv_it ; )
        {
            p1 = hec.getVertex(vv_it.vertexId()).position();
            cog += p1;
            valence++;
            
            ++vv_it;
            p2 = hec.getVertex(vv_it.vertexId()).position();
            
            Vector3 nf = (p1-v.position()).normalized().cross( (p2-v.position()).normalized() );
            Real lnf;
            if ( (lnf=nf.length())>0.1 )
                normal += nf / lnf;
            else
                nok = false;
        }
        cog = (cog+v.position())/Real(valence+1);
        if (nok)
        {
            normal.normalize();
            v.position() = cog - (cog-v.position()).dot(normal) * normal;
        }
        else
        {
            // fall back to accurate projection
            //if (pSurface->project(cog, normal, dummy_c))
            {
                v.position() = cog;
            }
        }
    }
}

HalfedgeConnectivity& MarchingCube::editConnectivity(void)
{
    if (mConnectivity==0)
    {
        LOG_MESSAGE("MarchingCube - build the halfedge connectivity...");
        mStats->connectivityTimer.start();
        #ifdef EXPE_DEBUG
        mConnectivity = new HalfedgeConnectivity(mpMesh, Verbose);
        #else
        mConnectivity = new HalfedgeConnectivity(mpMesh, Quiet);
        #endif
        // the halfedge data structure cannot import faces that leads to non-manifold situations,
        // we then have to explicitly throw them away by calling garbage_collection
        mConnectivity->garbage_collection();
        mStats->connectivityTimer.stop();
        //mConnectivity->dumpAsOFF("mc_raw.off");
    }
    return *mConnectivity;
}

void MarchingCube::printStats(void) const
{
    std::cout << "\n*********************************************\n";
    std::cout << "*  MarchingCube statistics\n";
    std::cout << "*   - marching cube time:   " << mStats->marchingcubeTimer.toHMS() << "\n";
    std::cout << "*   - connectivity time:    " << mStats->connectivityTimer.toHMS() << "\n";
    std::cout << "*   - gaps filling time:    " << mStats->gapsfillingTimer.toHMS() << "\n";
    std::cout << "*   - relaxation time:      " << mStats->relaxationTimer.toHMS() << "\n";
    std::cout << "*   - projection time:      " << mStats->projectionTimer.toHMS() << "\n";
    std::cout << "*   - total time:           " << mStats->totalTimer.toHMS() << "\n";
    std::cout << "*\n";
    std::cout << "*   - number of vertices:   " << mpMesh->getNofVertices() << "\n";
    std::cout << "*   - number of faces:      " << mpMesh->getNofFaces() << "\n";
    std::cout << "*********************************************\n";
}

//************************************************************************
// Static precalculated table
//************************************************************************
int MarchingCube::msEdgeTable[256]={
    0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0};

int MarchingCube::msTriTable[256][16] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

};

