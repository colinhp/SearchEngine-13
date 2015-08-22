#include"Iterm.h"
#include"Logger.h"

#include<sstream>
#include<map>
#include<algorithm>
#include<openssl/md5.h>
#include<string.h>
#include<utility>
#include<vector>

//Wtf= a + (1-a)*(Tf)/Max(Tf)
//a=0.4
//Tf为单词频数

Iterm::Iterm( const std::vector<std::string> & word_vec )
{
	std::map< std::string, double > tmpMap;
	for( const auto & v : word_vec )
	{
		tmpMap[ v ]++;
	}
	
	double max_tf=0;
	for( auto & v : tmpMap )
	{
		if( v.second > max_tf )
			max_tf = v.second;
		m_itermVec.push_back(v);
	}

	calculate_tf( max_tf);

}


//对TF最高的前10个词进行MD5 Hash
//128bit md5
std::string Iterm::get_hash()
{
	std::string str; 
	auto it = m_itermVec.begin();
	for(  int i=0; i <10 && it != m_itermVec.end(); i++ , it++ )
	{
		str += it->first;	
		str +=" ";
	}
	unsigned char md[ MD5_DIGEST_LENGTH];
	memset(md, 0, MD5_DIGEST_LENGTH );
	MD5( reinterpret_cast< const unsigned char *>( str.c_str()), str.size(), md );
	std::string md_str( md, md+MD5_DIGEST_LENGTH );

	return md_str;
}

//计算TF，并按从大到小的顺序排序
void Iterm::calculate_tf( double max_tf )
{

	MY_LOG_DEBUG("Iterm::calculate_tf(): max TF = %lf ", max_tf);
	for( auto & v : m_itermVec )
	{
		v.second = 	0.4 + 0.6*v.second/max_tf;
//		MY_LOG_DEBUG("Iterm::calculate_tf(): TF %s = %lf ", v.first.c_str(), v.second);
	}

	std::sort( m_itermVec.begin(), m_itermVec.end(), 
			[]( const std::pair< std::string , double > &lhs,
				const std::pair< std::string , double > &rhs 
			  ) -> bool
	{
	return lhs.second > rhs.second;	
	}	
	);
}


