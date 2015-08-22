#ifndef _PAGEQUE_H_
#define _PAGEQUE_H_

#include<queue>
#include<string>

#include"Page.h"

class PageQue
{
	public:
		explicit PageQue( const char * dir );
		bool empty();
		void pop( Page & page );
		void scan_dir( const char * dir );
		void make_page( Page & page );
		std::string get_title( const std::string & content );
	private:
		std::queue< std::string > m_fileQue;
};

#endif
