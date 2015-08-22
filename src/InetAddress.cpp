#include"InetAddress.h"
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
//#include"Logger.h"



#define MY_LOG_ERROR( msg ) \
	do{\
		perror( msg) ; \
		exit( EXIT_FAILURE); \
	}while(0);



InetAddress::InetAddress( const std::string &ip, unsigned short port)
{
//	MY_LOG_DEBUG("InetAddress::InetAddress()  ip: %s, port= %d " ,ip.c_str(), port);
	memset(&m_addr, 0, sizeof( m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);
	if( inet_aton( ip.c_str(), & m_addr.sin_addr )==0)
	{
		MY_LOG_ERROR(" ip invalid ");
	}	

}

const struct sockaddr_in * InetAddress::get_sockaddr_in() const 
{
	return & m_addr;
}








