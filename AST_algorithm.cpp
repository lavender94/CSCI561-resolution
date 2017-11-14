#include <string>
#include "AST_class.h"
#include "AST_algorithm.h"
#include "Parser.h"

Expression *buildAST(Parser &p)
{
	return EXP(p);
}

Expression *EXP(Parser &p)
{
    Expression *ptr = 0;
	std::string next;
	while ((next = p.next()).length() > 0)
	{
    	switch (next[0])
    	{
    	case '(':
    		return OP(p);
    	case '~':
    	{
    		Unary *_ptr = new Unary();
    		_ptr->op = NOT;
    		_ptr->first = EXP(p);
    		return _ptr;
    	}
    	case '&':
		case '|':
		case '=':
		{ // ptr
		    Binary *_ptr = new Binary();
	        _ptr->first = ptr;
			_ptr->op = strToOp(next);
			_ptr->second = EXP(p);
			return _ptr;
        }
    	default: // word
    		if (Parser::isCharactor(next[0]))
    		{
    		    ptr = FUNC(p, next);
    		    std::string lookahead = p.lookahead(0);
                if (lookahead != "" && lookahead[0] == ')')
                    return ptr;
    		    break;
            }
    		printf("Syntax error: %s\n", next.c_str());
    	}
    }
    if (!ptr)
	    printf("Empty expression\n");
	return ptr;
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
	return ptr;
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
