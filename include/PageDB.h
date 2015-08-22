#ifndef _PAGEDB_H_
#define _PAGEDB_H_

#include"Page.h"
#include"Mutex.h"

#include<string>
#include<vector>
#include<unordered_map>
#include<utility>
#include<memory>
#include<map>
#include<fstream>
#include<functional>
#include<my_global.h>
#include<mysql.h>



/* 
//索引文件格式
//id, offset, length


class PageDB
{
public:

	bool add_page( const Page & page);
	bool get_page( size_t id, Page & page );
	size_t generate_id();
	void save_to_file();


	PageDB() = delete;
	PageDB( const PageDB &  ) = delete;
	PageDB & operator=( const PageDB & ) = delete;

	static PageDB & get_instance();
	static void init_instance();
	static void shut_down();


private:

	explicit PageDB( const std::string & data_file, const std::string & index_file );
	~PageDB();

	static PageDB * m_instance; 
	static Mutex m_mutex;
	size_t m_count;
	bool  m_update;
	std::fstream  m_dataStream;
	std::fstream  m_indexStream;
	std::unordered_map< size_t , std::pair< size_t, size_t> > m_indexMap;
	
	class Garbage
	{
		public:
			~Garbage()	
			{
				if( m_instance != NULL )	
				{
					delete m_instance;	
					m_instance =NULL;
				}
			}
	};
	static Garbage gc;
	

};

*/


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


//索引管理器
class Indexer
{
	public:
	virtual bool add_index( const IndexEntry & idx ) =0;
	virtual bool get_index( IndexEntry & idx )=0 ;
	virtual bool save()=0;
	virtual size_t generate_id()=0;
	virtual ~Indexer(){};

};

//索引存储到文件中, 运行时加载到内存中
class IndexFile : public Indexer
{
	public:
	 explicit IndexFile( const std::string & indexfile );
	 ~IndexFile();
	 bool add_index( const IndexEntry & idx );
	 bool get_index( IndexEntry & idx);
	 bool save();
	 size_t generate_id();
	private:
	 std::fstream  m_indexStream ;
	 std::map< size_t , IndexEntry > m_indexMap;
	 size_t m_count;
	 bool  m_update;

};


//索引存储到数据库中
class IndexDB : public Indexer
{
	public:
	 IndexDB();
	 ~IndexDB();
	 bool add_index( const IndexEntry & idx );
	 bool get_index( IndexEntry & idx);
	 bool save(){ return true;}
	 size_t generate_id();

	private:
	MYSQL * m_conn;
	std::string m_db;
	std::string m_table;
	size_t m_count;
	Mutex m_mutex;

};


//网页库文件管理器
#define  MAX_FILE_NUM 5
class FileMgr
{
	public:
		explicit FileMgr( const std::string & file );
		~FileMgr();
		FileMgr( const FileMgr & )  = delete;
		FileMgr & operator=( const FileMgr & ) = delete ;
		bool add_page( const Page & page, IndexEntry & idx);
		bool get_page( const IndexEntry & idx, Page & page );
		void save_to_file();

	private:
		std::fstream  m_fileStream[ MAX_FILE_NUM ];
		int  m_lastFileNum;
		Mutex m_mutex;
	

};



//单例模式,网页库数据类
class PageDB
{
public:

	bool add_page( const Page & page);
	bool get_page( size_t id, Page & page );
	size_t generate_id();
	void save_to_file();


	PageDB() = delete;
	PageDB( const PageDB &  ) = delete;
	PageDB & operator=( const PageDB & ) = delete;

	static PageDB & get_instance();
	static void init_instance();
	static void shut_down();


private:

	explicit PageDB(  const std::string & datafile , const std::string & indexfile );
	~PageDB();

	static PageDB * m_instance; 
	static Mutex m_mutex;
	bool  m_update;

	FileMgr  m_fileMgr;
	std::shared_ptr< Indexer >   m_indexer;
	
	class Garbage
	{
		public:
			~Garbage()	
			{
				if( m_instance != NULL )	
				{
					delete m_instance;	
					m_instance =NULL;
				}
			}
	};
	static Garbage gc;
	

};


#endif
