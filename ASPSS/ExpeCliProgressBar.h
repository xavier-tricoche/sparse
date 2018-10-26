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



#ifndef _ExpeCliProgressBar_h_ 
#define _ExpeCliProgressBar_h_

#include "ExpePrerequisites.h"
#include <stdio.h>

namespace Expe
{

template <typename T> class CliProgressBarT
{
public:

    CliProgressBarT(T start, T end)
        : mStart(start), mEnd(end)
    {
        mPercent = 0;
    }
    
    void update(const T& value)
    {
        int newPercent = (int)((T)(100)*(value - mStart)/(mEnd-mStart));
        if(newPercent-mPercent != 0)
        {
            mPercent = newPercent;
            // refresh
            std::cout << "\r\r\r\r\r" << mPercent << " %";
            fflush(stdout);
        }
    }
    
    ~CliProgressBarT() {}

protected:

    T mStart;
    T mEnd;
    int mPercent;

};

}

#endif

