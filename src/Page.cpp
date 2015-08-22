#include"Page.h"
#include"Logger.h"
#include<openssl/md5.h>
#include<string.h>
#include<string>


//
//<doc>
//    <id> id </id>
//    <url> url </url>
//    <title> title </title>
//    <content> content </content>
//</doc>
//

Page::Page( std::string &data )
{
	
	size_t beg = 0;
	size_t end = 0;

	std::string id_beg ="<doc><id>";
	std::string id_end ="</id>";
	beg = data.find( id_beg );
    end = data.find( id_end );
	if( beg == std::string::npos || end == std::string::npos)
		MY_LOG_ERROR("page id invalid");
	std::string id_str = data.substr( beg+id_beg.size(),  end-beg-id_beg.size());
	m_id = std::stoul( id_str) ;

	std::string url_beg ="<url>" ;
	std::string url_end ="</url>";
	beg = data.find( url_beg );
	end = data.find( url_end );
	if( beg == std::string::npos || end == std::string::npos)
		MY_LOG_ERROR("page url invalid");
	m_url = data.substr( beg+url_beg.size(),  end-beg-url_beg.size());

	std::string title_beg ="<title>" ;
	std::string title_end ="</title>";
	beg = data.find( title_beg );
	end = data.find( title_end );
	if( beg == std::string::npos || end == std::string::npos)
		MY_LOG_ERROR("page title invalid");
	m_title = data.substr( beg+title_beg.size(),  end-beg-title_beg.size());

	std::string content_beg ="<content>" ;
	std::string content_end ="</content></doc>";
	beg = data.find( content_beg );
	end = data.find( content_end );
	if( beg == std::string::npos || end == std::string::npos)
		MY_LOG_ERROR("page content invalid");
	m_content = data.substr( beg+content_beg.size(),  end-beg-content_beg.size());

}

std::ostream & operator<<( std::ostream & os, const Page & page )
{
	os<<"<doc><id>";
	os<<page.m_id;
	os<<"</id><url>";
	os<<page.m_url;
	os<<"</url><title>";
	os<<page.m_title;		
	os<<"</title><content>";
	os<<page.m_content;
	os<<"</content></doc>";

	return os;
}

std::string Page::get_hash() const
{

	unsigned char md[ MD5_DIGEST_LENGTH];
	memset(md, 0, MD5_DIGEST_LENGTH);
	MD5( reinterpret_cast< const unsigned char *>( m_content.c_str()), m_content.size(), md);
	std::string md_str( md, md + MD5_DIGEST_LENGTH);
	return md_str;

}




