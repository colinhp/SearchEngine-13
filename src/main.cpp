#include"Building.h"
#include"Logger.h"
#include"Conf.h"
#include"Split.h"
#include"PageDB.h"
#include"InvertIdx.h"
#include"EpollPoller.h"
#include"ThreadPool.h"
#include<cstdlib>
#include<iostream>



//配置文件
#define CONF_FILE "/zrepo/code/SearchEngine/data/config"





int main()
{


	Conf::init_instance( CONF_FILE );

	my_daemon();

	Logger::init_instance();
	Split::init_instance();
	PageDB::init_instance();
	InvertIdx::init_instance();

	int start = clock();
	initiation();
	printf("%.3lf second\nInitialization completed\n", double( clock()-start)/ CLOCKS_PER_SEC);


	Conf cf = Conf::get_instance();
	size_t queuesize = strtoul( cf["queuesize"].c_str() ,NULL, 0);
	size_t threadsnum = strtoul( cf["threadsnum"].c_str(), NULL, 0 ); 
	ThreadPool  threadpool( queuesize, threadsnum  );		
	threadpool.start();

	unsigned short port = atoi( cf["port"].c_str() );
	EpollPoller epoller( cf["ip"], port,  threadpool );
	epoller.loop();

	epoller.unloop();
	threadpool.stop();


	InvertIdx::shut_down();
	PageDB::shut_down();
	Split::shut_down();
	Logger::shut_down();

	return 0;

}
