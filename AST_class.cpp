#include <string>
#include <map>
#include <list>
#include <vector>
#include <stdio.h>
#include "CNF.h"
#include "AST_class.h"

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

std::map<std::string, int> Function::dict;
std::vector<std::string> Function::dict_itoa;

void Function::clear()
{
	dict.clear();
	dict_itoa.clear();
}

int Function::index(std::string name)
{
	int ind = dict.insert(std::pair<std::string, int>(name, dict.size())).first->second;
	if (ind == dict_itoa.size())
		dict_itoa.push_back(name);
	return ind;
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
	ARG_LIST args;
	for (std::list<Expression*>::iterator iter = arg_list.begin(); iter != arg_list.end(); ++iter)
		args.args.push_back((long)((*iter)->cnfs()));
	int _id = id;
	if (op == NOT)
		_id = ~_id;
	CNF cnf(_id, args);
	CNFs *res = new CNFs();
	res->sentences.insert(cnf);
	return res;
}

void Function::print()
{
	printf("%s%s(", opToStr(op).c_str(), dict_itoa[id].c_str());
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

std::map<std::string, int> Constant::dict;
std::vector<std::string> Constant::dict_itoa;

void Constant::clear()
{
	dict.clear();
	dict_itoa.clear();
}

int Constant::index(std::string name)
{
	int ind = dict.insert(std::pair<std::string, int>(name, dict.size())).first->second;
	if (ind == dict_itoa.size())
		dict_itoa.push_back(name);
	return ind;
}

void Constant::print()
{
	printf("%s", dict_itoa[id].c_str());
}

void Variable::print()
{
	printf("%c", 'a' + id);
}
