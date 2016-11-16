#pragma once

#include <string>
#include <map>
#include <list>
#include <stdio.h>
#include "CNF.h"

typedef enum { NONE = 0, NOT, AND, OR, IMPLY } OPERATOR;
OPERATOR strToOp(std::string op)
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

std::string opToStr(OPERATOR op)
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

void Binary::negation()
{
	switch (op)
	{
	case AND:
		op = OR;
		first->negation();
		second->negation();
		break;
	case OR:
		op = AND;
		first->negation();
		second->negation();
		break;
	case IMPLY:
		op = AND;
		second->negation();
		break;
	default:
		break;
	}
}

void Binary::simplify()
{
	if (op == IMPLY)
	{
		op = OR;
		first->negation();
	}
	first->simplify();
	second->simplify();
}

CNFs *Binary::cnfs()
{
	switch (op)
	{
	case AND:
	{
		CNFs *cnfs_f = first->cnfs(), *cnfs_s = second->cnfs();
		*cnfs_f &= *cnfs_s;
		delete cnfs_s;
		return cnfs_f;
	}
	case OR:
	{
		CNFs *cnfs_f = first->cnfs(), *cnfs_s = second->cnfs();
		CNFs *res = new CNFs();
		res->combine(*cnfs_f, *cnfs_s);
		delete cnfs_f;
		delete cnfs_s;
		return res;
	}
	default:
		return new CNFs();
	}
}

void Binary::print()
{
	printf("(");
	first->print();
	printf(" %s ", opToStr(op).c_str());
	second->print();
	printf(")");
}

Binary::~Binary()
{
	if (first) delete first;
	if (second) delete second;
}

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

void Unary::negation()
{
	if (op == NONE)
		op = NOT;
	else
		op = NONE;
}

void Unary::simplify()
{
	if (op == NOT)
	{
		op = NONE;
		first->negation();
	}
	// op must be NONE
	first->simplify();
}

CNFs *Unary::cnfs()
{
	return first->cnfs();
}

void Unary::print()
{
	printf("%s", opToStr(op).c_str());
	first->print();
}

Unary::~Unary()
{
	if (first) delete first;
}

class Function : public Expression
{
public:
	static std::map<std::string, int> dict;
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

std::map<std::string, int> Function::dict;

void Function::clear()
{
	dict.clear();
}

int Function::index(std::string name)
{
	return dict.insert(std::pair<std::string, int>(name, dict.size())).first->second;
}

void Function::negation()
{
	if (op == NONE)
		op = NOT;
	else
		op = NONE;
}

CNFs *Function::cnfs()
{
	ARG_LIST *args = new ARG_LIST();
	for (std::list<Expression*>::iterator iter = arg_list.begin(); iter != arg_list.end(); ++iter)
		args->args.push_back((int)((*iter)->cnfs()));
	int _id = id;
	if (op == NOT)
		_id = ~_id;
	CNF *cnf = new CNF(_id, args);
	CNFs *res = new CNFs();
	res->sentences.push_back(cnf);
	return res;
}

void Function::print()
{
	printf("%sF%d(", opToStr(op).c_str(), id);
	bool first = true;
	for (std::list<Expression*>::iterator iter = arg_list.begin(); iter != arg_list.end(); ++iter)
	{
		if (first)
			first = false;
		else
			printf(", ");
		(*iter)->print();
	}
	printf(")");
}

void Function::add(Expression *arg)
{
	arg_list.push_back(arg);
}

Function::~Function()
{
	for (std::list<Expression*>::iterator iter = arg_list.begin(); iter != arg_list.end(); ++iter)
		delete *iter;
}

class Constant : public Expression
{
public:
	static std::map<std::string, int> dict;
	static void clear();
	static int index(std::string);

	void negation() {}
	void simplify() {}
	CNFs *cnfs() { return (CNFs*)id; }

	void print();

	Constant(std::string name)
	{
		id = index(name);
	}
	~Constant() {}

	int id;
};

std::map<std::string, int> Constant::dict;

void Constant::clear()
{
	dict.clear();
}

int Constant::index(std::string name)
{
	return dict.insert(std::pair<std::string, int>(name, dict.size())).first->second;
}

void Constant::print()
{
	printf("C%d", id);
}

class Variable : public Expression
{
public:
	void negation() {}
	void simplify() {}
	CNFs *cnfs() { return (CNFs*)(~id); }

	void print();

	Variable(char name)
	{
		id = name - 'a';
	}
	~Variable() {}

	int id;
};

void Variable::print()
{
	printf("%c", 'a' + id);
}
