#include"Condition.h"
#include"Mutex.h"

Condition::Condition( Mutex & mutex):m_mutex(mutex)
{
	pthread_cond_init( &m_cond, NULL);
}	

Condition::~Condition()
{
	pthread_cond_destroy(&m_cond);
}

void Condition::wait()
{
	pthread_cond_wait( &m_cond, m_mutex.get_mutex_ptr());

}

void Condition::notify()
{

	pthread_cond_signal(&m_cond);
}


void Condition::notify_all()
{
		
	pthread_cond_broadcast( &m_cond );

}

