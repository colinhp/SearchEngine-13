#ifndef _ITERM_H_
#define _ITERM_H_

#include<vector>
#include<string>


class Iterm
{
	public:
		explicit Iterm() = default;
		explicit Iterm( const std::vector<std::string> & word_vec );
		std::string get_hash();
		void calculate_tf( double max_tf );
		std::vector< std::pair< std::string, double > > m_itermVec;	
};



#endif
