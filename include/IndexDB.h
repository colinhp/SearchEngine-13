#ifndef _INDEXDB_H_
#define _INDEXDB_H_

#include"IndexEntry.h"
#include"Mutex.h"
#include"Indexer.h"
#include<string>
#include<my_global.h>
#include<mysql.h>

//索引存储到数据库中
class IndexDB : public Indexer
{
	public:
	 IndexDB();
	 ~IndexDB();
	 bool add_index( const IndexEntry & idx );
	 bool get_index( IndexEntry & idx);
	 bool save(){ return true;}
	 size_t generate_id();

	private:
	MYSQL * m_conn;
	std::string m_db;
	std::string m_table;
	size_t m_count;
	Mutex m_mutex;

};




#endif
