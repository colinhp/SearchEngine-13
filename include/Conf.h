#ifndef  _CONF_H_
#define _CONF_H_

#include<string>
#include<unordered_map>
#include"Mutex.h"

//单例模式，作为全局配置类使用

class Conf
{
	public:
		static void init_instance( const std::string & file );
		static const  Conf & get_instance();
		const std::string  operator[]( const std::string & query) const;

	private:
		static Conf * m_instance;
		static Mutex m_mutex;
		Conf( const char * file );
		std::unordered_map< std::string, std::string > conf_map;

};


#endif
