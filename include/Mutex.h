#ifndef _MUTEX_H_
#define _MUTEX_H_

#include<pthread.h>

class Mutex
{
	public:
		explicit Mutex();
		~Mutex();
		void lock();
		void unlock();
		pthread_mutex_t * get_mutex_ptr();

	private:
		pthread_mutex_t m_mutex;

};

#endif
