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



//--------------------------------------------------------------------------------
// NOTE THAT THIS FILE IS BASED ON MATERIAL FROM:
//
//  OpenMesh
//  Copyright (C) 2004 by Computer Graphics Group, RWTH Aachen
//  http://www.openmesh.org
//
//  This library is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, version 2.1.
//--------------------------------------------------------------------------------

#ifndef _ExpeHalfedgeIterators_h_
#define _ExpeHalfedgeIterators_h_

#include "ExpeHalfedgeConnectivity.h"

namespace Expe
{

namespace HalfedgeConnectivityCirculators
{

class VertexCirculator
{
friend class ConstVertexCirculator;
public:

    typedef HalfedgeConnectivity::HalfedgeId HalfedgeId;
    typedef HalfedgeConnectivity::Halfedge Halfedge;
    typedef HalfedgeConnectivity::VertexId VertexId;
    typedef HalfedgeConnectivity::FaceId FaceId;

    VertexCirculator(void) : mpConnectivity(0), mIsActive(false) {}

    VertexCirculator(HalfedgeConnectivity* pConnectivity, HalfedgeConnectivity::VertexId vidStart)
        :   mpConnectivity(pConnectivity), mStartHeId(pConnectivity->halfedgeId(vidStart)),
            mCurrentHeId(mStartHeId), mIsActive(false)
    {}


    /// Allows to start at a specific halfedge
    VertexCirculator(HalfedgeConnectivity* pConnectivity, HalfedgeConnectivity::HalfedgeId hidStart)
        :   mpConnectivity(pConnectivity), mStartHeId(hidStart),
            mCurrentHeId(mStartHeId), mIsActive(false)
    {}

    /// Copy constructor
    VertexCirculator(const VertexCirculator& vc)
         :  mpConnectivity(vc.mpConnectivity), mStartHeId(vc.mStartHeId),
            mCurrentHeId(vc.mCurrentHeId), mIsActive(vc.mIsActive)
    {}

    /// Assignment operator
    VertexCirculator& operator=(const VertexCirculator& vc)
    {
        mpConnectivity = vc.mpConnectivity;
        mStartHeId = vc.mStartHeId;
        mCurrentHeId = vc.mCurrentHeId;
        mIsActive = vc.mIsActive;
        return *this;
    }
    
    /*
    /// construct from non-const circulator type
    ConstVertexCirculator(const VertexCirculator& vc)
         :  mpConnectivity(vc.mpConnectivity), mStartHeId(vc.mStartHeId)
            mCurrentHeId(vc.mCurrentHeId), mIsActive(vc.mIsActive)
    {}

    /// assign from non-const circulator
    ConstVertexCirculator& operator=(const VertexCirculator& vc)
    {
        mpConnectivity(vc.mpConnectivity);
        mStartHeId(vc.mStartHeId);
        mCurrentHeId(vc.mCurrentHeId);
        mIsActive(vc.mIsActive);
        return *this;
    }
    */

    /// Equal ?
    bool operator==(const VertexCirculator& other) const
    {
        return ((mpConnectivity == other.mpConnectivity) && (mStartHeId==other.mStartHeId)
            && (mCurrentHeId==other.mCurrentHeId) && (mIsActive==other.mIsActive));
    }


    /// Not equal ?
    bool operator!=(const VertexCirculator& other) const {
        return !operator==(other);
    }

    /// Pre-Increment (next cw target)
    VertexCirculator& operator++()
    {
        assert(mpConnectivity);
        mIsActive = true;
        mCurrentHeId = mpConnectivity->cwRotatedHalfedgeId(mCurrentHeId);
        return *this;
    }
    
    /// Pre-Decrement (next ccw target)
    VertexCirculator& operator--()
    {
        assert(mpConnectivity);
        mIsActive = true;
        mCurrentHeId = mpConnectivity->ccwRotatedHalfedgeId(mCurrentHeId);
        return *this;
    }

