#ifndef _INDEXER_H_
#define _INDEXER_H_

#include"IndexEntry.h"

//索引管理器
class Indexer
{
	public:
	virtual bool add_index( const IndexEntry & idx ) =0;
	virtual bool get_index( IndexEntry & idx )=0 ;
	virtual bool save()=0;
	virtual size_t generate_id()=0;
	virtual ~Indexer(){};

};




#endif
