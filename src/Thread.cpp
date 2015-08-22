#include"Thread.h"
#include"Logger.h"


Thread::Thread( ThreadCallback callback ): m_callback( callback ), m_threadid(0), m_starting( false)
{

}

Thread::~Thread()
{
	if( m_starting)
	{
		pthread_detach( m_threadid)	;
	}

}

void Thread::start()
{
	MY_LOG_DEBUG("Thread::start() ");
	if( !m_starting)
	{
		pthread_create( & m_threadid, NULL, run_in_thread, this);
		m_starting = true;
	}
	MY_LOG_DEBUG("thread id = %lu ", m_threadid);

}

void Thread::join()
{
	if( m_starting)
	{
		pthread_join( m_threadid, NULL);
		m_starting = false;
	}
}

void * Thread::run_in_thread( void * arg)
{
	Thread * pthread = static_cast< Thread * >(arg);
	if( pthread->m_callback)
	{
		pthread->m_callback()	;
	}

	return NULL;
}


