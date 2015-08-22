#include"Logger.h"
#include"MutexGuard.h"
#include"Conf.h"
#include<stdlib.h>


log4cpp::Category* Logger::m_pCategory = NULL;
Mutex Logger::m_mutex;

void Logger::init_instance()
{
	MutexGuard guard(m_mutex);
	if( m_pCategory == NULL)
	{
		try
		{
			const Conf & conf = Conf::get_instance();
			log4cpp::PropertyConfigurator::configure( conf["log_config"] )	;
			m_pCategory = &log4cpp::Category::getRoot();
		}
		catch(log4cpp::ConfigureFailure & f)
		{
			std::cout<<"log4cpp Configure Problem"<<f.what()<<std::endl;
			exit( EXIT_FAILURE );
		}
	}
}

void Logger::shut_down()
{
	(*m_pCategory).info("shut down logger");
	log4cpp::NDC::clear();
	log4cpp::Category::shutdown();
}


log4cpp::Category & Logger::get_instance()
{
	if( m_pCategory == NULL)
	{
		init_instance()	;
	}
	return *m_pCategory;
}




