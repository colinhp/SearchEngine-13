#ifndef _INVERTIDX_H_
#define _INVERTIDX_H_

#include<string>
#include<unordered_map>
#include<fstream>
#include<map>
#include"Iterm.h"
#include"Mutex.h"

//倒排索引文件存储格式为:
//word docid TF docid TF
//内存中的倒排索引格式为:
//word docid WTF-IDF docid WTF-IDF 

//IDF = log( m_count /(DF+1)  )
//m_count 文档总数
//DF Iterm在文档中出现的次数
//TF-IDF = TF*IDF  
//WTF-IDF = TF-IDF/sqrt( sum( TF-IDF*TF-IDF ))  

//单例模式
class InvertIdx
{
	public:
		explicit InvertIdx( const std::string & file ,int max_res);
		~InvertIdx();
		void add_iterm( size_t docid, const Iterm &  iterm );
		void build_invert_index();
		bool query( Iterm & iterm , std::vector< size_t > & docid_vec );
		void calculate_weight( Iterm & iterm);
		double calculate_similarity( const Iterm & iterm, size_t docid );
		void save_to_file();
		static InvertIdx & get_instance();
		static void init_instance();
		static void shut_down();
	private:
		//文档总数
		size_t  m_count;
		bool    m_update;
		int   m_maxNumResult;
		std::map< std::string , std::vector< std::pair<size_t,double> > >   m_ndxMap;
		static Mutex m_mutex;
		static InvertIdx * m_instance;

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