    /** Get the current outgoing halfedge index.
    */
    inline HalfedgeId halfedgeId(void) const { return mCurrentHeId; }
    inline const Halfedge& halfedge(void) const { return mpConnectivity->getHalfedge(mCurrentHeId); }
    inline Halfedge& halfedge(void) { return mpConnectivity->editHalfedge(mCurrentHeId); }
    
    /** Get the current opposite/ingoing halfedge index.
    */
    inline HalfedgeId oppositeHalfedgeId(void) const { return mpConnectivity->oppositeHalfedgeId(mCurrentHeId); }
    inline const Halfedge& oppositeHalfedge(void) const { return mpConnectivity->getHalfedge(oppositeHalfedgeId()); }
    inline Halfedge& oppositeHalfedge(void) { return mpConnectivity->editHalfedge(oppositeHalfedgeId()); }
    
    /** Get the current vertex index
    */
    inline VertexId vertexId(void) const { return mpConnectivity->toVertexId(mCurrentHeId); }
    
    /** Get the current/left/ccw face index
    */
    inline FaceId ccwFaceId(void) const { return mpConnectivity->faceId(mCurrentHeId); }
    /// convenient alias for ccwFaceId
    inline FaceId faceId(void) const { return mpConnectivity->faceId(mCurrentHeId); }
    
    /** Get the next/opposite/right/cw face index
    */
    inline FaceId cwFaceId(void) const { return mpConnectivity->faceId(oppositeHalfedgeId()); }
    
    /** Returns whether the circulator is still valid.
        After one complete round around a vertex/face the circulator becomes
        invalid, i.e. this function will return \c false. Nevertheless you
        can continue circulating. This method just tells you whether you
        have completed the first round.
    */
    inline operator bool() const
    {
        return mCurrentHeId.isValid() && ((mStartHeId != mCurrentHeId) || (!mIsActive));
    }

protected:

    HalfedgeConnectivity* mpConnectivity;
    HalfedgeId mStartHeId, mCurrentHeId;
    bool mIsActive;
};

class ConstVertexCirculator
{
public:

    typedef HalfedgeConnectivity::HalfedgeId HalfedgeId;
    typedef HalfedgeConnectivity::Halfedge Halfedge;
    typedef HalfedgeConnectivity::VertexId VertexId;
    typedef HalfedgeConnectivity::FaceId FaceId;

    ConstVertexCirculator(void) : mpConnectivity(0), mIsActive(false) {}

    ConstVertexCirculator(const HalfedgeConnectivity* pConnectivity, HalfedgeConnectivity::VertexId vidStart)
        :   mpConnectivity(pConnectivity), mStartHeId(pConnectivity->halfedgeId(vidStart)),
            mCurrentHeId(mStartHeId), mIsActive(false)
    {}


    /// Allows to start at a specific halfedge
    ConstVertexCirculator(const HalfedgeConnectivity* pConnectivity, HalfedgeConnectivity::HalfedgeId hidStart)
        :   mpConnectivity(pConnectivity), mStartHeId(hidStart),
            mCurrentHeId(mStartHeId), mIsActive(false)
    {}

    /// Copy constructor
    ConstVertexCirculator(const ConstVertexCirculator& vc)
         :  mpConnectivity(vc.mpConnectivity), mStartHeId(vc.mStartHeId),
            mCurrentHeId(vc.mCurrentHeId), mIsActive(vc.mIsActive)
    {}

    /// Assignment operator
    ConstVertexCirculator& operator=(const ConstVertexCirculator& vc)
    {
        mpConnectivity = vc.mpConnectivity;
        mStartHeId = vc.mStartHeId;
        mCurrentHeId = vc.mCurrentHeId;
        mIsActive = vc.mIsActive;
        return *this;
    }
    
    /// construct from non-const circulator type
    ConstVertexCirculator(const VertexCirculator& vc)
         :  mpConnectivity(vc.mpConnectivity), mStartHeId(vc.mStartHeId),
            mCurrentHeId(vc.mCurrentHeId), mIsActive(vc.mIsActive)
    {}

    /// assign from non-const circulator
    ConstVertexCirculator& operator=(const VertexCirculator& vc)
    {
        mpConnectivity = vc.mpConnectivity;
        mStartHeId = vc.mStartHeId;
        mCurrentHeId = vc.mCurrentHeId;
        mIsActive = vc.mIsActive;
        return *this;
    }

