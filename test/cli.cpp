#include<iostream>
#include<string>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include"SocketIO.h"
#include"json/json.h"
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>


#define MY_LOG_ERROR(msg)\
	do{\
		perror(msg);\
		exit(EXIT_FAILURE);\
	}while(0)



void client()
{
	int sfd  = socket(AF_INET, SOCK_STREAM, 0 );
	if( sfd == -1 )
		MY_LOG_ERROR("socket error");
	std::string ip = "127.0.0.1";
	unsigned int port = 8088;

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	
	if( -1 == connect( sfd,(const struct sockaddr *) &addr, sizeof(addr)))
		MY_LOG_ERROR("connect error");
	
	char * buf = new char [1024*1024];
	int num=0;
	while(1)
	{
		std::string query;	
		std::cin>> query;
		query +="\r\n";
		std::cout<<num<< "  you have input "<<std::endl;
		printf("%s", query.c_str());
		num++;

		int ret = write( sfd, query.c_str(), query.size());
		std::cout<<"write n "<<ret<<std::endl;
		if( -1 == ret )
			MY_LOG_ERROR("read");
		memset(buf, 0, 1024*100);
		ret = read(sfd, buf,1024*100);
		std::cout<<"read n "<<ret<<std::endl;
		if( -1 == ret )
			MY_LOG_ERROR("read");
				
		Json::Reader reader;
		Json::Value jres;
		reader.parse(buf, jres );
		Json::StyledWriter writer;
	
		std::cout<<jres.toStyledString();
		std::cout<<std::endl;

	}


}


int main()
{

	client();

}



