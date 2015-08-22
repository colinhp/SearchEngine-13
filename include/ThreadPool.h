#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include"Task.h"
#include"Mutex.h"
#include"Condition.h"
#include"Thread.h"
#include<vector>
#include<queue>
#include<memory>


class ThreadPool
{
	public:
	explicit ThreadPool( size_t queue_size, size_t threads_num );
	~ThreadPool();
	void start();
	void add_task( Task & task);
	Task get_task();
	void stop();
	
	void run_in_thread();

	private:
	mutable Mutex m_mutex;	
	Condition m_empty;
	Condition m_full;
	size_t m_queueSize;
	size_t m_threadNum ;
	std::queue< Task > m_taskQueue;	
	std::vector< std::unique_ptr< Thread> > m_threadsVec;
	bool m_starting;
};




#endif
