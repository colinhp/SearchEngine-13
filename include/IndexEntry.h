#ifndef _INDEXENTRY_H_ 
#define _INDEXENTRY_H_ 

#include<string>


//索引格式
//id, offset, length, contenthash , filenum

//网页库索引项
class IndexEntry
{
	public:
	IndexEntry() = default;
	explicit IndexEntry(const std::string & str);

	std::string hash_to_hex_string() const ;
	void hex_string_to_hash( const std::string & hexstr );

	size_t	m_id;
	size_t  m_offset;
	size_t  m_length;
	unsigned char   m_contenthash[16];
	int   m_filenum;

};


#endif
