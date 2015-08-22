#ifndef _SOCKET_H_
#define _SOCKET_H_

#include"InetAddress.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/tcp.h>



class Socket
{
public:
	explicit Socket( int sockfd);
	void bind( const InetAddress & addr);
	void listen();
	int accept();
	int accept( std::string & ip, int & port );

	void shut_down_write();
	void set_nonblock();
	void set_tcp_no_delay( bool on);
	void set_reuse_addr(bool on);
	void set_reuse_port(bool on);
	void set_keep_alive(bool on);
private:
	const int m_sockfd;

};



#endif
