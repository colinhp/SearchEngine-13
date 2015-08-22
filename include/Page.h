#ifndef _PAGE_H_
#define _PAGE_H_

#include<string>
#include<ostream>

//Page格式为：
//<doc><id>ID</id><url>url</url><title>title</title><content>content</content></doc>

class Page
{
	public:
		Page() = default;
		explicit Page( std::string & data );
		friend std::ostream & operator<<( std::ostream & os, const Page & page );
		Page & operator=( const Page & ) = default;
		std::string get_hash() const;
	
		size_t m_id;
		std::string m_url;
		std::string m_title;
		std::string m_content;
};

#endif
