#include"PageDB.h"
#include"Conf.h"
#include"MutexGuard.h"
#include"Logger.h"
#include<string.h>
#include<stdio.h>
#include<memory>
#include<cstdlib>
#include<functional>
#include<ios>
#include<sstream>
#include<fstream>

#include<iostream>

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


IndexEntry::IndexEntry( const std::string &str)
{
	std::istringstream iss(str);
	std::string hash;
	iss>>m_id>>m_offset>>m_length>>hash>>m_filenum;
	hex_string_to_hash(hash);
}

//hash转换为hex字符串
std::string IndexEntry::hash_to_hex_string() const
{
	std::string hash;
	for( int i = 0 ; i< 16; i++)
	{
		int h = m_contenthash[i]/16;
		int l = m_contenthash[i]%16;
		unsigned char  chigh = h >= 10 ? h-10+'a' : h-0 + '0';
		unsigned char  clow = l >= 10 ? l-10+'a' : l-0 + '0';
		hash.append(1,chigh);
		hash.append(1, clow);
	}
	return hash;
}


//hex字符串转换为hash
void  IndexEntry::hex_string_to_hash( const std::string & hexstr )
{

	if( hexstr.size() != 32)
	{
		MY_LOG_ERROR("IndexEntry::hex_string_to_hash()  size error  size=%lu", hexstr.size() );
	}
	for( int i=0, j=0; i<32; i++)
	{
		unsigned char  ch;
		if( hexstr[i] >='0' && hexstr[i] <='9')			
		{
			ch  = (hexstr[i] - '0') + 0 ;
		}
		else if( hexstr[i] >='a' && hexstr[i] <= 'f')
		{
			ch  = (hexstr[i] - 'a') + 10 ;
		}
		else
		{
			MY_LOG_ERROR("IndexEntry::hex_string_to_hash() character error");
		}

		if( i%2 ==0 )
		{
			m_contenthash[j] = ch<<4;
		}
		else
		{
			m_contenthash[j] |= ch;
			j++;
		}
	}


}


IndexFile::IndexFile( const std::string & indexfile):m_update(false)
{

	MY_LOG_DEBUG("IndexFile::IndexFile()");
	 m_indexStream.open( indexfile, std::ios::in | std::ios::out | std::ios::binary );
	if( !m_indexStream )
	{
		MY_LOG_ERROR(" IndexFile::IndexFile m_indexStream error  ");
	}
			
	m_count = 0;
	std::string line;
	std::string hash;
	while( getline( m_indexStream, line))
	{
		IndexEntry idx(line);
		if( idx.m_id > m_count )
			m_count = idx.m_id;
		m_indexMap.insert( { idx.m_id, idx} );
	}

	MY_LOG_DEBUG("PageDB::PageDB() complete!");

}

IndexFile::~IndexFile()
{
	save();
	m_indexStream.close();
}

bool IndexFile::add_index( const IndexEntry & idx)
{
	MY_LOG_DEBUG("IndexFile::add_index()");
	m_indexMap.insert( { idx.m_id , idx} );
	m_update = true;
	return true;

}

bool IndexFile::get_index( IndexEntry & idx)
{
	MY_LOG_DEBUG("IndexFile::get_index()");
	auto it = m_indexMap.find( idx.m_id);
	if( it != m_indexMap.end())
	{
		idx =  it->second;	
		return true;
	}
	return false;
}

bool IndexFile::save()
{
	MY_LOG_DEBUG("IndexFile::save()");
	if( m_update)
	{
		m_indexStream.clear();
		m_indexStream.seekp(0, std::ios::beg );
		for( const auto & v : m_indexMap)
		{
			std::string hash = v.second.hash_to_hex_string();
			m_indexStream << v.second.m_id << "\t"<<v.second.m_offset <<"\t "<< v.second.m_length<<"\t" << hash<<"\t" << v.second.m_filenum<<'\n';
		}
		m_indexStream.flush();
		m_update = false;
		MY_LOG_DEBUG("IndexFile::save() sucess ");
		return true;
	}
	MY_LOG_DEBUG("IndexFile::save() donot need to save ");
	return true;
}

size_t IndexFile::generate_id()
{
	MY_LOG_DEBUG("IndexFile::generate_id() id=%lu", m_count);
	return m_count++;
}


