#ifndef _EPOLLPOLLER_H_
#define _EPOLLPOLLER_H_

#include<vector>
#include<sys/epoll.h>
#include<string>

#include"ThreadPool.h"


class EpollPoller
{
	public:
		explicit EpollPoller( const std::string & ip, unsigned int  port  , ThreadPool & threadpool);

		~EpollPoller();
		void add_readfd( int fd );
		void del_readfd( int fd );
		void handle_connection();
		void handle_message( int fd );

		void loop( );
		void unloop();

	private:
		void wait_epollfd();
		ThreadPool & m_threadpool;
		int m_epollfd;
		int m_listenfd;
		bool m_looping;
		std::vector< struct epoll_event> m_events;
};


#endif
