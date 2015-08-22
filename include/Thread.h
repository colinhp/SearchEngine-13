#ifndef _THREAD_H_
#define _THREAD_H_

#include<memory>
#include<pthread.h>

class Thread
{
	public:	
		typedef std::function< void ( ) > ThreadCallback;
	explicit Thread( ThreadCallback callback);
		~Thread();
	
		void start();
		void join();

	private:
		static void * run_in_thread( void *);
		ThreadCallback m_callback;
		pthread_t m_threadid;
		bool  m_starting;

};

#endif
