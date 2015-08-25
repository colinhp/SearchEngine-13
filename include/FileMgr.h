#ifndef _FILEMGR_H_
#define _FILEMGR_H_

#include"IndexEntry.h"
#include"Page.h"
#include"Mutex.h"
#include<fstream>


//网页库文件管理器
#define  MAX_FILE_NUM 5
class FileMgr
{
	public:
		explicit FileMgr( const std::string & file );
		~FileMgr();
		FileMgr( const FileMgr & )  = delete;
		FileMgr & operator=( const FileMgr & ) = delete ;
		bool add_page( const Page & page, IndexEntry & idx);
		bool get_page( const IndexEntry & idx, Page & page );
		void save_to_file();

	private:
		std::fstream  m_fileStream[ MAX_FILE_NUM ];
		int  m_lastFileNum;
		Mutex m_mutex;
	

};






#endif