IndexDB::IndexDB():m_mutex()
{
	MY_LOG_DEBUG("IndexDB::IndexDB()");
	const Conf & cf = Conf::get_instance();
	m_conn = mysql_init(NULL);
	if( m_conn == NULL)
	{
		MY_LOG_ERROR("IndexDB() mysql_init() %s", mysql_error(m_conn));
	}
	unsigned int  port = (unsigned int)atoi( cf["sql_port"].c_str());
	m_db = cf["sql_db"];
	m_table = cf["sql_table"];
	if( mysql_real_connect( 
				m_conn, 
				cf["sql_host"].c_str(),  
				cf["sql_usr"].c_str(), 
				cf["sql_passwd"].c_str(), 
				m_db.c_str(),
				port, 
				NULL, 
				0 ) == NULL)
	{
		MY_LOG_ERROR("IndexDB() mysql_real_connect() %s ", mysql_error(m_conn));
	}

	std::string qstr = " select max(id) from ";
	qstr += m_table;
	if( mysql_real_query( m_conn,  qstr.c_str(), qstr.size() )  )
	{

		MY_LOG_ERROR("IndexDB() mysql_real_query() %s  :  %s ", qstr.c_str(),  mysql_error(m_conn));
	}
	
	MYSQL_RES * result = mysql_store_result( m_conn);
	if( NULL ==  result )
	{
		MY_LOG_ERROR("IndexDB() mysql_store_result() %s ", mysql_error(m_conn));
	}
	else
	{
		MYSQL_ROW  row;
		while( (row = mysql_fetch_row( result)))
		{
			if( row[0] == NULL)	
			{
				m_count =0;
			}
			else
			{
				m_count = strtoul( row[0] , NULL, 0)	;
			}
		}
	
	}

	mysql_free_result(result);

}


IndexDB::~IndexDB()
{
	mysql_close(m_conn);
}


bool IndexDB::add_index( const IndexEntry & idx)
{
	MY_LOG_DEBUG("IndexDB::add_index() idx.m_id=%lu, idx.m_offset=%lu, idx.m_length=%lu, idx.m_filenum=%d, hash=%s",
			idx.m_id, idx.m_offset, idx.m_length, idx.m_filenum, idx.hash_to_hex_string().c_str());
	MutexGuard guard(m_mutex);
	const int len = sizeof(IndexEntry) + sizeof(IndexEntry) +2;
	char cmd_buf[512];
	char escape_buf[len];
	mysql_real_escape_string(m_conn, 
							escape_buf, 
							(const char *)idx.m_contenthash, 
							sizeof(idx.m_contenthash) );
	int cmd_len = sprintf( cmd_buf, 
						"insert into %s values( %lu, %lu, %lu, '%s', %d)", 
						m_table.c_str(), 
						idx.m_id, 
						idx.m_offset, 
						idx.m_length, 
						escape_buf, 
						idx.m_filenum );
	int qret = mysql_real_query( m_conn, cmd_buf, cmd_len);
	if( qret)
	{
		MY_LOG_INFO("IndexDB::add_index() insert failed %s", mysql_error(m_conn) );
		return false;
	}
	else
	{
		return true;
	}

}

bool  IndexDB::get_index( IndexEntry & idx)
{
	MY_LOG_DEBUG("IndexDB::get_index()");
	MutexGuard guard(m_mutex);
	char cmd_buf[512];
	int cmd_len = sprintf( cmd_buf, "select * from %s where id=%lu ", m_table.c_str(), idx.m_id );
	int qret = mysql_real_query(m_conn, cmd_buf, cmd_len );

	if( qret )
	{
		MY_LOG_INFO("IndexDB::get_index() mysql_real_query() failed %s", mysql_error(m_conn) );
		return false;
	}
	else
	{
		MYSQL_RES * result = mysql_store_result(m_conn);

		MY_LOG_DEBUG("IndexDB::get_index() mysql_store_result() result = %p ", result);
		if( NULL == result)
		{
				
			MY_LOG_INFO("IndexDB::get_index() mysql_store_result() failed %s", mysql_error(m_conn) );
			mysql_free_result(result);
			return false;
		}
		else
		{
			MY_LOG_DEBUG("IndexDB::get_index()  result is not empty");
			MYSQL_ROW row;

			if( (row = mysql_fetch_row(result)))
			{
				size_t * row_len = mysql_fetch_lengths(result);
				idx.m_offset = strtoul(row[1], NULL, 0);
				idx.m_length = strtoul(row[2], NULL, 0);
				memcpy((void *)idx.m_contenthash, (const void *)row[3], row_len[3] );
				idx.m_filenum = atoi(row[4]);
			}

			mysql_free_result(result);
		}
	
	}

	MY_LOG_DEBUG("IndexDB::get_index() idx.m_id=%lu, idx.m_offset=%lu, idx.m_length=%lu, idx.m_filenum=%d, hash=%s",
			idx.m_id, idx.m_offset, idx.m_length, idx.m_filenum, idx.hash_to_hex_string().c_str());

	return true;
}


