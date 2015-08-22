#ifndef _CACHE_H_
#define _CACHE_H_

#include"hiredis.h"
#include<string>


//用redis作为缓存, 保存的是json格式字符串
class Cache
{
	public:
	explicit Cache( const std::string & hostname, int port);
	~Cache();
	std::string query( const std::string & qw );
	void add_cache( const std::string  & key, const std::string & value );

	private:
	redisContext *rc;
	redisReply * reply;
	bool is_connecting;

};



#endif
