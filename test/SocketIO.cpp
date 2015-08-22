#include"SocketIO.h"
#include<unistd.h>
#include<sys/socket.h>
#include<errno.h>


SocketIO::SocketIO( int sockfd): m_sockfd(sockfd)
{

}

ssize_t SocketIO::readn( char * buf, size_t count)
{
	size_t nleft = count;
	ssize_t nread;
	char *pbuf = buf;

	while( count >0)
	{
		nread = read( m_sockfd, pbuf, nleft )	;
		if( nread ==-1)	
		{
			if( errno == EINTR )	
			{
				continue;	
			}
			return -1;
		}
		else if( nread == 0)
		{
			break;	
		}

		nleft -= nread;
		pbuf += nread;	
	}

	return count - nleft;
}


ssize_t SocketIO::writen( const char * buf ,size_t count)
{
	size_t nleft = count;
	ssize_t nwrite;
	const char * pbuf = buf;
	while( nleft > 0)
	{
		nwrite = write( m_sockfd, pbuf, nleft);
		if( nwrite <=0)	
		{
			if( nwrite == -1 && errno == EINTR)	
			{
				continue;	
			}
			return -1;
		}
		nleft -= nwrite;	
		pbuf += nwrite;
	}
	return count;
}

ssize_t SocketIO::readline( char *buf, size_t maxlen)
{
	size_t nleft = maxlen-1;
	char * pbuf = buf;
	size_t total =0;
	ssize_t nread;
	while( nleft >0)
	{
		nread = recv_peek(pbuf,nleft);
		if( nread <=0)	
			return nread;
		for( int i=0; i<nread; i++ )
		{
			if( pbuf[i]=='\n')		
			{
				size_t nsize = i+1;	
				if( readn(pbuf, nsize)!= static_cast<ssize_t>(nsize))	
					return -1;
				pbuf += nsize;
				total +=nsize;
				*pbuf ='\0';
				return total;
			}
		}
		
		if( readn(pbuf, nread) !=nread )
			return -1;
		pbuf +=nread;
		total +=nread;
		nleft -=nread;
	}
	*pbuf='\0';
	return maxlen -1;
}



ssize_t SocketIO::recv_peek( char * buf, size_t len)
{
	int nread;
	do
	{
		nread = recv( m_sockfd, buf, len ,MSG_PEEK)	;
	}
	while( nread ==-1 && errno ==EINTR);
	return nread;
}




