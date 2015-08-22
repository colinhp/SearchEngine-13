#include<iostream>
#include<map>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<string>


#define handle_error(msg) \
            do { perror( msg); exit(1); } while(1)

int main( int argc, char * argv[])
{
	char * fin = argv[1];
	char * fout = argv[2];

	std::ifstream ifs( fin);
	if( !ifs)
		handle_error( "ifs error");
	std::map< std::string ,std::string > stop;
	std::string word;
	while( ifs >> word)
		stop[word]=word;
	ifs.close();

	std::ofstream ofs( fout );
	if( !ofs)
		handle_error("ofs error");
	auto it = stop.begin();
	for( ;it != stop.end(); it++)
	{
		ofs << it->second <<std::endl;	
	}

	ofs.close();


}
