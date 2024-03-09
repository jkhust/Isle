#pragma once

#include <string>
#include <vector>

class StringUtil
{
public:
	static bool beginsWith( const std::string& first, const std::string& second );
	static std::string after( const std::string& first, const std::string &second );
	static std::vector<std::string> tokenize( const std::string& str );
};