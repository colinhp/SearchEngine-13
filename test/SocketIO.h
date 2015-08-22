#ifndef _SOCKETIO_H_
#define _SOCKETIO_H_

#include<sys/types.h>

class SocketIO
{
	public:
		explicit SocketIO( int sockfd );
		
		ssize_t readn( char * buf, size_t count);
		ssize_t writen( const char * buf, size_t count);
		ssize_t readline( char *buf, size_t maxlen);
		ssize_t recv_peek( char * buf, size_t len);
	private:
		const int m_sockfd;


};




#endif
