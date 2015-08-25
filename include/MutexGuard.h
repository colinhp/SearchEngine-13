#ifndef _MUTEXGUARD_H_
#define _MUTEXGUARD_H_

class Mutex;

class MutexGuard
{
	public:
		explicit  MutexGuard( Mutex & mutex);
		~MutexGuard();
	private:
		MutexGuard(const MutexGuard&)=delete;
		MutexGuard & operator=( const MutexGuard &)=delete;
		Mutex & m_mutex;
};

#endif
