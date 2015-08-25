#include"Cache.h"
#include"Logger.h"

Cache::Cache( const std::string & hostname, int port)
{

	MY_LOG_DEBUG("Cache::Cache()");
	is_connecting = false;
	struct timeval timeout = {1,500000};
	rc = redisConnectWithTimeout( hostname.c_str(), port, timeout);
	if( NULL == rc || rc->err)
	{
		if( rc  )
		{
			is_connecting = true;
			MY_LOG_ERROR(" Cache::Cache() redisConnectWithTimeout() errro: %s", rc->errstr);
		}
		else
		{

			MY_LOG_ERROR(" Cache::Cache() redisConnectWithTimeout() errro");
		}
	}
	reply = NULL;		
	is_connecting = true;
}

Cache::~Cache()
{
	MY_LOG_DEBUG("Cache::~Cache()");
	if( is_connecting)
	{
		redisFree(rc);
		is_connecting = false;
	}
}


std::string Cache::query( const std::string  & qw) 
{
	MY_LOG_DEBUG("Cache::query() qw:%s",qw.c_str());
	if( is_connecting )
	{
		reply = (redisReply*) redisCommand( rc ," GET %s ", qw.c_str());
		if( NULL != reply 
				&&
				reply->type != REDIS_REPLY_NIL)		
		{
			std::string ret = reply->str;
			freeReplyObject(reply);
			MY_LOG_DEBUG("Cache::query() match qw=%s\ncache=%s", qw.c_str(), ret.c_str());
			return ret;
		}
	}

	MY_LOG_DEBUG("Cache::query() not match qw:%s", qw.c_str());
	std::string empty="";	
	return empty; 

}

void Cache::add_cache( const std::string & key, const std::string & value )
{
	MY_LOG_DEBUG("Cache::add_cache()");
	if( is_connecting)
	{
		reply =(redisReply*) redisCommand( rc ," SET %b %b ", key.c_str(), key.size(), value.c_str(), value.size());
		if( NULL == reply || 
				!( (reply->type == REDIS_REPLY_STATUS)
					&&
					(strcasecmp( reply->str, "OK")== 0)
				 )
		  )	
		{
			MY_LOG_ERROR(" redisCommand() failure: SET %s %s ", key.c_str(), value.c_str());
		}
		freeReplyObject(reply);
		MY_LOG_DEBUG("Cache::add_cache() sucess");
		return;
	}

	MY_LOG_DEBUG("Cache::add_cache() failed");
	return ;
}


