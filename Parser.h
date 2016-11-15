#pragma once

#include <string>
#include <string.h>

class Parser
{
public:
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
	std::string next();
	
private:
	void skip_space();
	bool isCharactor(char c);

	unsigned ptr, len;
	char *line, *word;
};

void Parser::init()
{
	ptr = 0;
	skip_space();
}

void Parser::feed(const char *line)
{
	len = strlen(line);
	memcpy(this->line, line, len + 1);
	init();
}

std::string Parser::next()
{
	if (ptr == len)
		return "";
	char c = line[ptr++];
	skip_space();
	switch (c)
	{
	case '(':
		return "(";
	case ')':
		return ")";
	case '~':
		return "~";
	case '&':
		return "&";
	case '|':
		return "|";
	case '=':
		ptr += 1;
		skip_space();
		return "=>";
	case ',':
		return ",";
	default: // word
		if (isCharactor(c))
		{
			word[0] = c;
			unsigned pw = 1;
			while (isCharactor(line[ptr]))
			{
				word[pw++] = line[ptr++];
				skip_space();
			}
			word[pw] = '\0';
			return std::string(word);
		}
		return "";
	}
}

void Parser::skip_space()
{
	while (line[ptr] == ' ') ptr += 1;
}

bool Parser::isCharactor(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}
