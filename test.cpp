#include "test.h"

bool g_b_Switch = true;

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

bool task( std::string command, std::string args )
{
        if( !command.compare( "mkdir" ) )
        {

                std::regex matchPatten( "[^\\s]+,[0-9]{1,5}" );

                if( !regex_match( args, matchPatten ) )
                	return false;

                if( NULL == opendir( ( args.substr( 0, args.find_first_of( "," ) ) ).c_str() ) )
                {
                                           
			mkdir( ( args.substr( 0, args.find_first_of( "," )  ) ).c_str(), stoi( args.substr( args.find_first_of( "," ) + 1 ) ) );
                 
		}

        }
        else if( !command.compare( "cp" ) )
        {
                std::cout << "cp ";
        }
	else
		return false; //no match in config file

        return true;
}

bool inConfigFile( std::string sMessage, std::map<std::string, std::string> & configMap )
{

        auto findResult = configMap.find( sMessage.substr( 0, sMessage.find_first_of( "|" ) ) ); // we expect all message are in the form of aaa|whatever

        if( findResult != configMap.end() )
		return task( findResult->second, sMessage.substr( sMessage.find_first_of( "|" ) + 1 ) );
	
	return false;

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

                if( match( sRedisReply, configMap) )
                        ;
                else
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

