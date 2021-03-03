#include "redis.h"
#include <iostream>

int Redis::connect( std::string host, int port)
{
    this->_connect = redisConnect( host.c_str(), port);
    if( this->_connect != NULL && this->_connect->err)
    {
        std::cout << "connect error: " << this->_connect->errstr << std::endl;
        return 0;
    }

    return 1;
}

void Redis::lpush( std::string sDestList, std::string sValue )
    {
            redisCommand( this->_connect, "LPUSH %s %s", sDestList.c_str(), sValue.c_str() );
    }


std::string Redis::rpop( std::string sListName )
{
    this->_reply = ( redisReply* ) redisCommand( this->_connect, "RPOP %s", sListName.c_str() );
    
    if( this->_reply->type == 4 ) //list doesn't exsit
    {
        
        freeReplyObject( this->_reply );
        return "error! list is empty";
    }

    std::string str = this->_reply->str;
            freeReplyObject( this->_reply );    	       
    return str;

}