size_t IndexDB::generate_id()
{
	MY_LOG_DEBUG("IndexDB::generate_id()  id=%lu", m_count);
	return m_count++;
}



FileMgr::FileMgr( const std::string & file ) : m_mutex()
{
	MY_LOG_DEBUG("FileMgr()");
	m_lastFileNum = 0;
	m_fileStream[ m_lastFileNum ].open( file , std::ios::in | std::ios::out | std::ios::binary );
	if( !m_fileStream)
	{
		MY_LOG_ERROR("FileMgr::FileMgr() m_fileStream error");
	}


}

FileMgr::~FileMgr()
{
	for( int i=0; i <= m_lastFileNum ; i++)
	{
		m_fileStream[i].close();
	}
}

void FileMgr::save_to_file()
{
	MY_LOG_DEBUG("FileMgr()::save_to_file()");
	MutexGuard guard(m_mutex);
	for( int i=0; i <= m_lastFileNum ; i++)
	{
		m_fileStream[i].flush();
	}
}


bool FileMgr::add_page( const Page & page , IndexEntry & idx)
{

	MY_LOG_DEBUG("FileMgr::add_page() %ld", page.m_id);

	MutexGuard guard(m_mutex);

	m_fileStream[m_lastFileNum].seekp(0, std::ios::end);
	size_t off1 = m_fileStream[m_lastFileNum].tellp();	
	m_fileStream[m_lastFileNum] << page;
	size_t off2 = m_fileStream[m_lastFileNum].tellp();

	idx.m_offset = off1;
	idx.m_length = off2 - off1;
	std::string hash = page.get_hash();			
	memcpy( (void *) idx.m_contenthash, (const void*)hash.c_str(), hash.size());

	idx.m_filenum = m_lastFileNum;

	if( !m_fileStream[m_lastFileNum])
	{
		MY_LOG_ERROR("fstream error file num: %d ", m_lastFileNum);
		return false;
	}

	MY_LOG_DEBUG("FileMgr::add_page() pageid=%ld hash=%s", page.m_id, idx.hash_to_hex_string().c_str());

	return true;
}


bool FileMgr::get_page( const IndexEntry & idx, Page & page )
{

	MY_LOG_DEBUG("FileMgr::get_page() %ld", idx.m_id);
	MutexGuard guard(m_mutex);

	int num = idx.m_filenum;
	if( num <= m_lastFileNum  && num >=0 )
	{
		std::unique_ptr<char> p( new char [ idx.m_length] );
		if( !p )
		{
			MY_LOG_ERROR("unique_ptr");
		}
		m_fileStream[num].clear();
		m_fileStream[num].seekg( idx.m_offset , std::ios::beg);
		m_fileStream[num].read( p.get(), idx.m_length );
		if( m_fileStream[num])
		{
			std::string tmp( p.get(), idx.m_length );
			Page pg(tmp);
			std::string hash = pg.get_hash();
			if( 0 == memcmp( idx.m_contenthash, hash.c_str(), hash.size() ))
			{
				page = pg;
				return true;
			}

			IndexEntry idx2;
			memcpy((void *)idx.m_contenthash, (const void *)hash.c_str(), hash.size());
			std::string hashstr1 = idx.hash_to_hex_string();
			std::string hashstr2 = idx2.hash_to_hex_string();
			MY_LOG_INFO("FileMgr::get_page() %ld hash not match {hash1:%s  hash2:%s}, tmp={%s}\npage.m_content={%s}", idx.m_id, hashstr1.c_str(), hashstr2.c_str(), tmp.c_str(), page.m_content.c_str());
		}
	}
	MY_LOG_DEBUG("FileMgr::get_page() %lu failed!", idx.m_id);
	return false;
}


 



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


