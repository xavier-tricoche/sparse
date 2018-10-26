/*************************************************************************
sparse: Efficient Computation of the Flow Map from Sparse Samples

Author: Samer S. Barakat

Copyright (c) 2010-2013, Purdue University

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/
#ifndef MYOMPSEMAPHORE_H_
#define MYOMPSEMAPHORE_H_

#include <omp.h>

class MyOMPSemaphore
{
private:
    int value_;
    omp_lock_t waitlist_;
    omp_lock_t mutex_;

public:
	MyOMPSemaphore(int count) : value_(count)
	{
		assert(count >= 0);

		omp_init_lock(&waitlist_);
		omp_init_lock(&mutex_);
	}
	~MyOMPSemaphore(void)
	{
		omp_destroy_lock(&waitlist_);
		omp_destroy_lock(&mutex_);
	}

    void wait()
	{
		omp_set_lock(&mutex_);

		if(--value_ < 0) {
			omp_unset_lock(&mutex_); // we have to give up the lock if we are going to block
			block();
			omp_set_lock(&mutex_); // reacquire the lock for symmetry when we exit
		}

		omp_unset_lock(&mutex_)
	}

    void signal()
	{
		omp_set_lock(&mutex_);

		if(++value_ <= 0) {
			unblock();
		}

		omp_unset_lock(&mutex_)
	}

private:
    void block()
	{

	}

    void unblock()
	{

	}


};

#endif
