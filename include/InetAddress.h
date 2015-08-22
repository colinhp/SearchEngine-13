#ifndef _INETADDRESS_H_
#define _INETADDRESS_H_

#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>

class InetAddress
{
	public:
		explicit InetAddress( const std::string &ip, unsigned short  port );
		const struct sockaddr_in * get_sockaddr_in() const;

	private:
		struct sockaddr_in m_addr;
};



#endif
