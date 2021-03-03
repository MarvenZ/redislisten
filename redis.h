#include <string>
#include <hiredis/hiredis.h>

bool createFolder( std::string );

bool cp( std::string command );

class Redis
{
public:

	Redis(){}

	~Redis()
	{
		this->_connect = NULL;
		this->_reply = NULL;
	}

	int connect( std::string host, int port);

	void lpush( std::string sDestList, std::string sValue );

	std::string rpop( std::string sListName );

protected:

	redisContext* _connect;
	redisReply* _reply;

};


