#ifndef _PageDedup_H_
#define _PageDedup_H_

#include<unordered_set>
#include<string>
#include<fstream>


//MD5值128bit


class PageDedup
{
	public:
	explicit PageDedup( std::string file );
	~PageDedup();
	bool is_duplicate( const std::string & md5 );
	bool add( const std::string & md5 );

	private:
		std::fstream m_fs;
		std::unordered_set< std::string > m_md5Set;
};


#endif

