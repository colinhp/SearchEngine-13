#include"Building.h"
#include"Conf.h"
#include"PageDB.h"
#include"Split.h"
#include"PageQue.h"
#include"InvertIdx.h"
#include"PageDedup.h"
#include"Logger.h"
#include<iostream>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<signal.h>
#include<string.h>
#include<errno.h>

/*

class TF
{
	public:
		void add( std::vector<std::string> wordvec )
		{
			for( const auto & v : wordvec )	
			{
				tfmap[v]++;
			}
		}

	~TF()	
	{
		
		const Conf & conf = Conf::get_instance();
		std::ofstream ofs( conf["tffile"], std::ios::out | std::ios::binary );
		if( !ofs )
		{
			MY_LOG_ERROR("TF file error ");
		}
		for( const auto & v : tfmap)	
		{
			ofs << v.first <<"    "	<<v.second <<"\n";
		}
		ofs.close();
	
	}
	std::map< std::string ,size_t > tfmap;
};

*/



void my_daemon(void)
{
	
	int pid = fork();
	if( pid <0)
	{
		MY_LOG_ERROR("fork: %s", strerror(errno));
		exit(0);
	}
	else if( pid > 0 )
	{
		exit(0);
	}

	struct sigaction act;
	act.sa_flags = 0;	
	act.sa_handler = SIG_IGN;	
	sigemptyset( &act.sa_mask);
	sigaction( SIGHUP, &act, NULL);

	pid = setsid();
	if( pid == -1)
	{
		MY_LOG_ERROR("setsid");
		exit(0);
	}
	
	pid = fork();
	if( fork < 0)
	{
		MY_LOG_ERROR("fork: %s", strerror(errno));
		exit(0);
	}
	else if( pid > 0)
	{
		exit(0)	;
	}

	struct rlimit rlmt;
	getrlimit(RLIMIT_NOFILE , &rlmt);
	if( rlmt.rlim_max == RLIM_INFINITY )
	{
		rlmt.rlim_max = 1024;	
	}
	for( size_t i=0; i< rlmt.rlim_max; i++)
	{
		close(i);
	}

	int fdnull = open("/dev/null", O_RDWR );
	int fdin = dup( fdnull);
	int fdout = dup( fdnull);
	if( fdnull !=0 || fdin != 1 || fdout != 2)	
	{
		MY_LOG_ERROR(" fdnull=%d, fdin=%d, fdout=%d  error:%s", fdnull,fdin, fdout, strerror(errno));
		exit(0);
	}
	chdir("/");
	umask(0);
}












bool initiation( )
{
	const Conf & conf = Conf::get_instance();
	const Split & split = Split::get_instance();
	PageDB & pagedb = PageDB::get_instance();
	InvertIdx & invndx = InvertIdx::get_instance();

	MY_LOG_DEBUG("initiation() ");

	PageQue pageque( conf["rawpage"].c_str()) ;
	PageDedup  dedup( conf["pagehash"]);
	//TF tf;
	while( !pageque.empty())	
	{
		Page page;
		pageque.pop(page);

		std::vector< std::string > wordVec;
		split.split_filter( page.m_content ,wordVec );
		Iterm iterm( wordVec );

		std::string hash = iterm.get_hash();		
		if( !dedup.is_duplicate( hash ) )
		{
//			tf.add( wordVec );


			dedup.add( hash );
			page.m_id = pagedb.generate_id();
			pagedb.add_page( page );
			invndx.add_iterm( page.m_id, iterm );

			MY_LOG_DEBUG("add page %ld ", page.m_id);

		}
		else
		{
			MY_LOG_INFO("page duplicate! ");

			continue;	
		}
	}

	pagedb.save_to_file();
	invndx.save_to_file();
	invndx.build_invert_index();

	MY_LOG_DEBUG("initiation complete!");

	return true;
}