    /// Equal ?
    bool operator==(const VertexCirculator& other) const
    {
        return ((mpConnectivity == other.mpConnectivity) && (mStartHeId==other.mStartHeId)
            && (mCurrentHeId==other.mCurrentHeId) && (mIsActive==other.mIsActive));
    }


    /// Not equal ?
    bool operator!=(const VertexCirculator& other) const {
        return !operator==(other);
    }

    /// Pre-Increment (next cw target)
    ConstVertexCirculator& operator++()
    {
        assert(mpConnectivity);
        mIsActive = true;
        mCurrentHeId = mpConnectivity->cwRotatedHalfedgeId(mCurrentHeId);
        return *this;
    }
    
    /// Pre-Decrement (next ccw target)
    ConstVertexCirculator& operator--()
    {
        assert(mpConnectivity);
        mIsActive = true;
        mCurrentHeId = mpConnectivity->ccwRotatedHalfedgeId(mCurrentHeId);
        return *this;
    }

    /** Get the current outgoing halfedge index.
    */
    inline HalfedgeId halfedgeId(void) const { return mCurrentHeId; }
    inline const Halfedge& halfedge(void) const { return mpConnectivity->getHalfedge(mCurrentHeId); }
//     inline Halfedge& halfedge(void) { return mpConnectivity->editHalfedge(mCurrentHeId); }
    
    /** Get the current opposite/ingoing halfedge index.
    */
    inline HalfedgeId oppositeHalfedgeId(void) const { return mpConnectivity->oppositeHalfedgeId(mCurrentHeId); }
    inline const Halfedge& oppositeHalfedge(void) const { return mpConnectivity->getHalfedge(oppositeHalfedgeId()); }
//     inline Halfedge& oppositeHalfedge(void) { return mpConnectivity->editHalfedge(oppositeHalfedgeId()); }
    
    /** Get the current vertex index
    */
    inline VertexId vertexId(void) const { return mpConnectivity->toVertexId(mCurrentHeId); }
    
    /** Get the current/left/ccw face index
    */
    inline FaceId ccwFaceId(void) const { return mpConnectivity->faceId(mCurrentHeId); }
    /// convenient alias for ccwFaceId
    inline FaceId faceId(void) const { return mpConnectivity->faceId(mCurrentHeId); }
    
    /** Get the next/opposite/right/cw face index
    */
    inline FaceId cwfaceId(void) const { return mpConnectivity->faceId(oppositeHalfedgeId()); }
    
    /** Returns whether the circulator is still valid.
        After one complete round around a vertex/face the circulator becomes
        invalid, i.e. this function will return \c false. Nevertheless you
        can continue circulating. This method just tells you whether you
        have completed the first round.
    */
    inline operator bool() const
    {
        return mCurrentHeId.isValid() && ((mStartHeId != mCurrentHeId) || (!mIsActive));
    }

protected:

    const HalfedgeConnectivity* mpConnectivity;
    HalfedgeId mStartHeId, mCurrentHeId;
    bool mIsActive;
};




class FaceCirculator
{
friend class ConstFaceCirculator;
public:

    typedef HalfedgeConnectivity::HalfedgeId HalfedgeId;
    typedef HalfedgeConnectivity::Halfedge Halfedge;
    typedef HalfedgeConnectivity::VertexId VertexId;
    typedef HalfedgeConnectivity::FaceId FaceId;

    FaceCirculator(void) : mpConnectivity(0), mIsActive(false) {}

    FaceCirculator(HalfedgeConnectivity* pConnectivity, HalfedgeConnectivity::FaceId fid)
        :   mpConnectivity(pConnectivity), mStartHeId(pConnectivity->halfedgeId(fid)),
            mCurrentHeId(mStartHeId), mIsActive(false)
    {}


    /// Allows to start at a specific halfedge
    FaceCirculator(HalfedgeConnectivity* pConnectivity, HalfedgeConnectivity::HalfedgeId hidStart)
        :   mpConnectivity(pConnectivity), mStartHeId(hidStart),
            mCurrentHeId(mStartHeId), mIsActive(false)
    {}

