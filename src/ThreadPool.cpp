#include"ThreadPool.h"
#include"Conf.h"
#include"Thread.h"
#include"InvertIdx.h"
#include"PageDB.h"
#include"MutexGuard.h"
#include"Task.h"
#include"Cache.h"
#include"Logger.h"
#include<cstdlib>
#include<memory>

ThreadPool::ThreadPool(size_t queue_size, size_t threads_num )
: m_mutex(),  
  m_empty( m_mutex), 
  m_full( m_mutex ), 
  m_queueSize( queue_size), 
  m_threadNum( threads_num ), 
  m_starting(false)
{
	MY_LOG_DEBUG("ThreadPool::ThreadPool()");

}

void ThreadPool::start()
{

	MY_LOG_DEBUG("ThreadPool::start()");
	if( m_starting == false)
	{
		for( size_t idx =0; idx != m_threadNum; idx++)
		{
			m_threadsVec.push_back( 
					std::unique_ptr< Thread > (
						new Thread( std::bind(& ThreadPool::run_in_thread, this))
						) );
		}

		m_starting = true;
		for( auto & v : m_threadsVec)
		{
			v->start();
		}
	}
}


void ThreadPool::stop()
{

	MY_LOG_DEBUG("ThreadPool::stop()");

	if( m_starting == false )
	{
		return ;	
	}
	else 
	{
		m_starting = false;
		m_empty.notify_all();
		m_full.notify_all();
		for( auto & v : m_threadsVec )
		{
			v->join()	;
		}
		m_threadsVec.clear();
	}

}


void ThreadPool::add_task( Task & task )
{
	MutexGuard guard( m_mutex);
	while( m_starting && m_taskQueue.size() >= m_queueSize)
	{
		m_empty.wait();
	}
	if( m_starting)
	{
		m_taskQueue.push( task );
		m_full.notify();

		MY_LOG_DEBUG("ThreadPool::add_task()");
	}
}


Task ThreadPool::get_task()
{
	MutexGuard guard(m_mutex);
	while(m_starting && m_taskQueue.empty())
	{
		m_full.wait();
	}
	if( m_starting)
	{
		Task task = m_taskQueue.front();
		m_taskQueue.pop();
		m_empty.notify();
		return task;
	}
	
	Task empty_task;
	return empty_task;

}



void ThreadPool::run_in_thread()
{

	MY_LOG_DEBUG("ThreadPool::run_in_thread()");

	const	Conf & cf = Conf::get_instance();
	int port = atoi( cf["redis_port"].c_str());
	Cache cache( cf["redis_host"], port);

	while( m_starting )	
	{
		Task task = get_task();
		if( m_starting)
		{
			task.execute( cache );
		}
	}
}


ThreadPool::~ThreadPool()
{
	MY_LOG_DEBUG("ThreadPool::~ThreadPool()");
	stop();
}



