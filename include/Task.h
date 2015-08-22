#ifndef _TASK_H_
#define _TASK_H_

#include"Iterm.h"
#include"PageDB.h"
#include"InvertIdx.h"
#include"Page.h"
#include"Cache.h"
#include<string>

class Task
{
	public:
		explicit Task() = default;
		explicit Task( int peerfd , const  std::vector< std::string > & word_vec );
		void execute( Cache & cache );
		bool get_result( std::vector<size_t> &docid_vec, std::vector<Page> &result_vec);
		bool get_excerpt( size_t doc_id, Page & page);

	private:
		int m_peerfd;
		Iterm m_iterm;
};


#endif
