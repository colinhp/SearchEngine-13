#include"MutexGuard.h"
#include"Mutex.h"

MutexGuard::MutexGuard( Mutex & mutex ):m_mutex(mutex)
{
	m_mutex.lock();
}

MutexGuard::~MutexGuard()
{
	m_mutex.unlock();
}



