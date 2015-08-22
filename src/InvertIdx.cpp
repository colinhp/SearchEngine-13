#include"InvertIdx.h"
#include"Iterm.h"
#include"MutexGuard.h"
#include"Logger.h"
#include<cstdlib>
#include<utility>
#include<unordered_map>
#include<queue>
#include<cmath>
#include"Conf.h"
#include<fstream>
#include<string>
#include<sstream>
#include<utility>

Mutex InvertIdx::m_mutex;
InvertIdx * InvertIdx::m_instance = NULL;
InvertIdx::Garbage InvertIdx::gc;


InvertIdx::InvertIdx( const std::string & file, int max_res ):m_count(0),m_update(false), m_maxNumResult(max_res)
{
	MY_LOG_DEBUG("InvertIdx::InvertIdx()");
	std::ifstream ifs( file , std::ios::binary | std::ios::in);
	std::string line;	
	std::string word;
	while( getline(ifs, line))
	{
		std::istringstream iss(line);
		iss >> word;
		size_t id;
		double fre;
		while( iss >> id >> fre)
		{
			m_ndxMap[word].push_back( std::make_pair(id,fre));
		}
		m_count++;
	}
	ifs.close();
	MY_LOG_DEBUG("InvertIdx::InvertIdx() complete!");
}

InvertIdx::~InvertIdx()
{
//	save_to_file();
}

void InvertIdx::save_to_file()
{
	MY_LOG_DEBUG("InvertIdx::save_to_file()");

	if( m_update )
	{
		const Conf & cf = Conf::get_instance()	;
		std::ofstream ofs( cf["invertidx"], std::ios::binary | std::ios::out);
		if( !ofs )
		{
			MY_LOG_ERROR("invertidx::ofstream error");
		}

		for( const auto & kvp : m_ndxMap )
		{
			ofs << kvp.first<<" ";	

			for( const auto & v : kvp.second )	
			{
				ofs << v.first <<" "<<v.second<<" " ;
			}

			ofs<<'\n';
		}

		ofs.close();
		m_update = false;
	}

	MY_LOG_DEBUG("InvertIdx::save_to_file() complete!");
}

void InvertIdx::init_instance()
{
	MY_LOG_DEBUG("InvertIdx::init()");
	MutexGuard guard(m_mutex);
	if( m_instance == NULL)
	{
		const Conf & cf = Conf::get_instance()	;
		int  max = atoi(cf["max_num_result"].c_str());
		m_instance = new InvertIdx( cf["invertidx"], max );
	}

	MY_LOG_DEBUG("InvertIdx::init() complete!");
}

void InvertIdx::shut_down()
{
	MY_LOG_DEBUG("InvertIdx::shut_down()");
	if( m_instance != NULL)
	{

		delete m_instance;	
		m_instance = NULL;
	}
}

InvertIdx & InvertIdx::get_instance()
{
	if( m_instance == NULL)
	{
		init_instance();
	}
	return *m_instance;
}


void InvertIdx::add_iterm( size_t docid, const Iterm & iterm )
{

	MY_LOG_DEBUG("InvertIdx::add_iterm()");
	for(  auto & v : iterm.m_itermVec )
	{
//		MY_LOG_DEBUG("InvertIdx::add_iterm(): doc %ld, TF %lf", docid, v.second);
		std::pair<size_t, double> val(docid, v.second);
		m_ndxMap[ v.first ].push_back( std::move(val) );
	}
	m_count++;
	m_update = true;
}

//根据TF-IDF建立倒排索引表
//归一化处理
void InvertIdx::build_invert_index()
{
	
	MY_LOG_DEBUG("InvertIdx::build_invert_index()");

	std::unordered_map< size_t, double> sumMap;
	for( auto & kvp : m_ndxMap )
	{
		size_t df = kvp.second.size()	;
		double idf = log( m_count/(df+1) )/log(2);

		for( auto & v : kvp.second  )
		{
			v.second *= idf;
			sumMap[v.first]	+= v.second * v.second ;
		}
	}
//归一化处理
	for( auto & kvp : m_ndxMap )
	{
		for( auto & v : kvp.second )
		{
			v.second = v.second / sqrt( sumMap[v.first] );
		}
	}

	MY_LOG_DEBUG("InvertIdx::build_invert_index() complete!");
}
//检索iterm中的查询词，返回的匹配文档id, 并且文档id按相似度从高到低排序
bool InvertIdx::query( Iterm & iterm, std::vector<size_t> & docid_vec )
{

	MY_LOG_DEBUG("InvertIdx::query()");

	std::unordered_map< size_t, size_t> andMap;

	for( const auto &  kvp : iterm.m_itermVec )
	{
		for( const auto & v : m_ndxMap[kvp.first] )	
		{
			andMap[v.first]++;
		}
	}

	std::vector< size_t> id_vec;		
	for( const auto & v : andMap  )
	{
		if( v.second == iterm.m_itermVec.size() )		
			id_vec.push_back( v.first );
	}
		
	if( id_vec.size() > 0 )
	{

		MY_LOG_DEBUG("InvertIdx::query(): match %d doc", id_vec.size() );

//定义优先级队列，从大到小输出
		auto comp = []( const std::pair<size_t, double> &lhs, const std::pair<size_t, double> &rhs )->bool
		{
			return lhs.second < rhs.second;
		};
		std::priority_queue< std::pair<size_t,double>, 
							std::vector<std::pair< size_t, double> >,
							decltype( comp ) > sim_pq(comp);
		calculate_weight( iterm );
//计算相似度，并把结果放入优先级队列
		for( const auto & v : id_vec )
		{
			double sim = calculate_similarity( iterm, v );
			sim_pq.push( std::make_pair(v, sim));
		}
//取出相似度最高的前m_maxNumResult个结果
		for( int i=0; i<m_maxNumResult && !sim_pq.empty(); i++ )
		{
			size_t doc = sim_pq.top().first;
			docid_vec.push_back( doc );
			sim_pq.pop();
		}

		return true;	
	}
	else
	{
		MY_LOG_INFO("InvertIdx::query(): not match");
		return false;
	}
}

//计算iterm中各个词语的TF-IDF权重
void InvertIdx::calculate_weight( Iterm & iterm )
{
	double sum = 0;
	for( auto & v : iterm.m_itermVec )
	{
		size_t df = m_ndxMap[v.first].size()	;
		double idf = log( m_count/(df+1) )/log(2);
		v.second = v.second * idf;	
		sum += v.second * v.second;
	}
//归一化处理
	double m = sqrt( sum );
	for( auto & v : iterm.m_itermVec )
	{
		v.second /= m;
	}

}

//计算检索词语和文档的余弦相似度
double InvertIdx::calculate_similarity( const Iterm & iterm, size_t docid )
{
	double sim;
	for( const auto & kvp : iterm.m_itermVec )
	{
		double w1=0;
		for( const auto & v : m_ndxMap[kvp.first] )
		{
			if( v.first == docid )	
			{
				w1 = v.second;
				break;	
			}
		}	
		sim += kvp.second * w1 ;	
	}
	MY_LOG_DEBUG("InvertIdx::calculate_similarity(): %lf", sim );
	return sim;
}





