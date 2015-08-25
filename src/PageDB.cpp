#include"PageDB.h"
#include"Conf.h"
#include"MutexGuard.h"
#include"Logger.h"
#include"IndexEntry.h"
#include"Indexer.h"
#include"IndexFile.h"
#include"IndexDB.h"
#include"FileMgr.h"
#include<string.h>
#include<stdio.h>
#include<memory>
#include<cstdlib>
#include<functional>
#include<ios>
#include<sstream>
#include<fstream>


/*
 
PageDB * PageDB::m_instance = NULL;
Mutex PageDB::m_mutex;
PageDB::Garbage PageDB::gc;


void PageDB::init_instance()
{
	MY_LOG_DEBUG("PageDB::init()");

	MutexGuard guard(m_mutex);
	if( m_instance == NULL )
	{
		Conf cf = Conf::get_instance();
		m_instance = new PageDB( cf["pagedb_file"], cf["pagedbidx_use_file"] );


	}
	MY_LOG_DEBUG("PageDB::init() complete!");
}

void PageDB::shut_down()
{
	MY_LOG_DEBUG("PageDB::shut_down()");
	if( m_instance != NULL )
	{
		delete m_instance;	
		m_instance = NULL;	
	}
}

PageDB & PageDB::get_instance()
{
	if( m_instance == NULL)
	{
		init_instance()	;
	}
	return *m_instance;
}



PageDB::PageDB( const  std::string &  data_file, const std::string & index_file)
{
	MY_LOG_DEBUG("PageDB::PageDB()");
	m_dataStream.open( data_file, std::ios::in | std::ios::out | std::ios::binary );
	m_indexStream.open( index_file, std::ios::in | std::ios::out | std::ios::binary );
	if( m_dataStream.bad() || m_indexStream.bad() )
		MY_LOG_ERROR("data_file");

	size_t id, offset, length;
			
	m_count = 0;
	while( m_indexStream>>id>>offset>>length)
	{
		if( id > m_count )
			m_count = id;
		std::pair< size_t, size_t > second( offset, length);
		std::pair< size_t, std::pair< size_t, size_t > > val( id, second );
		m_indexMap.insert( val );
	}

	m_update = false;	
	MY_LOG_DEBUG("PageDB::PageDB() complete!");
}

PageDB::~PageDB()
{

	m_dataStream.close();
	m_indexStream.close();

}


void PageDB::save_to_file()
{
	if( m_update )
	{
//保存网页库文件和索引文件
		m_dataStream.flush();
		MY_LOG_DEBUG("save pagedb index");
		m_indexStream.clear();
		m_indexStream.seekp(0, std::ios::beg );
		for( const auto & v : m_indexMap  )
		{
			m_indexStream << v.first <<" "<< v.second.first << " " << v.second.second<<"\n";
		}
//		m_indexStream.flush();

		m_update = false;
	}

}


bool PageDB::add_page( const Page & page )
{

	MY_LOG_DEBUG("PageDB::add_page() %ld", page.m_id);

	m_dataStream.seekp(0, std::ios::end);
	size_t off1 = m_dataStream.tellp();	
	m_dataStream << page;
	size_t off2 = m_dataStream.tellp();

	std::pair< size_t, size_t> second( off1, off2-off1);
	std::pair< size_t, std::pair< size_t, size_t> > val( page.m_id, second);	
	m_indexMap.insert( val );
	if( m_dataStream.good() )
	{
		m_update = true;
		return true;
	}
	else
	{
		MY_LOG_DEBUG("PageDB::add_page() fail!");
		return false;
	}
}

bool PageDB::get_page( size_t id, Page & page )
{

	MutexGuard guard(m_mutex);

	auto it = m_indexMap.find(id);
	if( it != m_indexMap.end() )
	{
		std::unique_ptr<char> p( new char [ it->second.second] );
		if( !p )
			MY_LOG_ERROR("unique_ptr");

		m_dataStream.flush();
		m_dataStream.clear();
		m_dataStream.seekg( it->second.first, std::ios::beg);

		m_dataStream.read( p.get(), it->second.second );

		std::string tmp( p.g*et(), it->second.second );

		Page pg(tmp);
		
		page = pg;

		return true;

	}
	else
	{
		return false;	
	}

}

//生成文档id
size_t PageDB::generate_id()
{
	return m_count++;
}

*/
//
//
//
//



PageDB * PageDB::m_instance = NULL;
Mutex PageDB::m_mutex;
PageDB::Garbage PageDB::gc;


void PageDB::init_instance()
{
	MY_LOG_DEBUG("PageDB::init()");

	MutexGuard guard(m_mutex);
	if( m_instance == NULL )
	{
		Conf cf = Conf::get_instance();
		m_instance = new PageDB( cf["pagedb_file"], cf["pagedbidx_use_file"] );


	}
	MY_LOG_DEBUG("PageDB::init() complete!");
}

void PageDB::shut_down()
{
	MY_LOG_DEBUG("PageDB::shut_down()");
	if( m_instance != NULL )
	{
		delete m_instance;	
		m_instance = NULL;	
	}
}

PageDB & PageDB::get_instance()
{
	MY_LOG_DEBUG("PageDB::get_instance()");
	if( m_instance == NULL)
	{
		init_instance()	;
	}
	return *m_instance;
}

PageDB::~PageDB()
{

}


bool PageDB::add_page( const Page & page)
{
	MY_LOG_DEBUG("PageDB::add_page() id=%lu", page.m_id);
	IndexEntry idx;
	idx.m_id = page.m_id;
	if( m_fileMgr.add_page( page , idx ))
	{
		if(	m_indexer->add_index( idx))
		{
			m_update = true;
			return true;	
		}
	}
	return false;	
}


bool PageDB::get_page( size_t id , Page & page )
{
	MY_LOG_DEBUG("PageDB::get_page() id=%lu", id);
	IndexEntry idx; 
	idx.m_id = id ;
	if( m_indexer->get_index( idx ))	
	{
		if( m_fileMgr.get_page( idx, page ))	
		{
			return true;	
		}
	}
	return false;
}

PageDB::PageDB( const std::string & datafile, const std::string & indexfile): m_update(false),  m_fileMgr( datafile) 
{
	MY_LOG_DEBUG("PageDB::PageDB()");
	const Conf & cf = Conf::get_instance();
	if( cf["pagedbidx_use_db"] == "yes")	
	{
		m_indexer.reset(new IndexDB());
		if(!m_indexer)
		{
			MY_LOG_ERROR( "PageDB::PageDB() new IndexDB() error  ");
		}
	}
	else
	{
		m_indexer.reset(new IndexFile( indexfile ));
		if( !m_indexer)
		{
		
			MY_LOG_ERROR( "PageDB::PageDB() new IndexFile() error  ");
		}
	}

}


void PageDB::save_to_file()
{
	MY_LOG_DEBUG("PageDB::save_to_file()");
	if( m_update)
	{
		m_fileMgr.save_to_file();
		m_indexer->save();
		m_update =false;
		MY_LOG_DEBUG("PageDB::save_to_file() sucess!");
	}

	MY_LOG_DEBUG("PageDB::save_to_file() donot need to save!");
}


size_t PageDB::generate_id()
{
	MY_LOG_DEBUG("PageDB::generate_id()");
	return m_indexer->generate_id();
}


