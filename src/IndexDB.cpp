#include"IndexDB.h"
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



