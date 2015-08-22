#include"PageDedup.h"
#include"Logger.h"
#include<memory>


PageDedup::PageDedup( std::string file )
{

	MY_LOG_DEBUG("PageDedup::PageDedup()");
	m_fs.open( file , std::ios::in | std::ios::out | std::ios::binary );
	if( m_fs.fail() )
		MY_LOG_ERROR("fstream");
	std::unique_ptr< char > p( new char [16]);
	if( !p )
		MY_LOG_ERROR("unique_ptr");

	while( m_fs.read( p.get(), 16))
	{
		std::string tmp( p.get(), 16);
		m_md5Set.insert( tmp );
	
	}
	MY_LOG_DEBUG("PageDedup::PageDedup() complete");
}


PageDedup::~PageDedup()
{
	MY_LOG_DEBUG("PageDedup::~PageDedup()");

	m_fs.clear();
	m_fs.seekp(0,std::ios::beg);
	for( const auto & v : m_md5Set )
	{
		m_fs<<v;
	}
	m_fs.close();

	MY_LOG_DEBUG("PageDedup::~PageDedup() complete");
}

bool PageDedup::is_duplicate( const std::string & md5 )
{

	if( m_md5Set.find(md5) != m_md5Set.end() )
	{
		MY_LOG_DEBUG("md5 is duplicate");
		return true;
	}
	else
	{
		return false;
	}
}

bool PageDedup::add( const std::string & md5 )
{
	MY_LOG_DEBUG("PageDedup::add()");
	if( m_md5Set.insert( md5 ).second )
	{
		return true;
	}
	else
	{
		return false;	
	}
}



