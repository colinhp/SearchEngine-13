#ifndef _BUILDING_H_
#define _BUILDING_H_

#include"Page.h"
#include"Iterm.h"
#include<vector>

//守护进程
void my_daemon( void );

//初始化函数
bool initiation( );
//获取摘要
bool get_excerpt( size_t doc_id , Iterm & iterm,  Page & page );
//返回查询结果
bool get_result( std::vector< size_t > &docid_vec, Iterm & iterm,  std::vector< Page > & result_vec );

#endif
