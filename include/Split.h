#ifndef _SPLIT_H_
#define _SPLIT_H_


#include"NLPIR.h"
#include"Mutex.h"
#include<vector>
#include<set>
#include<string>


//单例模式，分词类
class Split
{
	public:
	void split_filter( const std::string & text, std::vector< std::string > & word_vec ) const ;
	static const Split & get_instance();
	static void init_instance();
	static void shut_down();

	private:
	explicit Split( const std::string & init_dir , const std::string & stop_file1, const std::string & stop_file2);
	~Split();
	static Split * m_instance;
	static Mutex m_mutex;
	std::set< std::string>  m_stopSet1;
	std::set< std::string>  m_stopSet2;
	class Garbage	
	{
		public:	
			~Garbage()
			{
		
				if( m_instance != NULL )	
				{
					delete m_instance;					
					m_instance = NULL;
				}
				
			}
	};
	static Garbage gc;

};

#endif
