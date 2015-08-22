#include<iostream>
#include<unistd.h>
#include<fstream>
#include<iconv.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<iconv.h>
#define handle_error(msg ) \
	do {  perror(msg); exit(EXIT_FAILURE); } while(0)


using namespace std;


//将path_src目录下的文件转码到path_dst目录
void gbk2utf(char * path_src, char* path_dst)
{
	DIR* pdir1 = opendir( path_src);
	if( pdir1 == NULL )
		handle_error("opendir errno:");
	DIR* pdir2 = opendir( path_dst);
	if( pdir2 == NULL)
		handle_error( "opendir errno:");
	struct dirent * pdirent =NULL;
	size_t len1 = strlen(path_src );
	size_t len2	= strlen(path_dst);
	while( pdirent = readdir( pdir1), pdirent !=NULL)
	{
		if( strcmp( pdirent->d_name , ".")==0 || strcmp(pdirent->d_name, "..")==0 )
			continue;	
		sprintf(path_src+len1,"/%s",pdirent->d_name );
		sprintf(path_dst+len2,"/%s", pdirent->d_name);
		struct stat buf;	
		int ret = stat( path_src, &buf);
		if( ret == -1)
			handle_error("stat errno: ");
//如果是目录，则深度遍历目录
		if( S_ISDIR(buf.st_mode) )
		{
			int ret = access( path_dst, F_OK);
			if( ret != 0)
				if(0 !=	mkdir( path_dst, buf.st_mode))
					handle_error(" mkdir: ");
			gbk2utf( path_src, path_dst);
		}
//如果是文件，则将文件编码从gb10803转码到utf-8
		else if( S_ISREG( buf.st_mode))
		{
			ifstream   ifs(path_src,ios::binary|ios::in);
			if( !ifs)
				handle_error("ifs: ")	;
			streambuf * pbuf = ifs.rdbuf();	
			size_t size =  pbuf->pubseekoff(0,ifs.end,ios::in);
			pbuf->pubseekpos(0,ios::in);
			char * buff_src = new char[size];
			memset(buff_src,0,size);
			if( buff_src==NULL)
				handle_error("new:");
			pbuf->sgetn(buff_src, size);
			char * buff_dst = new char [1024*1024];
			memset(buff_dst,0,1024*1024);
			if( buff_dst == NULL)
				handle_error("new:");
			iconv_t cd = iconv_open( "utf-8", "gb18030");
			if( cd == (iconv_t )-1)
				handle_error("iconv error:");
			char * bf1 = buff_src;
			char * bf2 = buff_dst;
			size_t oleft = 1024*1024-1;	
			size_t  cnt = iconv(cd, &bf1,&size, &bf2, &oleft);
//转码失败，则输出转码失败的原因和文件
			if( cnt == (size_t)-1)	
			{
				printf("%s:-->%s\n",strerror(errno), path_src);
			}
//转码成功，则保存到文件
			else
			{
				ofstream   ofs(path_dst,ios::binary|ios::out);
				if( !ofs)
					handle_error("ofs: ");		
				ofs.write(buff_dst, 1024*1024-1 - oleft);
				if( !ofs)
					handle_error("ofs write:");		
				ofs.close();
			}
			iconv_close(cd);
			delete [] buff_src;
			delete [] buff_dst;
			ifs.close();
		}
		*(path_src+len1)= '\0';
		*(path_dst+len2)='\0';
	}
	closedir(pdir1);
	closedir(pdir2);
}




int main(int argc, char *argv[])
{
	if( argc< 3)
		handle_error("argument error ");
	char * path1 = new char [4096];
	if( path1 == NULL)
		handle_error("new :");
	memset(path1, 0, 4096);
	char * path2 = new char [4096];
	if( path2 == NULL)
		handle_error("new: ");
	memset(path2, 0, 4096);
	memcpy( path1, argv[1], strlen(argv[1]));
	memcpy( path2, argv[2], strlen(argv[2]));
	DIR * pdir1 = opendir( path1);
	if( pdir1 == NULL)
	{
		perror("opendir errno :");
		exit(1);
	}
	closedir(pdir1);
	DIR * pdir2 = opendir( path2 );
	if( pdir2 == NULL)
	{
		perror( "opendir errno :");
		exit(1);
	}
	closedir(pdir2);
	gbk2utf(path1,path2);
}


