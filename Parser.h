#pragma once

#include "common.h"

#include <string>
#include <string.h>

class Parser
{
public:
	static inline bool isCharactor(char c);

	Parser(unsigned size) : ptr(0), len(0)
	{
		line = new char[size];
		word = new char[size];
	}
	~Parser()
	{
		if (line) delete[] line;
		if (word) delete[] word;
	}

	void init();
	void feed(const char *line);
	std::string next(); // "" for end
	std::string lookahead(int n);
	
private:
	unsigned ptr, len;
	char *line, *word;
};

inline bool Parser::isCharactor(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9');
}
