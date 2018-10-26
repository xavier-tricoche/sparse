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



#ifndef _ExpeWeightingFunction_h_
#define _ExpeWeightingFunction_h_

#include "ExpeTypedObject.h"
#include "ExpeMath.h"
#include "ExpeQuickManager.h"

namespace Expe
{

/** 
*/


class WeightingFunction : public TypedObject
{
//Q_OBJECT

public:

    WeightingFunction() : mCutoff(1.0) {}
    
    virtual ~WeightingFunction();
    
    /** Returns the cut-off value of the weight function.
        It is usually equal to 1.
    */
    inline Real getCutoff(void) const {return mCutoff;}
    
    /** Compute the weights from the vector x2List to weightList.
        Note that implementations have to take care that the vectors
        can be the same.
    */
    virtual void computeWeights(const RealArray& x2List, RealArray& weightList) const = 0;
    
    /** Compute the derivatives of the weight function from the vector x2List to weightList.
    */
    virtual void computeDerivativeWeights(const RealArray& x2List, RealArray& derivativeWeightList) const = 0;
    
protected:

    Real mCutoff;

};

/** A macro for super lazy people which add a WeightingFunction and related members.
    This macro add:
    \code
    protected:
        WeightingFunction* mWeightingFunction;
    public:
        // useful for LazzyUI
        StringArray getAvailableWeightingFunctionTypes(void) const;
        // useful for LazzyUI
        const String& getCurrentWeightingFunctionType(void) const;
        void selectWeightingFunction(const String& type);
        WeightingFunction* editWeightingFunction(void);
    \endcode
*/
#define QUICK_WEIGHTINGFUNCTION_MEMBERS   \
    /** Returns the list of available WeightingFunction type names. \
        It is useful for LazzyUI. \
    */ \
    /*QStringList getAvailableWeightingFunctionTypes(void) const {return WeightingFunctionManager::Instance().getAvailableTypes(); } */ \
    /** Returns the type of the current weight function. \
    */ \
    QString getCurrentWeightingFunctionType(void) const {return mWeightingFunction->getType();} \
    /** Specifies the weight function to use from its type name. \
    */ \
    /*void selectWeightingFunction(const QString& type) { \
        WeightingFunction* pTry = WeightingFunctionManager::Instance().create(type); \
        if (pTry) { \
            delete mWeightingFunction; \
            mWeightingFunction = pTry; \
        } \
    }*/ \
	void setWeightingFunction(WeightingFunction* weightingfunction) { \
		mWeightingFunction = weightingfunction; \
	} \
    /** Returns a pointer to the current weighting function object. \
    */ \
    WeightingFunction* editWeightingFunction(void) {return mWeightingFunction;} \
    protected: \
    WeightingFunction* mWeightingFunction; \
    public:

/*class WeightingFunctionFactory : public QuickFactory
{
public:
    WeightingFunctionFactory(const QString& type, const QMetaObject* meta=0) : QuickFactory(type, meta) {}
    virtual ~WeightingFunctionFactory() {}
    virtual WeightingFunction* create(void) = 0;
};

template <class T> class WeightingFunctionFactoryCreator : public QuickFactoryCreator<WeightingFunctionFactory,T>
{
public:
    virtual ~WeightingFunctionFactoryCreator() {}
    
    virtual WeightingFunction* create(void)
    {
        return new T();
    }
};

typedef QuickManager<WeightingFunction,WeightingFunctionFactory> WeightingFunctionManager;

*/

/** Implements f(x) = (1-x^2)^4
*/
class Wf_OneMinusX2Power4 : public WeightingFunction
{
//Q_OBJECT
//Q_CLASSINFO("Type","OneMinusX2Power4")

public:

    Wf_OneMinusX2Power4() {}
    
    virtual ~Wf_OneMinusX2Power4() {}
    
    inline Real f2(Real x2) const
    {
        Real aux = Math::Clamp<Real>(1.-x2,0.,1.);
        aux = aux*aux;
        return aux*aux;
    }
    
    inline Real df2(Real x2) const
    {
        if (x2<=0. || x2>=1.)
            return 0.;
        Real aux = 1.-x2;
        //return -2. * Math::Sqrt(x2) * aux*aux*aux;
        return -4.*aux*aux*aux;
    }
    
    void computeWeights(const RealArray& x2List, RealArray& weightList) const
    {
        for (uint i=0 ; i<x2List.size() ; ++i)
        {
            weightList[i] = f2(x2List[i]);
        }
    }
    
    void computeDerivativeWeights(const RealArray& x2List, RealArray& derivativeWeightList) const
    {
        for (uint i=0 ; i<x2List.size() ; ++i)
        {
            derivativeWeightList[i] = df2(x2List[i]);
        }
    }
    
//     void computeWeights(const RealArray& x2List, RealArray& weightList, Real inv_h2) const
//     {
//         for (uint i=0 ; i<weightList.size() ; ++i)
//         {
//             weightList[i] = f(x2List[i]*inv_h2);
//         }
//     }
    

};

}

#endif

