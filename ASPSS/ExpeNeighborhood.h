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



#ifndef _ExpeNeighborhood_h_
#define _ExpeNeighborhood_h_

#include "ExpeTypedObject.h"
#include "ExpeQueryDataStructure.h"
#include "ExpePointSet.h"
//#include "ExpeQuickManager.h"

namespace Expe
{

class WeightingFunction;

/** Base class to perform 3D neighbors queries.
*/

class Neighborhood : public TypedObject
{
//Q_OBJECT

public:

    Neighborhood(ConstPointSetPtr pPoints);
    
    virtual ~Neighborhood();
    
    //inline const PointSet* getPoints(void) const {return mpPoints;}
    
    virtual void computeNeighborhood(const Vector3& p, const WeightingFunction* pWeightingFunc = 0) = 0;
    
    inline uint getNofFoundNeighbors(void) const;
    inline PointSet::ConstPointHandle getNeighbor(uint i) const;
    inline Index getNeighborId(uint i) const;
    inline Real getNeighborSquaredDistance(uint i) const;
    
    inline Real getNeighborWeight(uint i) const;
    inline Real getNeighborDerivativeWeight(uint i) const;
    
    Real getCurrentFilterRadius(void) const;
    void sortNeighbors(void);
    
protected:
    
    ConstPointSetPtr mpPoints;
    uint mNofFoundNeighbors;
    const QueryDataStructure::NeighborList* mpNeighborList;
    RealArray mWeights;
    RealArray mDerivativeWeights;
    Real mCurrentFilterRadius;
    Vector3 mQueriedPoint;
};


/** A macro for super lazy people which add a Neighborhood and related members.
    \param POINTSET the PointSet* member handled by the neighborhood.
    This macro add:
    \code
    protected:
        Neighborhood* mNeighborhood;
    public:
        // useful for LazzyUI
        StringArray getAvailableNeighborhoodTypes(void) const;
        // useful for LazzyUI
        const String& getCurrentNeighborhoodType(void) const;
        void selectNeighborhood(const String& type);
        Neighborhood* editNeighborhood(void);
    \endcode
*/
#define QUICK_NEIGHBORHOOD_MEMBERS(POINTSET)  \
    /** Returns the list of available Neighborhood type names. \
        It is useful for LazzyUI. \
    */ \
    /*QStringList getAvailableNeighborhoodTypes(void) const {return NeighborhoodManager::Instance().getAvailableTypes(); } */\
    /** Returns the type of the current Neighborhood. \
    */ \
    QString getCurrentNeighborhoodName(void) const {return mNeighborhood->getType();} \
    /** Specifies the Neighborhood to use from its type name. \
    */ \
    /*void selectNeighborhood(const QString& type) { \
        Neighborhood* pTry = NeighborhoodManager::Instance().create(type, POINTSET); \
        if (pTry) { \
            delete mNeighborhood; \
            mNeighborhood = pTry; \
        } \
    }*/ \
	void setNeighborhood(Neighborhood* neighborhood) { \
		mNeighborhood = neighborhood; \
	} \
    /** Returns a pointer to the current Neighborhood object. \
    */ \
    Neighborhood* editNeighborhood(void) {return mNeighborhood;} \
    protected: \
    Neighborhood* mNeighborhood; \
    public:

/** Macro 
*/
#define QUICK_REGISTER_NEIGHBORHOOD_PROPERTY(CAT)  \
    REGISTER_PROPERTY_RW_META("Neighborhood", QString, CAT, "",getCurrentNeighborhoodName, selectNeighborhood, META_STRING_RESTRICTED(getAvailableNeighborhoodNames)); \
    REGISTER_PROPERTY_RO("NeighborhoodObject", Object, CAT, "", editNeighborhood);
    
//----------
/*
class NeighborhoodFactory : public QuickFactory
{
public:
    NeighborhoodFactory(const QString& type, const QMetaObject* meta=0) : QuickFactory(type, meta) {}
    virtual ~NeighborhoodFactory() {}
    virtual Neighborhood* create(ConstPointSetPtr pPoints) = 0;
};

template <class T> class NeighborhoodFactoryCreator : public QuickFactoryCreator<NeighborhoodFactory,T>
{
public:
    virtual ~NeighborhoodFactoryCreator() {}
    
    virtual Neighborhood* create(ConstPointSetPtr pPoints)
    {
        return new T(pPoints);
    }
};


typedef QuickManager<Neighborhood,NeighborhoodFactory> NeighborhoodManager;
*/
#include "ExpeNeighborhood.inl"

}

#endif

