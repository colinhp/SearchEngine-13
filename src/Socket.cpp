#include"Socket.h"
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netdb.h>
#include<string.h>


//#include"Logger.h"


#define MY_LOG_ERROR( msg  )\
	do{\
		perror( msg );\
		exit(EXIT_FAILURE);\
	}while(0);



Socket::Socket( int sockfd ): m_sockfd (sockfd)
{
}


void Socket::bind( const InetAddress &addr)
{
	if( -1 == ::bind(m_sockfd, (const struct sockaddr *)addr.get_sockaddr_in(), sizeof( addr )) )
	{
		MY_LOG_ERROR("Socket::bind() error ");
	}
}

void Socket::listen()
{
	if( -1 == ::listen( m_sockfd, SOMAXCONN))	
	{
		MY_LOG_ERROR("Socket::listen() error ");
	}

}

int Socket::accept()
{

	int fd = ::accept( m_sockfd, NULL, NULL);

	/*
	if( fd ==-1)
	{
		MY_LOG_ERROR("Socket::accept() error: %s ", strerror(errno) );
	}
	*/

	return fd;
}

int Socket::accept( std::string & ip,  int & port )
{
	struct sockaddr_in  peeraddr;
	int addrlen = sizeof( peeraddr );

	int fd = ::accept( m_sockfd, (struct sockaddr *)& peeraddr , (socklen_t *)&addrlen);
/*
	if( fd ==-1)
	{
		MY_LOG_ERROR("Socket::accept() error ");
	}
*/
	ip = inet_ntoa(peeraddr.sin_addr);
	port = ntohs(peeraddr.sin_port);
	return fd;



}


void Socket::shut_down_write() 
{
	if( -1 == ::shutdown(m_sockfd, SHUT_WR))
	{
		MY_LOG_ERROR("Socket::shut_down_write() error ");
	}
}


void Socket::set_nonblock()
{
	int flag = fcntl( m_sockfd, F_GETFL,0);
	if( flag <0 )
	{
		MY_LOG_ERROR("Socket::set_nonblock() error ");
	}
	flag |= O_NONBLOCK;
	if( fcntl( m_sockfd, F_SETFL, flag) < 0)
	{
		MY_LOG_ERROR("Socket::set_nonblock() error ");
	}
	return ;
}


void Socket::set_tcp_no_delay(bool on)
{  
	int optval = on? 1:0;
	int ret = setsockopt( m_sockfd, IPPROTO_TCP,  TCP_NODELAY, &optval, static_cast< socklen_t>(sizeof( optval)));
	if( ret == -1 )	
	{
		MY_LOG_ERROR("Socket::set_tcp_no_delay() error ");
	}

}

void Socket::set_reuse_addr(bool on)
{
	int optval = on ? 1 :0;
	int ret = setsockopt( m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>( sizeof(optval))) ;
	if( ret == -1)
	{
		MY_LOG_ERROR("Socket::set_reuse_addr() error ");
	}

}


void Socket::set_reuse_port(bool on)
{
	int optval = on? 1:0;
	int ret = setsockopt( m_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>( sizeof(optval))) ;
	if(ret == -1 )
	{
		MY_LOG_ERROR("Socket::set_reuse_port() error ");
	}
}


void Socket::set_keep_alive(bool on)
{
	int optval = on? 1:0;
	int ret = setsockopt( m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>( sizeof(optval))) ;
	if(ret == -1 )
	{
		MY_LOG_ERROR("Soccket::set_keep_alive() error ");
	}
}