    /// Copy constructor
    FaceCirculator(const FaceCirculator& vc)
         :  mpConnectivity(vc.mpConnectivity), mStartHeId(vc.mStartHeId),
            mCurrentHeId(vc.mCurrentHeId), mIsActive(vc.mIsActive)
    {}

    /// Assignment operator
    FaceCirculator& operator=(const FaceCirculator& vc)
    {
        mpConnectivity = vc.mpConnectivity;
        mStartHeId = vc.mStartHeId;
        mCurrentHeId = vc.mCurrentHeId;
        mIsActive = vc.mIsActive;
        return *this;
    }
    
    /*
    /// construct from non-const circulator type
    ConstFaceCirculator(const FaceCirculator& vc)
         :  mpConnectivity(vc.mpConnectivity), mStartHeId(vc.mStartHeId)
            mCurrentHeId(vc.mCurrentHeId), mIsActive(vc.mIsActive)
    {}

    /// assign from non-const circulator
    ConstFaceCirculator& operator=(const FaceCirculator& vc)
    {
        mpConnectivity(vc.mpConnectivity);
        mStartHeId(vc.mStartHeId);
        mCurrentHeId(vc.mCurrentHeId);
        mIsActive(vc.mIsActive);
        return *this;
    }
    */

    /// Equal ?
    bool operator==(const FaceCirculator& other) const
    {
        return ((mpConnectivity == other.mpConnectivity) && (mStartHeId==other.mStartHeId)
            && (mCurrentHeId==other.mCurrentHeId) && (mIsActive==other.mIsActive));
    }


    /// Not equal ?
    bool operator!=(const FaceCirculator& other) const {
        return !operator==(other);
    }

    /// Pre-Increment (next halfedge)
    FaceCirculator& operator++()
    {
        assert(mpConnectivity);
        mIsActive = true;
        mCurrentHeId = mpConnectivity->nextHalfedgeId(mCurrentHeId);
        return *this;
    }
    
    /// Pre-Decrement (previous halfedge)
    FaceCirculator& operator--()
    {
        assert(mpConnectivity);
        mIsActive = true;
        mCurrentHeId = mpConnectivity->previousHalfedgeId(mCurrentHeId);
        return *this;
    }

    /** Get the current halfedge index.
    */
    inline HalfedgeId halfedgeId(void) const { return mCurrentHeId; }
    inline const Halfedge& halfedge(void) const { return mpConnectivity->getHalfedge(mCurrentHeId); }
    inline Halfedge& halfedge(void) { return mpConnectivity->editHalfedge(mCurrentHeId); }
    
    /** Get the current opposite halfedge index.
    */
    inline HalfedgeId oppositeHalfedgeId(void) const { return mpConnectivity->oppositeHalfedgeId(mCurrentHeId); }
    inline const Halfedge& oppositeHalfedge(void) const { return mpConnectivity->getHalfedge(oppositeHalfedgeId()); }
    inline Halfedge& oppositeHalfedge(void) { return mpConnectivity->editHalfedge(oppositeHalfedgeId()); }
    
    /** Get the current vertex index
    */
    inline VertexId vertexId(void) const { return mpConnectivity->toVertexId(mCurrentHeId); }
    
    /** Get the adjacent face index
    */
    inline FaceId faceId(void) const { return mpConnectivity->faceId(oppositeHalfedgeId()); }
    
    /** Returns whether the circulator is still valid.
        After one complete round around a vertex/face the circulator becomes
        invalid, i.e. this function will return \c false. Nevertheless you
        can continue circulating. This method just tells you whether you
        have completed the first round.
    */
    inline operator bool() const
    {
        return mCurrentHeId.isValid() && ((mStartHeId != mCurrentHeId) || (!mIsActive));
    }

protected:

    HalfedgeConnectivity* mpConnectivity;
    HalfedgeId mStartHeId, mCurrentHeId;
    bool mIsActive;
};


}

}

#endif
