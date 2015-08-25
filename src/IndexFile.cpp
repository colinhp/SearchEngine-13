#include"Indexer.h"
#include"IndexFile.h"
#include"Logger.h"




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




