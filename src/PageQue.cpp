#include<iostream>
#include<fstream>
#include<sys/types.h>
#include<dirent.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<streambuf>
#include<sstream>
#include<string.h>
#include<memory>

#include"Page.h"
#include"PageQue.h"
#include"Logger.h"

PageQue::PageQue( const char * dir )
{
	scan_dir(dir);
}

bool PageQue::empty()
{
	if( m_fileQue.empty())
	{
		return true;
	}
	else
	{
		return false;	
	}
}

void PageQue::pop( Page & page )
{
	if( !m_fileQue.empty())
	{
		make_page( page );
		m_fileQue.pop();
	}
}



//遍历dir,获得该目录下文档文件的路径
void PageQue::scan_dir( const char * dir )
{
	if( NULL == dir)
		MY_LOG_ERROR("dir");
	DIR * dp = opendir( dir );
	if( NULL == dp )
		MY_LOG_ERROR("opendir");
	if(	-1 == chdir(dir))
		MY_LOG_ERROR("chdir");

	struct dirent * entry;
	struct stat buf;

	while( (entry=readdir(dp)) != NULL)	
	{
		if( -1 == stat(entry->d_name ,&buf))
			MY_LOG_ERROR("stat");

		if( S_ISDIR(buf.st_mode))
		{
			if( strcmp( entry->d_name, ".") ==0  || strcmp( entry->d_name, "..") == 0 )			
			{
				continue;	
			}
			else
			{
				scan_dir(entry->d_name);
			}
		}
		else if( S_ISREG( buf.st_mode))
		{
			char * path_buf = new char [1024*4];
			memset( path_buf,0,1024*4);
			if( NULL == path_buf )
				MY_LOG_ERROR("new");
			getcwd(path_buf, 1024*4);
			std::string file_path = path_buf;
			file_path +="/";
			file_path += entry->d_name; 
			m_fileQue.push(file_path);
			delete [] path_buf;
		}
	}

	if( -1 == chdir(".."))
		MY_LOG_ERROR("chdir");
	closedir(dp);
}

void PageQue::make_page( Page & page )
{
	std::ifstream file_ifs( m_fileQue.front() );
	if( !file_ifs )
		MY_LOG_ERROR("PageQue::make_page::ifstream");

	std::streambuf * pbuf = file_ifs.rdbuf();
	size_t size = pbuf->pubseekoff(0, std::ios::end, std::ios::in);
	pbuf->pubseekpos(0, std::ios::in);

	std::unique_ptr< char >  buf( new char [size] );
	if( buf.get() == NULL )
		MY_LOG_ERROR("new");
	pbuf->sgetn( buf.get(), size);
	std::string content( buf.get(), size );
	std::string title =	get_title( content );

	page.m_title = title;	
	page.m_content = content;
	page.m_url = m_fileQue.front();

	file_ifs.close();
}


std::string PageQue::get_title( const std::string &  content )
{
	std::istringstream is(content);	
	std::string pattern ="标  题";
	std::string title;
	std::string line;
	for( int cnt=0; getline( is, line) && cnt!= 10; cnt++)
	{
		if( line.find( pattern) != std::string::npos)
		{
			title = line ;	
			break;
		}
		if( line.size() > title.size())
		{
			title = line;	
		}
	}
	if( title[ title.size()-1] == '\r')
		title.erase( title.size()-1, 1);
	//去除标题开始的空字符
	std::string ws = "　";
	while( true )
	{
		if( title.compare( 0 , ws.size(), ws) == 0  )
		{
			title.erase(0,ws.size());
		}
		else if(	title.compare( 0 , 1 , " ") == 0 )
		{
			title.erase( 0, 1 );
		}
		else if( title.compare( 0 , 1 , "\t") == 0 )
		{
			title.erase( 0, 1 );
		}
		else if( title.compare( 0 , 1 , "\r") == 0 )
		{
			title.erase( 0, 1 );
		}
		else
		{
			break;	
		}
	}

	//标题长于90个字节，则裁剪标题
	if( title.size() > 90)
	{
		size_t pos = 0;
		if( ( pos = title.find("。")) != std::string::npos  )	
		{
			title.resize( pos );
		}
		else if( ( pos = title.find_first_of(".")) != std::string::npos )
		{
			title.resize( pos );
		}
		//如果标题长度还大于90字节，就强制剪切
		if( title.size() > 90 )
		{
			size_t i = 85;
			while( ((title[i] & 0xC0) == 0x80 )
					&& (( title[i] & 0x80) != 0))
				i++;
			title.resize(i);
			title +="...";
		}
	}
	return title;
}


