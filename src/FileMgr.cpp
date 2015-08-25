#include"FileMgr.h"
#include"MutexGuard.h"
#include"Logger.h"
#include"IndexEntry.h"
#include"Indexer.h"
#include<string.h>
#include<stdio.h>
#include<memory>
#include<cstdlib>
#include<functional>
#include<ios>
#include<sstream>
#include<fstream>



FileMgr::FileMgr( const std::string & file ) : m_mutex()
{
	MY_LOG_DEBUG("FileMgr() %s", file.c_str());
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


 


