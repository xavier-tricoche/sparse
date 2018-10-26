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



#ifndef _ExpeMultiSet_h_ 
#define _ExpeMultiSet_h_

#include "ExpePrerequisites.h"

namespace Expe
{

/** Handle in a single 32 bits integer value the index of the set
    and the index of the indexed element in this set.
    Therefore, the number of sets is limited to 256 and the number of elements in a set is limited to 16,777,216
*/
class MultiIndex
{
public:
    MultiIndex() : mMultiIndex(0xffffffff) {}
//     explicit MultiIndex(uint elementId) : mMultiIndex(elementId) { assert(elementId<=0xfffff); }
    MultiIndex(uint setId, uint elementId) : mMultiIndex((setId<<24) | elementId)
    {
        assert(setId<=0xff);
        assert(elementId<=0xfffff);
    }
    
    inline uint getSetId(void) const { return (mMultiIndex & 0xff000000) >> 24; }
    inline uint getElementId(void) const { return (mMultiIndex & 0x00ffffff); }
    
    inline bool operator == (const MultiIndex& mi) const { return mMultiIndex == mi.mMultiIndex; }
    inline bool operator!= (const MultiIndex& mi) const { return mMultiIndex!=mi.mMultiIndex; }
    inline bool operator< (const MultiIndex& mi) const { return mMultiIndex<mi.mMultiIndex; }
    inline bool operator> (const MultiIndex& mi) const { return mMultiIndex>mi.mMultiIndex; }
    
    inline bool isValid(void) const { return mMultiIndex != 0xffffffff; }
    inline uint _asInt(void) { return mMultiIndex; }
    
protected:
    uint mMultiIndex;
};

#define EXPE_DECLARE_NEW_MULTIINDEX(TYPENAME) class TYPENAME : public MultiIndex { public: \
    inline TYPENAME(void) : MultiIndex() {} \
    inline TYPENAME(uint setId, uint elementId) : MultiIndex(setId,elementId) {} \
}

/** \class MultiSet
    \brief Aims to facilitate the manipulation of different set as a single one.
    
    For instance this class is used to manipulate sets of points having a variable number of normals or to unify the faces of a Mesh.
    
    \todo Add Iterator support.
*/
template <typename SetT,typename ElementT,typename ConstElementT,class MultiIndexT=MultiIndex> class MultiSet
{
public:
    MultiSet()
    {}

    ~MultiSet()
    {}

    inline ConstElementT at(MultiIndexT mid) const { return const_cast<const SetT*>(mpSets[mid.getSetId()])->at(mid.getElementId()); }

    inline ElementT at(MultiIndexT mid) { return mpSets[mid.getSetId()]->at(mid.getElementId()); }

    inline ConstElementT at(uint sid, uint eid) const { return mpSets[sid]->at(eid); }
    
    inline void clear(void) { mpSets.clear(); }
    
    inline ElementT at(uint sid, uint eid) { return mpSets[sid]->at(eid); }
    
    inline bool isInRange(MultiIndexT mid) const {
        uint sid = mid.getSetId();
        return sid<mpSets.size() && mid.getElementId()<mpSets[sid]->size();
    }

    /** Append a new set in the MultiSet.
    */
    void add(SetT* pSet) { mpSets.push_back(pSet); }

    inline uint getNofSets(void) const { return mpSets.size(); }

    inline SetT* editSet(uint sid) { return mpSets[sid]; }

    inline const SetT* getSet(uint sid) const { return mpSets[sid]; }

    /** Create a MultiIndex from the index of the set and the index of the element in the index.
    */
    inline MultiIndexT compose(uint sid, uint eid) const { return MultiIndexT(sid,eid); }

    /** Return the total number of elements in the MultiSet.
        That is the sum of number of points in each sub set.
    */
    inline uint size(void) const
    {
        uint aux=0;
        for(uint i=0 ; i<getNofSets() ; ++i)
            aux += mpSets[i]->size();
        return aux;
    }
    
    // STL-like iterator support
    
    class iterator
    {
    friend class MultiSet<SetT,ElementT,ConstElementT,MultiIndexT>;
    public:
        iterator(void) {}
        
        inline ElementT operator*() const { return mMultiset.at(mSId,mEId); }
        /*inline ElementT* operator->() const { return &mMultiset.at(mSId,mEId); }*/
        inline MultiIndexT id(void) const { return MultiIndexT(mSId,mEId); }
        
        inline iterator& operator++ (void)
        {
            assert(mSId<int(mMultiset.mpSets.size()) && mEId<int(mMultiset.mpSets[mSId]->size()) && "you probably try to increment the \"end\" iterator or you have an invalid iterator");
            
            mEId++;
            if (mEId==int(mMultiset.mpSets[mSId]->size()))
            {
                mEId=0;
                mSId++;
            }
            return *this;
        }
        
        inline iterator& operator-- (void)
        {
            mEId--;
            if (mEId<0)
            {
                mSId--;
                assert(mSId>0 && "you probably tried to decrement the \"begin\" iterator");
                mEId = mMultiset.mpSets[mSId]->size()-1;
            }
            return *this;
        }
        
        inline bool operator== (const iterator& it) const { return _asUint64==it._asUint64; }
        inline bool operator!= (const iterator& it) const { return _asUint64!=it._asUint64; }
        inline bool operator< (const iterator& it) const { return _asUint64<it._asUint64; }
        inline bool operator> (const iterator& it) const { return _asUint64>it._asUint64; }
        
    protected:
        iterator(MultiSet& multiSet, uint sid, uint eid)
            : mMultiset(multiSet), mSId(sid), mEId(eid)
        {}
        
        MultiSet& mMultiset;
        union
        {
            struct {int mSId, mEId;};
            uint64 _asUint64;
        };
    };
    
    inline iterator begin(void) {return iterator(*this,0,0);}
    inline iterator end(void) {return iterator(*this,mpSets.size(),0);}

protected:

    std::vector<SetT*> mpSets;
};

}

#endif
