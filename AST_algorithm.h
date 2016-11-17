#pragma once

#include <string>
#include "AST_class.h"
#include "Parser.h"

Expression *EXP(Parser &p);
Expression *OP(Parser &p);
Expression *FUNC(Parser &p, std::string name);

Expression *buildAST(Parser &p)
{
	return EXP(p);
}

Expression *EXP(Parser &p)
{
	std::string next = p.next();
	switch (next[0])
	{
	case '(':
		return OP(p);
	case '~':
	{
		Unary *ptr = new Unary();
		ptr->op = NOT;
		ptr->first = EXP(p);
		return ptr;
	}
	default: // word
		if (Parser::isCharactor(next[0]))
			return FUNC(p, next);
		printf("Syntax error: %s\n", next.c_str());
	}
	return 0;
}

Expression *OP(Parser &p)
{
	Expression *ptr = 0;
	std::string next;
	while ((next = p.next()).length() > 0)
	{
		switch (next[0])
		{
		case '(':
		{
			Binary *_ptr = new Binary();
			_ptr->first = OP(p);
			ptr = _ptr;
			break;
		}
		case '~':
		{
			Unary *_ptr = new Unary();
			_ptr->op = NOT;
			_ptr->first = EXP(p);
			ptr = _ptr;
			break;
		}
		case '&':
		case '|':
		case '=':
		{
			Binary *_ptr = (Binary*)ptr;
			_ptr->op = strToOp(next);
			_ptr->second = EXP(p);
			break;
		}
		case ')':
			return ptr;
		default:
			// word
			if (Parser::isCharactor(next[0]))
			{
				Binary *_ptr = new Binary();
				_ptr->first = FUNC(p, next);
				ptr = _ptr;
				break;
			}
			// unknown
			printf("Syntax error: %s\n", next.c_str());
			return 0;
		}
	}
}

Expression *FUNC(Parser &p, std::string name)
{
	Function *ptr = new Function(name);
	std::string next;
	while ((next = p.next()).length() > 0)
	{
		switch (next[0])
		{
		case ')':
			return ptr;
		case '(':
		case ',':
			break;
		default: //word
			if ('a' <= next[0] && next[0] <= 'z')
				ptr->add(new Variable(next[0]));
			else if ('A' <= next[0] && next[0] <= 'Z')
				ptr->add(new Constant(next));
			else
			{
				printf("Syntax error: %s\n", next.c_str());
				return 0;
			}
		}
	}
}
