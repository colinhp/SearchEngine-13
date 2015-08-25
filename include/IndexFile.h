#ifndef _INDEXFILE_H_
#define _INDEXFILE_H_

#include"IndexEntry.h"
#include"Indexer.h"
#include<string>
#include<map>
#include<fstream>

//索引存储到文件中, 运行时加载到内存中
class IndexFile : public Indexer
{
	public:
	 explicit IndexFile( const std::string & indexfile );
	 ~IndexFile();
	 bool add_index( const IndexEntry & idx );
	 bool get_index( IndexEntry & idx);
	 bool save();
	 size_t generate_id();
	private:
	 std::fstream  m_indexStream ;
	 std::map< size_t , IndexEntry > m_indexMap;
	 size_t m_count;
	 bool  m_update;

};



#endif
