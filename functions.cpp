#include <iostream>
#include <map> 

#include <regex>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstddef>

#include "functions.h"

bool task( std::string message )
{
    int seperateLocation = message.find_first_of( "|" );

    if( !seperateLocation )
        return false;

    std::string command = message.substr( 0, seperateLocation );
    std::string args = message.substr( seperateLocation + 1 );

    auto findResult = funcMap.find( command );

    if( findResult == funcMap.end() )
        return false;
        
    return findResult->second( args );
}

bool createFolder( std::string args )
{
    std::regex matchPatten( "[^\\s]+,[0-9]{1,5}" );
    if( !regex_match( args, matchPatten ) )
        return false;
    
    int commaLocation = args.find_first_of( "," );
    auto firstArg = args.substr( 0,commaLocation ).c_str();
    auto secondArg = stoi( args.substr( commaLocation + 1 ) );    

    if( NULL != opendir( firstArg ) )
        return false;
        
    return !!mkdir( firstArg, secondArg );
}

bool cp( std::string args )
{
    std::cout << args;
    return true;
}

bool ll( std::string args )
{
    std::cout << args;
    return true;
}