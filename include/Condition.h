#ifndef _CONDITION_H_
#define _CONDITION_H_

#include<pthread.h>

class Mutex;

class Condition
{
	public:
		explicit Condition( Mutex & mutex);
		~Condition();
		void wait();
		void notify();
		void notify_all();
	private:
		Mutex & m_mutex;
		pthread_cond_t m_cond;

};



#endif
