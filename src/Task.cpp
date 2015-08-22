#include"Task.h"
#include"SocketIO.h"
#include"InvertIdx.h"
#include"Logger.h"
#include"PageDB.h"
#include"Page.h"
#include"json/json.h"


Task::Task( int peerfd, const std::vector<std::string> & word_vec): m_peerfd(peerfd ), m_iterm( word_vec)
{
}

void Task::execute( Cache & cache)
{


	MY_LOG_DEBUG("Task::execute()");

//查询cache
	Json::Value jqw;	
	for( const auto & v: m_iterm.m_itermVec )
	{
		jqw.append(v.first);
	}

	Json::FastWriter writer;	
	std::string qw = writer.write(jqw);

	MY_LOG_DEBUG("Task::execute() query word: %s", qw.c_str() );

	std::string res = cache.query(qw);
	SocketIO sio(m_peerfd);
//在缓存中找到查询词
	if( !res.empty())
	{
		sio.writen( res.c_str(), res.size());
		return ;
	}

//到倒排索引中检索
	InvertIdx & indx = InvertIdx::get_instance();
	std::vector<size_t> docid_vec;
	indx.query( m_iterm, docid_vec);
//匹配的文档
	if( docid_vec.size() <=0 )		
	{

		MY_LOG_DEBUG("Task::execute() cannot find result ");
		res="[{\"title\": \"cannot find query word\"}]";

	}
	else
	{
		std::vector<Page> result_vec;
		if( !get_result( docid_vec, result_vec))	
		{

			res="[{\"title\": \"cannot find query word\"}]";
		}	
		else
		{

			MY_LOG_DEBUG("Task::execute():match %d doc, get %d result ",  docid_vec.size(), result_vec.size() );

			int i=0;
			Json::Value jresult;			
			//将结果转换为json格式
			for( const auto & v : result_vec )	
			{
				Json::Value tmp;
				tmp["title"]	 = v.m_title;
				tmp["url"] = v.m_url;
				tmp["content"] = v.m_content;
				jresult[i] = tmp;
				i++;
			}
			res= writer.write(jresult);
		}
	}

	cache.add_cache(qw, res);
	sio.writen( res.c_str(), res.size());
	MY_LOG_DEBUG("Task::execute() result:%s", res.c_str());
	return ;
}


bool Task::get_result( std::vector<size_t> &docid_vec, std::vector<Page> & result_vec)
{

	MY_LOG_DEBUG("get_result()");
	for( size_t i =0; i < docid_vec.size() ; i++)	
	{
		Page page;	
		if( get_excerpt( docid_vec[i], page))
		{
			result_vec.push_back(page);
		}
	}
	if( result_vec.size() >0 )
	{
		return true;	
	}
	else
	{
		MY_LOG_INFO("Task::get_result() cannot find result! ");
		return false;	
	}
}

bool Task::get_excerpt(size_t doc_id, Page & page )
{
	PageDB & pagedb = PageDB::get_instance();
	
	MY_LOG_DEBUG("get excerpt from doc %ld ", doc_id);

//没有在网页库中找网页
	if(	!pagedb.get_page( doc_id, page ))
	{
		MY_LOG_INFO(" Task::get_except() failed ! cannot get page ! ");
		return false;	
	}

//根据TF最高的五个关键词提取摘要
	std::string excerpt;
	std::istringstream iss( page.m_content);
	std::string line;
	for( int cnt=0; iss>>line && cnt < 3 ; )
	{
		for( size_t idx =0; idx < m_iterm.m_itermVec.size() && idx < 5; idx++ )
		{
//查找关键词
			size_t i=line.find( m_iterm.m_itermVec[idx].first) ;

			if( i != std::string::npos )	
			{

//修剪摘要的长度
				size_t j = 0;
				size_t k = line.size() ;

				if( i > 45  )				
				{
					j = i-45;
					while( j>0 && (( line[j] & 0xC0) == 0x80) )
					{
						j--;
					}
				}

				if( i+45 < line.size() )
				{
					k = i+45;
					while( k < line.size() && (( line[k] & 0xC0) == 0x80 ))
					{
						k++;	
					}
				}
			
				excerpt += std::string( line, j, k-j);
				excerpt +="...";
				cnt++;
				break;
			}
		}
	}


	page.m_content = excerpt;

	if( excerpt.size() > 0)
	{
		return true;
	}
	else
	{
		MY_LOG_INFO("Task::get_excerpt() failed!  excerpt is empty  ");

		return false;	
	}
}


