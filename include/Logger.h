#ifndef _LOGGER_H_
#define _LOGGER_H_

#include"log4cpp/Category.hh"
#include"log4cpp/Appender.hh"
#include"log4cpp/FileAppender.hh"
#include"log4cpp/OstreamAppender.hh"
#include"log4cpp/StringQueueAppender.hh"
#include"log4cpp/RollingFileAppender.hh"
#include"log4cpp/SyslogAppender.hh"
#include"log4cpp/Layout.hh"
#include"log4cpp/BasicLayout.hh"
#include"log4cpp/SimpleLayout.hh"
#include"log4cpp/PatternLayout.hh"
#include"log4cpp/BasicConfigurator.hh"
#include"log4cpp/PropertyConfigurator.hh"
#include"log4cpp/Priority.hh"
#include"log4cpp/NDC.hh"

#include"Mutex.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>


#define MY_LOG_ERROR(...) \
	do { \
		Logger::get_instance().error(__VA_ARGS__);\
		exit(-1);\
	}while(0)

#define MY_LOG_WARN(...) \
	do { \
			Logger::get_instance().warn(__VA_ARGS__);\
	}while(0)

#define MY_LOG_NOTICE(...) \
	do { \
			Logger::get_instance().notice(__VA_ARGS__);\
	}while(0)

#define MY_LOG_INFO(...) \
	do { \
			Logger::get_instance().info(__VA_ARGS__);\
	}while(0)

#ifdef MY_DEBUG
    #define MY_LOG_DEBUG(...) \
     	do { \
	    		Logger::get_instance().debug(__VA_ARGS__);\
	    }while(0)
#else
    #define MY_LOG_DEBUG(...) 

#endif


class Logger
{
public:
	static log4cpp::Category & get_instance();
	static void init_instance();
	static void shut_down();
private:

	static log4cpp::Category * m_pCategory;
	static Mutex m_mutex;

};


#endif
