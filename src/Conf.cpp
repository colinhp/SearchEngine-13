#include"Conf.h"
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<streambuf>
#include<sstream>
#include<iterator>
#include"MutexGuard.h"

#define MY_LOG_ERROR(msg)\
	do{\
		perror(msg);\
		exit(EXIT_FAILURE);\
	}while(0)


#define CONF_FILE "/zrepo/code/SearchEngine/data/config"

Conf * Conf::m_instance = NULL;
Mutex Conf::m_mutex;

void Conf::init_instance( const std::string & file  )
{
	MutexGuard guard(m_mutex);
	if( m_instance == NULL )	
	{
		m_instance = new Conf( file.c_str());
	}
}

const std::string  Conf::operator[]( const std::string & query) const
{
	auto it =conf_map.find(query);
	if( it != conf_map.end())	
		return it->second;
	else
	{
		std::string ret="";	
		return ret;
	}
}


const Conf & Conf::get_instance()
{
	if( m_instance == NULL )
	{
		MY_LOG_ERROR("Conf has not been initialized!");
	}
	return *m_instance;
}





Conf::Conf( const char * file)
{
	if( NULL == file)
		MY_LOG_ERROR("file");
	std::ifstream ifs( file );
	if( !ifs )
		MY_LOG_ERROR("Conf::Conf::ifstream");

	std::string line;
	std::string key;
	std::string value;
	while( getline( ifs, line))
	{
		if( line[0] == '#')
		{
			continue;	
		}
		std::istringstream iss(line);
		iss>>key>>value;
		conf_map[key] = value;
	}
	ifs.close();
}



