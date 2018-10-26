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



#ifndef _ExpePrioritySet_h_
#define _ExpePrioritySet_h_

#include "ExpePrerequisites.h"
#include "ExpeLogManager.h"

#include <vector>
#include <algorithm>

namespace Expe
{

// template <class ContainerT> PriorityQueueHelper
// {
// public:
// 
//     typedef typename ContainerT::value_type Element;
//     typedef typename Element::Index Index;
//     typedef typename Element::Weight Weight;
//     
//     restoreMin
// };

template <class ContainerT> class PriorityQueueWrapper
{
public:

    typedef typename ContainerT::value_type Element;
    typedef typename Element::Index Index;
    typedef typename Element::Weight Weight;
    
    PriorityQueueWrapper(ContainerT& container) : mContainer(container) {}
    ~PriorityQueueWrapper() {}
    
    void reserve(int n) { mContainer.reserve(n); }
    
    /** Set the fixed size
    */
    void setSize(int maxSize)
    {
        mContainer.resize(maxSize);
        init();
    }

    /** initialize queue with zero elements
    */
    inline void init()
    {
        mNofElements = mContainer.size();
        mCount = 0;
    }

    /** returns true if queue is empty
    */
    inline bool isEmpty() const
    {
        return mCount == 0;
    }

    /** returns true if queue is full
    */
    inline bool isFull() const
    {
        return mCount == mNofElements;
    }

    /** returns number of elements in queue
    */
    inline int getNofElements() const
    {
        return mCount;
    }
    
    inline const Element& operator[] (uint i) const
    {
        return mContainer[i];
    }

    /** gets the index of the element with minimal/maximal weight in O(1)
    */
    inline Index getTopIndex() const
    {
        assert(!isEmpty());
        return mContainer[0].index;
    }

    /** gets the minimal/maximal weight in O(1)
    */
    inline Weight getTopWeight() const
    {
        assert(!isEmpty());
        return mContainer[0].weight;
    }

    /** gets all elements in the queue
    */
    inline Element* getData()
    {
        return &(mContainer[0]);
    }

    /** sorts all elements
    */
    inline void sort(void)
    {
        typename std::vector<Element>::iterator start = mContainer.begin();
        typename std::vector<Element>::iterator end = mContainer.begin();
        end += mCount;
        std::sort(mContainer.begin(),end);
    }

protected:

    ContainerT& mContainer;
    int mNofElements;
    int mCount;

    inline Element* getOffsetedElements() { return static_cast<Element*>(getData()-1); }
    
    inline void swap(Element* m1, Element* m2)
    {
        Element tmp;
        tmp = *m2;
        *m2 = *m1;
        *m1 = tmp;
    }
};


template <class ContainerT> class MinPriorityQueueWrapper : public PriorityQueueWrapper<ContainerT>
{
    typedef PriorityQueueWrapper<ContainerT> Super;
    
public:

    typedef typename ContainerT::value_type Element;
    typedef typename Element::Index Index;
    typedef typename Element::Weight Weight;
    
    MinPriorityQueueWrapper(ContainerT& container) : Super(container) {}
    ~MinPriorityQueueWrapper() {}
    
    /** inserts a new element in O(log n)
    */
    inline void insert(Index index, Weight weight)
    {
        if (Super::isFull())
        {
            if (weight>Super::mContainer[0].weight)
            {
                // if full replace min
                Super::mContainer[0].index = index;
                Super::mContainer[0].weight = weight;
                restore(1, Super::mNofElements);
            }
        }
        else
        {
            Super::mContainer[Super::mCount].index = index;
            Super::mContainer[Super::mCount].weight = weight;
            Super::mCount++;
            register int i = Super::mCount;
            Element* pElements = Super::getOffsetedElements();
            while (i>1 && (pElements[i].weight < pElements[i>>1].weight))
            {
                swap(&pElements[i], &pElements[i>>1]);
                i >>= 1;
            }
        }
    }
    
    /** deletes the element with minimal/maximal weight in O(log n)
    */
    inline void removeTop()
    {
        assert(!Super::isEmpty());
        Super::mContainer[0] = Super::mContainer[Super::mCount-1];
        restore(1, Super::mCount);
        Super::mCount--;
    }
    
protected:

    inline void restore(register int L, register int R)
    {
        register int R1 = R>>1;
        register int i, j;
        Element* pElements = Super::getOffsetedElements();
        i = L;
        while (i <= R1)
        {
            if ( (2*i < R) && (pElements[2*i+1].weight < pElements[2*i].weight) )
                j = 2*i+1;
            else
                j = 2*i;
            if (pElements[j].weight < pElements[i].weight)
            {
                swap(&pElements[j], &pElements[i]);
                int tmp = i; i = j; j = tmp;
            }
            else
                i = R;
        }
    }

};

template <class ContainerT> class MaxPriorityQueueWrapper : public PriorityQueueWrapper<ContainerT>
{

    typedef PriorityQueueWrapper<ContainerT> Super;
    
public:

    typedef typename ContainerT::value_type Element;
    typedef typename Element::Index Index;
    typedef typename Element::Weight Weight;
    
    MaxPriorityQueueWrapper(ContainerT& container) : Super(container) {}
    ~MaxPriorityQueueWrapper() {}
    
    /** inserts a new element in O(log n)
    */
    inline void insert(Index index, Weight weight)
    {
        if (Super::isFull())
        {
            if (weight<Super::mContainer[0].weight)
            {
                // if full replace max
                Super::mContainer[0].index = index;
                Super::mContainer[0].weight = weight;
                restore(1, Super::mNofElements);
            }
        }
        else
        {
            Super::mContainer[Super::mCount].index = index;
            Super::mContainer[Super::mCount].weight = weight;
            Super::mCount++;
            Element* pElements = Super::getOffsetedElements();
            register int i = Super::mCount;
            while (i>1 && (pElements[i].weight > pElements[(i>>1)].weight))
            {
                Super::swap(&pElements[i], &pElements[(i>>1)]);
                i >>= 1;
            }
        }
    }
    
    /** deletes the element with minimal/maximal weight in O(log n)
    */
    inline void removeTop()
    {
        assert(!Super::isEmpty());
        Super::mContainer[0] = Super::mContainer[Super::mCount-1];
        restore(1, Super::mCount);
        Super::mCount--;
    }
    
protected:

    inline void restore(register int L, register int R)
    {
        Element* pElements = Super::getOffsetedElements();
        register int R1 = R>>1;
        register int i, j;
        i = L;
        while (i <= R1)
        {
            if ( (2*i < R) && (pElements[2*i+1].weight > pElements[2*i].weight) )
                j = 2*i+1;
            else
                j = 2*i;
            if (pElements[j].weight > pElements[i].weight)
            {
                Super::swap(&pElements[j], &pElements[i]);
                int tmp = i; i = j; j = tmp;
            }
            else
                i = R;
        }
    }

};


template<class IndexT, class WeightT> struct PriorityQueueElement
{
    typedef IndexT Index;
    typedef WeightT Weight;
    
    PriorityQueueElement() {};
    PriorityQueueElement(Index i, Weight w) : index(i), weight(w) {}
    
    Index index;
    Weight weight;
    bool operator < (const PriorityQueueElement& el) const
    {
        return weight < el.weight;
    }
};


/** Default min priority queue
*/
template<class Index, class Weight> class MinPriorityQueue : public MinPriorityQueueWrapper< std::vector< PriorityQueueElement<Index,Weight> > >
{

    typedef MinPriorityQueueWrapper< std::vector< PriorityQueueElement<Index,Weight> > > Super;

public:

    typedef PriorityQueueElement<Index,Weight> Element;

    MinPriorityQueue() : Super(mVector) {}

    ~MinPriorityQueue() {}

protected:

    std::vector<Element> mVector;
};


/** Default max priority queue
*/
template<class Index, class Weight> class MaxPriorityQueue : public MaxPriorityQueueWrapper< std::vector< PriorityQueueElement<Index,Weight> > >
{

    typedef MaxPriorityQueueWrapper< std::vector< PriorityQueueElement<Index,Weight> > > Super;

public:

    typedef PriorityQueueElement<Index,Weight> Element;

    MaxPriorityQueue() : Super(mVector) {}

    ~MaxPriorityQueue() {}

protected:

    std::vector<Element> mVector;
};


}

#endif
