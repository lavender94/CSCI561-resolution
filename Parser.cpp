#include <string>
#include <string.h>
#include "Parser.h"

void Parser::init()
{
	ptr = 0;
}

void Parser::feed(const char *line)
{
	len = strlen(line);
	unsigned _p = 0;
	for (unsigned p = 0; p < len; ++p)
		if (line[p] != ' ')
			(this->line)[_p++] = line[p];
	(this->line)[_p] = '\0';
	init();
}

std::string Parser::next()
{
	if (ptr == len)
		return "";
	char c = line[ptr++];
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
			}
			word[pw] = '\0';
			return std::string(word);
		}
		return "";
	}
}
