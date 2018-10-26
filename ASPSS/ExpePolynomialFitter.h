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



#ifndef _ExpePolynomialFitter_h_
#define _ExpePolynomialFitter_h_

#include <gsl/gsl_poly.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_multifit.h>

#include "ExpePrerequisites.h"

//#include <QObject>

namespace Expe
{

/** 
*/

class PolynomialFitter //: public QObject
{
//Q_OBJECT

public:

    PolynomialFitter(uint debree);
    
    virtual ~PolynomialFitter();
    
    //bool fit(const WheightedNeihgborhood& neighborhood);
    
protected:

    uint mDegree;
    
    Real* p;


	uint mNofCoeffs;
    gsl_vector* mCoeffVector;
    gsl_matrix* mCovarianceMatrix;

};

}

#endif

