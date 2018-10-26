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



#ifndef _ExpeTimer_h_ 
#define _ExpeTimer_h_

#include "ExpePrerequisites.h"
#include <QString>

#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

namespace Expe
{

class Timer
{
public:

    typedef double Time;

    Timer()
    {
        reset();
    }
    
    ~Timer()
    {
    }
    
    inline void start(void)
    {
        mLast = getTime();
    }
    inline void stop(void)
    {
        mSum += getTime() - mLast;
    }
    inline void reset(void)
    {
        mSum = 0.;
    }
    
    /** Return the elasped time.
    */
    inline Time value(void)
    {
        return mSum;
    }
    
    inline QString toHMS(void)
    {
        Time remains = mSum;
        int nbHours = int(remains/3600.);
        remains -= Time(nbHours*3600);
        int nbMinutes = int(remains/60.);
        remains -= Time(nbMinutes*60);
        int nbSeconds = int(remains);
        remains -= Time(nbSeconds);
        Real nbMiliSeconds = remains*1000.;
        QString str = "";
        if(nbHours>0)
        {
            str += QString::number(nbHours) + " h ";
            str += QString::number(nbMinutes) + " m ";
            str += QString::number(nbSeconds) + " s ";
        }
        else if(nbMinutes>0)
        {
            str += QString::number(nbMinutes) + " m ";
            str += QString::number(Real(nbSeconds) + nbMiliSeconds/1000.f) + " s ";
        }
        else if(nbSeconds>0)
        {
            str += QString::number(Real(nbSeconds) + nbMiliSeconds/1000.f) + " s ";
        }
        else
        {
            str += QString::number(nbMiliSeconds) + " ms";
        }
        return str;
    }
    
    /** Return the current elasped time (and memorizes it too).
    */
    inline Time grab(void)
    {
        Time t = getTime();
        mSum += t - mLast;
        mLast = t;
        return mSum;
    }
    
    static Time getTime(void)
    {
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);
        return (Time) ((double)tv.tv_sec + 1.e-6 * (double)tv.tv_usec);
    }

protected:

    Time mSum, mLast;

};

}

#endif

