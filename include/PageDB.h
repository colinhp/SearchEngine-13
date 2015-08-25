#ifndef _PAGEDB_H_
#define _PAGEDB_H_

#include"Page.h"
#include"Mutex.h"
#include"IndexEntry.h"
#include"Indexer.h"
#include"IndexFile.h"
#include"IndexDB.h"
#include"FileMgr.h"
#include<string>
#include<vector>
#include<unordered_map>
#include<utility>
#include<memory>
#include<map>
#include<fstream>
#include<functional>
#include<my_global.h>
#include<mysql.h>



/* 
//索引文件格式
//id, offset, length


class PageDB
{
public:

	bool add_page( const Page & page);
	bool get_page( size_t id, Page & page );
	size_t generate_id();
	void save_to_file();


	PageDB() = delete;
	PageDB( const PageDB &  ) = delete;
	PageDB & operator=( const PageDB & ) = delete;

	static PageDB & get_instance();
	static void init_instance();
	static void shut_down();


private:

	explicit PageDB( const std::string & data_file, const std::string & index_file );
	~PageDB();

	static PageDB * m_instance; 
	static Mutex m_mutex;
	size_t m_count;
	bool  m_update;
	std::fstream  m_dataStream;
	std::fstream  m_indexStream;
	std::unordered_map< size_t , std::pair< size_t, size_t> > m_indexMap;
	
	class Garbage
	{
		public:
			~Garbage()	
			{
				if( m_instance != NULL )	
				{
					delete m_instance;	
					m_instance =NULL;
				}
			}
	};
	static Garbage gc;
	

};

*/


//单例模式,网页库数据类
class PageDB
{
public:

	bool add_page( const Page & page);
	bool get_page( size_t id, Page & page );
	size_t generate_id();
	void save_to_file();


	PageDB() = delete;
	PageDB( const PageDB &  ) = delete;
	PageDB & operator=( const PageDB & ) = delete;

	static PageDB & get_instance();
	static void init_instance();
	static void shut_down();


private:

	explicit PageDB(  const std::string & datafile , const std::string & indexfile );
	~PageDB();

	static PageDB * m_instance; 
	static Mutex m_mutex;
	bool  m_update;

	FileMgr  m_fileMgr;
	std::shared_ptr< Indexer >   m_indexer;
	
	class Garbage
	{
		public:
			~Garbage()	
			{
				if( m_instance != NULL )	
				{
					delete m_instance;	
					m_instance =NULL;
				}
			}
	};
	static Garbage gc;
	

};


#endif
