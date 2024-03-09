#include "StringUtil.h"

bool StringUtil::beginsWith( const std::string& first, const std::string &second )
{
	return ( first.compare( 0, second.length(), second ) == 0 );
}

// -------------------------------------------------------------
std::string StringUtil::after( const std::string& first, const std::string &second )
{
	return first.substr( second.length() );
}

// -------------------------------------------------------------
std::vector<std::string> StringUtil::tokenize( const std::string& str )
{
	std::string work = str;
	std::vector<std::string> arr;

	bool done = false;
	while( !done )
	{
		std::size_t found = work.find_first_of(" ");
		if( found == std::string::npos )
		{
			arr.push_back( work );
			done = true;
		}		
		else
		{
			std::string first = work.substr( 0, found );
			std::string rest = work.substr( found + 1 );
			arr.push_back( first );
			work = rest;
		}
	}

	return arr;
}