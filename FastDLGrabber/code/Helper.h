#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <set>

class CHelper
{
public:
	static inline std::string FileToString(const std::string _fileName)
	{
		std::ifstream file(_fileName);
		return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>{} };
	}

	// Checks if String ends with (case unsensitive)
	static inline bool EndsWith(const std::string _string, const std::string _suffix)
	{
		return  std::regex_search(_string,
				std::regex(std::string(_suffix) + "$", 
				std::regex_constants::icase));
	}

	static inline bool StartsWith(std::string _string, std::string _prefix, bool _caseSensitive = false)
	{
		if (!_caseSensitive)
		{
			std::transform(_string.begin(), _string.end(), _string.begin(), ::tolower);
			std::transform(_prefix.begin(), _prefix.end(), _prefix.begin(), ::tolower);
		}
		return ((_prefix.size() <= _string.size()) && std::equal(_prefix.begin(), _prefix.end(), _string.begin()));
	}

	static inline std::set<std::string> ExtractHyperlinks(std::string _htmlFileName)
	{
		static const std::regex hl_regex("<a href=\"(.*?)\">", std::regex_constants::icase);

		const std::string text = FileToString(_htmlFileName);

		return { std::sregex_token_iterator(text.begin(), text.end(), hl_regex, 1),
				 std::sregex_token_iterator{} };
	}
};