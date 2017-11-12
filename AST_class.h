#pragma once

#include <string>
#include <map>
#include <list>
#include <vector>
#include "CNF.h"

typedef enum { NONE = 0, NOT, AND, OR, IMPLY } OPERATOR;

inline OPERATOR strToOp(std::string op)
{
	switch (op[0])
	{
	case '~':
		return NOT;
	case '&':
		return AND;
	case '|':
		return OR;
	case '=':
		if (op.length() > 1 && op[1] == '>')
			return IMPLY;
	default:
		return NONE;
	}
}

inline std::string opToStr(OPERATOR op)
{
	switch (op)
	{
	case NOT:
		return "~";
	case AND:
		return "&";
	case OR:
		return "|";
	case IMPLY:
		return "=>";
	default:
		return "";
	}
}

class Expression
{
public:
	virtual void negation() = 0;
	virtual void simplify() = 0;
	virtual CNFs *cnfs() = 0;

	virtual void print() = 0;

	virtual ~Expression() {}
};

class Binary : public Expression
{
public:
	void negation();
	void simplify();
	CNFs *cnfs();

	void print();

	Binary() : op(NONE), first(0), second(0) {}
	~Binary();

	OPERATOR op;
	Expression *first, *second;
};

class Unary : public Expression
{
public:
	void negation();
	void simplify();
	CNFs *cnfs();

	void print();

	Unary() : op(NONE), first(0) {}
	~Unary();

	OPERATOR op;
	Expression *first;
};

class Function : public Expression
{
public:
	static std::map<std::string, int> dict;
	static std::vector<std::string> dict_itoa;
	static void clear();
	static int index(std::string);
	
	void negation();
	void simplify() {}
	CNFs *cnfs();

	void print();

	void add(Expression *arg);

	Function(std::string name) : op(NONE)
	{
		id = index(name);
	}
	~Function();

	OPERATOR op;
	int id;
	std::list<Expression*> arg_list;
};

class Constant : public Expression
{
public:
	static std::map<std::string, int> dict;
	static std::vector<std::string> dict_itoa;
	static void clear();
	static int index(std::string);

	void negation() {}
	void simplify() {}
	CNFs *cnfs() { return (CNFs*)(long)id; }

	void print();

	Constant(std::string name)
	{
		id = index(name);
	}
	~Constant() {}

	int id;
};

class Variable : public Expression
{
public:
	void negation() {}
	void simplify() {}
	CNFs *cnfs() { return (CNFs*)(long)(~id); }

	void print();

	Variable(char name)
	{
		id = name - 'a';
	}
	~Variable() {}

	int id;
};
