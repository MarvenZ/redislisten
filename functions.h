#include <string>
#include <functional>

typedef
std::function<bool( std::string )>
func_bool__string;

extern std::map< std::string , func_bool__string > funcMap;

bool task( std::string );

bool cp( std::string );

bool ll( std::string );

bool createFolder( std::string );
