#pragma once

#include "common.h"

#include <map>
#include <vector>
#include <list>
#include <set>

class ARG_LIST
{
public:
	ARG_LIST() {}
	ARG_LIST(const ARG_LIST &);

	void print() const;

	std::list<int> args;
	typedef std::list<int>::iterator iterator;
	typedef std::list<int>::const_iterator const_iterator;

	bool operator<(const ARG_LIST &) const;
	bool operator==(const ARG_LIST &) const;
};

class CNF
{
public:
	CNF() : n_variables(0) {}
	CNF(int, const ARG_LIST &);
	CNF(const CNF &);
	~CNF();

	bool reindex_variable();
	void index() const; // index predicates

	void replace(const int *dict);
	bool erase_contradiction(); // modified to only find contradiction
	bool factor(std::set<CNF> &res) const;

	void print() const;

	std::map<int, std::set<ARG_LIST>*> clauses;
	typedef std::map<int, std::set<ARG_LIST>*>::iterator iterator;
	typedef std::map<int, std::set<ARG_LIST>*>::const_iterator const_iterator;

	mutable std::set<int> key_set;
	typedef std::set<int>::iterator iterator_key;
	typedef std::set<int>::const_iterator const_iterator_key;

	unsigned n_variables;

	void operator|=(const CNF &);

	bool operator<(const CNF &) const;
	
private:
	bool _factor(std::list<int*> &dicts) const;
};

class CNFs
{
public:
	CNFs() {}
	CNFs(const CNFs &);
	~CNFs();

	void reindex_variable(); // this func will destroy predicate index
	void index(); // index predicates

	void erase_contradiction();
	void factor();

	void print() const;

	std::set<CNF> sentences;
	typedef std::set<CNF>::iterator iterator;
	typedef std::set<CNF>::const_iterator const_iterator;

	std::map<int, std::list<iterator>*> func_table;
	typedef std::map<int, std::list<iterator>*>::iterator iterator_table;
	typedef std::map<int, std::list<iterator>*>::const_iterator const_iterator_table;

	void operator&=(const CNFs &);
	void combine(const CNFs &, const CNFs &);
};
