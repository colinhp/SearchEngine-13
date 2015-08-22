#include<Split.h>
#include<NLPIR.h>
#include<fstream>
#include<sstream>
#include"MutexGuard.h"
#include"Logger.h"
#include"Conf.h"

Split * Split::m_instance = NULL;
Mutex Split::m_mutex;
Split::Garbage Split::gc;


void Split::init_instance()
{
	MutexGuard guard(m_mutex);

	MY_LOG_DEBUG("Split::init()");

	if( m_instance == NULL)
	{
		Conf cf = Conf::get_instance();
		m_instance = new Split( cf["NLPIR"], cf["stoplist1"], cf["stoplist2"])	;
	}

}

void Split::shut_down()
{
	
	MY_LOG_DEBUG("Split::shut_down()");
	if( m_instance != NULL )
	{
		delete m_instance;	
		m_instance = NULL;	
	}
}

const Split & Split::get_instance()
{
	if( m_instance == NULL)
	{
		init_instance()	;
	}
	return *m_instance;
}

Split::Split( const std::string  & init_dir , const std::string &stop_file1, const std::string & stop_file2 )
{

	if( init_dir.empty() || stop_file1.empty() || stop_file2.empty())
		MY_LOG_ERROR( "Split::Split(): dir or file is empty");
	if( !NLPIR_Init( init_dir.c_str(), UTF8_CODE)  )
	{
		MY_LOG_ERROR( "NLPIR_init(): fail! init_dir=%s", init_dir.c_str());
	}	

	std::ifstream ifs1( stop_file1 );
	if( !ifs1)
		MY_LOG_ERROR("Split::Split(): stop_file1=%s", stop_file1.c_str());
	
	std::string word1;
	while( ifs1>> word1 )
	{
		m_stopSet1.insert(word1);
	}

	ifs1.close();

	std::ifstream ifs2( stop_file2 );
	if( !ifs2)
		MY_LOG_ERROR("Split::Split(): stop_file2=%s", stop_file2.c_str());
	
	std::string word2;
	while( ifs2>> word2 )
	{
		m_stopSet2.insert(word2);
	}

	ifs2.close()	;

}


	Split::~Split()
{

	NLPIR_Exit();

}


void Split::split_filter( const  std::string & text, std::vector< std::string >& word_vec) const
{

	if( text.empty())	
	{
		MY_LOG_INFO("Split::split_filter(): empty text");
		return ;
	}

	std::string str = NLPIR_ParagraphProcess( text.c_str(), 0);

//替换汉字空格字符为ASCII空格字符	

//	MY_LOG_DEBUG("Split::split_filter():\n%s\n", str.c_str());

	std::string ws="　";
	size_t len= ws.size();
	for( size_t pos=0; pos != std::string::npos; pos += len )	
	{
		if( (pos = str.find(ws, pos)) != std::string::npos)	
		{
			str.replace(pos, len, len,' ' );
		}
		else
		{
			break;	
		}
	}

	std::istringstream is(str);
	std::string word;
	
	while( is >> word )
	{
		size_t i=0; 
		for( i=1; i<word.size(); i++)
		{
			if((word[i] & 0xC0) != 0x80 )	
				break;
		}	
		std::string first_word = word.substr(0, i);
//		MY_LOG_INFO("first_word = %s", first_word.c_str());
		if( (m_stopSet1.find( word ) != m_stopSet1.end()) 
			|| (m_stopSet2.find( word ) != m_stopSet2.end()))	
		{
			continue;
		}
		if( (m_stopSet1.find( first_word ) != m_stopSet1.end())) 
		{
				continue;
		}
		word_vec.push_back(word);
	}

}

