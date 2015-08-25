#include"IndexEntry.h"
#include"Logger.h"
#include<sstream>

IndexEntry::IndexEntry( const std::string &str)
{
	std::istringstream iss(str);
	std::string hash;
	iss>>m_id>>m_offset>>m_length>>hash>>m_filenum;
	hex_string_to_hash(hash);
}

//hash转换为hex字符串
std::string IndexEntry::hash_to_hex_string() const
{
	std::string hash;
	for( int i = 0 ; i< 16; i++)
	{
		int h = m_contenthash[i]/16;
		int l = m_contenthash[i]%16;
		unsigned char  chigh = h >= 10 ? h-10+'a' : h-0 + '0';
		unsigned char  clow = l >= 10 ? l-10+'a' : l-0 + '0';
		hash.append(1,chigh);
		hash.append(1, clow);
	}
	return hash;
}


//hex字符串转换为hash
void  IndexEntry::hex_string_to_hash( const std::string & hexstr )
{

	if( hexstr.size() != 32)
	{
		MY_LOG_ERROR("IndexEntry::hex_string_to_hash()  size error  size=%lu", hexstr.size() );
	}
	for( int i=0, j=0; i<32; i++)
	{
		unsigned char  ch;
		if( hexstr[i] >='0' && hexstr[i] <='9')			
		{
			ch  = (hexstr[i] - '0') + 0 ;
		}
		else if( hexstr[i] >='a' && hexstr[i] <= 'f')
		{
			ch  = (hexstr[i] - 'a') + 10 ;
		}
		else
		{
			MY_LOG_ERROR("IndexEntry::hex_string_to_hash() character error");
		}

		if( i%2 ==0 )
		{
			m_contenthash[j] = ch<<4;
		}
		else
		{
			m_contenthash[j] |= ch;
			j++;
		}
	}


}




