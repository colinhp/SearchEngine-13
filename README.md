#SearchEngine
本项目实现了一个基于向量空间模型的轻量级网页文本搜索引擎，检索的核心是用TF-IDF计算文本的特征权重，根据Cosine相似性原理比较查询语句和文档的相似性。
#特点：
字符转码，信息指纹，网页去重，TF-IDF，余弦相似度，MySQL, Redis，单例模式，简单工厂模式，Epoll，线程池，TCP, STL

#开发环境：
CentOS Linux release 7.0.1406 (x64)
g++ (GCC) 4.8.3 20140911 (Red Hat 4.8.3-9)

#动态库需求
NLPIR
JsonCpp
Log4cpp
Redis
MySQL


#文件结构

```
./
├── bin 				执行文件目录
│   └── main
├── data  				
│   ├── config 			程序配置文件
│   ├── Data  			NLPIR分词系统配置目录
│   ├── invertidx 		倒排索引文件
│   ├── logcfg 			log4cpp配置文件
│   ├── pagedb 			网页库文件
│   ├── pagedbidx 		网页库文件的索引文件
│   ├── pagehash 		网页信息指纹库文件
│   ├── stoplist1.txt 	停用词文件1
│   ├── stoplist2.txt 	停用词文件2
│   └── text 			粗糙网页目录
├── include
│   ├── Building.h 		包含几个初始化函数
│   ├── Cache.h 		Cache类，用Redis实现缓存，key为查询词的JSON序列化字符串，value为搜索结果的JSON序列化字符串
│   ├── Condition.h 	条件变量类，封装了Linux条件变量
│   ├── Conf.h   		程序配置类，单例模式，用于全局配置
│   ├── EpollPoller.h 	Epoll类，非阻塞ET模式，处理Socket并发
│   ├── FileMgr.h 		网页库文件管理器，负责网页库文件的增加，删除，查找功能
│   ├── hiredis.h 		hiredis头文件		
│   ├── IndexDB.h  		索引数据库类，用mysql保存网页库的索引 
│   ├── IndexEntry.h 	索引项类，定义索引的格式
│   ├── Indexer.h 		索引管理器类，抽象类，作接口用。
│   ├── IndexFile.h 	索引文件类，用文件保存网页库的索引
│   ├── InetAddress.h   Socket地址类
│   ├── InvertIdx.h 	倒排索引表类，TF-IDF的计算，相似度的计算
│   ├── Iterm.h 		特征类，计算特征的TF
│   ├── json 			JsonCpp头文件目录
│   ├── Logger.h  		日志类，封装了LOG4CPP，作为程序的日志系统
│   ├── MutexGuard.h  	封装临界区的进入和退出
│   ├── Mutex.h 		用RAII方法封装了linux mutex
│   ├── NLPIR.h   		NLPIR的头文件
│   ├── PageDB.h 		网页数据类，管理网页库文件类和索引管理器类，用简单工厂模式确定索引的存储方式
│   ├── PageDedup.h 	网页去重类，采用信息指纹算法
│   ├── Page.h 			网页类，定义网页的数据格式
│   ├── PageQue.h 		网页队列类
│   ├── Socket.h 		封装socket的基本设置
│   ├── SocketIO.h 		封装socket的I/O操作
│   ├── Split.h 		分词器类，封装NLPIR作为分词器
│   ├── Task.h 			任务类，执行查询词的检索任务
│   ├── Thread.h 		线程类，funcitonal+bind
│   └── ThreadPool.h 	线程池类
├── lib 			动态库目录
│   ├── libhiredis.so
│   ├── libjsoncpp.so
│   ├── liblog4cpp.so
│   └── libnlpir.so
├── log 			日志目录
├── Makefile
├── obj
├── src 			源文件目录
│   ├── Building.cpp
│   ├── Cache.cpp
│   ├── Condition.cpp
│   ├── Conf.cpp
│   ├── EpollPoller.cpp
│   ├── FileMgr.cpp
│   ├── IndexDB.cpp
│   ├── IndexEntry.cpp
│   ├── IndexFile.cpp
│   ├── InetAddress.cpp
│   ├── InvertIdx.cpp
│   ├── Iterm.cpp
│   ├── Logger.cpp
│   ├── main.cpp
│   ├── Mutex.cpp
│   ├── MutexGuard.cpp
│   ├── Page.cpp
│   ├── PageDB.cpp
│   ├── PageDedup.cpp
│   ├── PageQue.cpp
│   ├── Socket.cpp
│   ├── SocketIO.cpp
│   ├── Split.cpp
│   ├── Task.cpp
│   ├── Thread.cpp
│   ├── ThreadPool.cpp
│   └── tool
├── test
│   ├── cli.cpp 	测试程序源文件
│   ├── client.o
│   ├── json
│   ├── libjsoncpp.so
│   ├── Makefile
│   ├── SocketIO.cpp
│   └── SocketIO.h
└── update

```

#程序流程
```
准备阶段：
Begin ——> Load Configuration ——> Daemon Process
——> Raw Page Queue ——> Deduplicate ——> Page Database 
——> Build Invert Index Table 

启动阶段：
1. 
ThreadPool ——> Start Threads ——> Start Thread Cache ——> Threads Waiting
2.
Socket ——> EpollPool Looping

连接阶段：
 Client ——> Query Words ——> Epoll ——> Read Socket Message
 Split Words ——> Iterm ——> Add Task ——> Thread Gets Task

检索阶段：
Cache Matched？——YES——>return cache
     |
     NO
     |
Found in Invert Index Table ? ——NO——> return failure
     |
    YES
     |
Order by Similarity
     |
Page Exist && Hash Correct ——NO——> return failure
     |
    YES
     |
 Get Excerpt
     |
Json Serialization 
     |
 Add Cache 
     |
 return result

```
#程序样例：
```
[usr1@localhost SearchEngine]$ ./test/client.o 
Please input query word:
理论
result: 
[
   {
      "content" : "科学院经济研究所“国外现代经济理论前沿研究”课题的经费资助，...的学者吸收国外主流劳动经济学的理论成果，以劳动力的市场...的应用研究方面，而且在它的基础理论建设方面作出更积极的努力。...",
      "title" : "【 标  题 】论国外劳动经济学中的人文因素",
      "url" : "/zrepo/code/SearchEngine/data/text/C34-Economy0393.txt"
   }
]

Please input query word:
```
