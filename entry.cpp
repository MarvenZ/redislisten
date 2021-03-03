#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <regex>

#include <fstream> //read config file
#include <map> //store config file

#include "redis.h"
#include "functions.h"

bool g_b_Switch = true;

std::map< std::string , func_bool__string > funcMap = 
{
	{ "createFolder", createFolder },
	{ "cp", cp },
	{ "ll", ll }

};

bool readConfigFile( std::map<std::string, std::string> & configMap)
{
	std::ifstream inConfFile( "/var/hw/cpp/configFile.conf" );

	if( inConfFile )
	{
		std::string line;

		while ( getline ( inConfFile, line ) )
			configMap.insert( std::pair<std::string, std::string>( line.substr(0, line.find_first_of( "=" ) ), line.substr( line.find_first_of( "=" ) + 1 ) ) );
	}
	else
	{
		std::cout <<"no such file" << std::endl;
		return false;
	}

	inConfFile.close();
	return true;
}

bool inConfigFile( std::string sMessage, std::map<std::string, std::string> & configMap )
{
    auto findResult = configMap.find( sMessage.substr( 0, sMessage.find_first_of( "|" ) ) );
	
	if( findResult == configMap.end() )
        return false;

	return task( sMessage );

}

bool match( std::string sMessage , std::map<std::string, std::string> & configMap )
{
	std::regex matchPatten( "[^\\s]+\\|[^\\s]*" );
                
	if( !regex_match( sMessage, matchPatten ) )
		return false;   //if message are not in the form of aaa|whatever, leave it    
	
	return inConfigFile( sMessage, configMap );
}

int redisListen( std::map<std::string, std::string> & configMap )
{

	Redis *redis = new Redis();
	if( !redis->connect("127.0.0.1", 6379) )
	{
		std::cout << "connection error!" << std::endl;
		return 1;
	}

	while(true)
	{
		std::string sRedisReply = redis->rpop( "mq" );

		if( sRedisReply == "error! list is empty" )
				continue;

		if( !match( sRedisReply, configMap) )
			redis->lpush( "unsolved", sRedisReply );
	}

	delete redis;
	
	return 0;
}

bool SavePid( int pid, const char* pid_path )
{
	std::ofstream pidstream( pid_path, std::ios::out );
	if( !pidstream )
	{
		return false;
	}
	pidstream << pid;
	pidstream.close();
	return true;
}

void QuitListen( int sig )
{
	if( sig == SIGUSR2 )
		g_b_Switch = false;
}

int main(int argc, char** argv )
{	
	std::map<std::string, std::string> configMap;

	if( !readConfigFile( configMap ) )
		return 3;

	auto pid = fork();
	if( pid )
	{
		if( pid == -1 )
			return 1;

		return SavePid( pid, "/run/redisListen.pid" ) ? 0 : 2;
	}
	else
	{
		signal( SIGUSR2, QuitListen );
		while( g_b_Switch )
			redisListen( configMap );
	}

	return 0;
}

