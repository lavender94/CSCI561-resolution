#pragma once

#include <map>
#include <vector>
#include <list>
#include <string>
#include <set>
#include <algorithm>

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

ARG_LIST::ARG_LIST(const ARG_LIST &b)
{
	args.assign(b.args.begin(), b.args.end());
}

void ARG_LIST::print() const
{
	printf("(");
	bool first = true;
	for (const_iterator iter = args.begin(); iter != args.end(); ++iter)
	{
		if (first)
			first = false;
		else
			printf(", ");
		if (*iter < 0) // variable
			printf("%c", 'a' + ~(*iter));
		else
			printf("C%d", *iter);
	}
	printf(")");
}

inline bool ARG_LIST::operator<(const ARG_LIST &b) const
{
	const std::list<int> &bargs = b.args;
	if (args.size() < bargs.size())
		return true;
	if (args.size() > bargs.size())
		return false;
	for (const_iterator ai = args.begin(), bi = bargs.begin(); ai != args.end(); ++ai, ++bi)
	{
		if (*ai < *bi)
			return true;
		if (*ai > *bi)
			return false;
	}
	return false;
}

inline bool ARG_LIST::operator==(const ARG_LIST &b) const
{
	const std::list<int> &bargs = b.args;
	if (args.size() == bargs.size())
	{
		for (const_iterator ai = args.begin(), bi = bargs.begin(); ai != args.end(); ++ai, ++bi)
			if (*ai != *bi)
				return false;
		return true;
	}
	return false;
}

class CNF
{
public:
	CNF() : n_variables(0) {}
	CNF(int, const ARG_LIST &);
	CNF(const CNF &);
	~CNF();

	void reindex_variable();
	void index(); // index predicates

	void replace(int dict[]);

	void print() const;

	std::map<int, std::set<ARG_LIST>*> clauses;
	typedef std::map<int, std::set<ARG_LIST>*>::iterator iterator;
	typedef std::map<int, std::set<ARG_LIST>*>::const_iterator const_iterator;

	std::set<int> key_set;
	typedef std::set<int>::iterator iterator_key;
	typedef std::set<int>::const_iterator const_iterator_key;

	unsigned n_variables;

	void operator|=(const CNF &);
};

CNF::CNF(int id, const ARG_LIST &arg_list) : n_variables(0)
{
	std::set<ARG_LIST> *al = new std::set<ARG_LIST>();
	al->insert(arg_list);
	clauses[id] = al;
}

CNF::CNF(const CNF &b) : n_variables(b.n_variables), key_set(b.key_set)
{
	clauses.clear();
	for (const_iterator citer = b.clauses.begin(); citer != b.clauses.end(); ++citer)
		clauses[citer->first] = new std::set<ARG_LIST>(citer->second->begin(), citer->second->end());
}

CNF::~CNF()
{
	for (iterator iter = clauses.begin(); iter != clauses.end(); ++iter)
		delete iter->second;
}

void CNF::reindex_variable()
{
	std::map<int, int> reindex;
	for (iterator iter = clauses.begin(); iter != clauses.end(); ++iter)
	{
		std::set<ARG_LIST> *l_args = iter->second;
		std::list<ARG_LIST> backup;
		std::set<ARG_LIST>::iterator it = l_args->begin();
		while (it != l_args->end())
		{
			bool modified = false;
			ARG_LIST temp;
			for (ARG_LIST::const_iterator _iter = it->args.begin(); _iter != it->args.end(); ++_iter)
				if (*_iter < 0) // variable
				{
					int id = reindex.insert(std::pair<int, int>(*_iter, ~(int)(reindex.size()))).first->second;
					if (id != *_iter)
						modified = true;
					temp.args.push_back(id);
				}
				else
					temp.args.push_back(*_iter);
			if (modified)
			{
				backup.push_back(temp);
				std::set<ARG_LIST>::iterator _it = it;
				++it;
				l_args->erase(_it);
			}
			else
				++it;
		}
		l_args->insert(backup.begin(), backup.end());
	}
	n_variables = reindex.size();
}

void CNF::index()
{
	key_set.clear();
	for (iterator iter = clauses.begin(); iter != clauses.end(); ++iter)
		key_set.insert(iter->first);
}

void CNF::replace(int dict[])
{
	for (iterator iter = clauses.begin(); iter != clauses.end(); ++iter)
	{
		std::set<ARG_LIST> *l_args = iter->second;
		std::list<ARG_LIST> backup;
		std::set<ARG_LIST>::iterator it = l_args->begin();
		while (it != l_args->end())
		{
			bool modified = false;
			ARG_LIST temp;
			for (ARG_LIST::const_iterator _iter = it->args.begin(); _iter != it->args.end(); ++_iter)
				if (*_iter < 0) // variable
				{
					int id = dict[~(*_iter)];
					if (id != *_iter)
						modified = true;
					temp.args.push_back(id);
				}
				else
					temp.args.push_back(*_iter);
			if (modified)
			{
				backup.push_back(temp);
				std::set<ARG_LIST>::iterator _it = it;
				++it;
				l_args->erase(_it);
			}
			else
				++it;
		}
		l_args->insert(backup.begin(), backup.end());
	}
}

void CNF::print() const
{
	bool first = true;
	for (const_iterator iter = clauses.begin(); iter != clauses.end(); ++iter)
	{
		std::set<ARG_LIST> *l_args = iter->second;
		for (std::set<ARG_LIST>::const_iterator it = l_args->begin(); it != l_args->end(); ++it)
		{
			if (first)
				first = false;
			else
				printf(" | ");
			if (iter->first < 0) // not
				printf("~F%d", ~(iter->first));
			else
				printf("F%d", iter->first);
			it->print();
		}
	}
}

void CNF::operator|=(const CNF &b)
{
	for (const_iterator citer = b.clauses.begin(); citer != b.clauses.end(); ++citer)
	{
		iterator iter = clauses.find(citer->first);
		if (iter == clauses.end())
			iter = clauses.insert(std::pair<int, std::set<ARG_LIST>*>(citer->first, new std::set<ARG_LIST>())).first;
		std::set<ARG_LIST> *l_args = iter->second;
		const std::set<ARG_LIST> *c_l_args = citer->second;
		l_args->insert(c_l_args->begin(), c_l_args->end());
	}
}

class CNFs
{
public:
	CNFs() {}
	CNFs(const CNFs &);
	~CNFs();

	void reindex_variable();
	void index(); // index predicates

	void print() const;

	std::vector<CNF*> sentences;
	typedef std::vector<CNF*>::iterator iterator;
	typedef std::vector<CNF*>::const_iterator const_iterator;

	std::map<int, std::set<unsigned>*> func_table;
	typedef std::map<int, std::set<unsigned>*>::iterator iterator_table;
	typedef std::map<int, std::set<unsigned>*>::const_iterator const_iterator_table;

	void operator&=(const CNFs &);
	void combine(const CNFs &, const CNFs &);
};

CNFs::CNFs(const CNFs &b)
{
	sentences.clear();
	sentences.reserve(b.sentences.size());
	for (const_iterator citer = b.sentences.begin(); citer != b.sentences.end(); ++citer)
		sentences.push_back(new CNF(**citer));
	for (const_iterator_table citer = b.func_table.begin(); citer != b.func_table.end(); ++citer)
		func_table[citer->first] = new std::set<unsigned>(*(citer->second));
}

CNFs::~CNFs()
{
	for (iterator iter = sentences.begin(); iter != sentences.end(); ++iter)
		delete *iter;
	for (iterator_table iter = func_table.begin(); iter != func_table.end(); ++iter)
		delete iter->second;
}

void CNFs::reindex_variable()
{
	for (iterator iter = sentences.begin(); iter != sentences.end(); ++iter)
		(*iter)->reindex_variable();
}

void CNFs::index()
{
	for (iterator iter = sentences.begin(); iter != sentences.end(); ++iter)
		(*iter)->index();
	func_table.clear();
	unsigned p = 0;
	for (iterator iter = sentences.begin(); iter != sentences.end(); ++iter, ++p)
		for (CNF::iterator_key kiter = (*iter)->key_set.begin(); kiter != (*iter)->key_set.end(); ++kiter)
		{
			iterator_table titer = func_table.find(*kiter);
			if (titer == func_table.end())
				titer = func_table.insert(std::pair<int, std::set<unsigned>*>(*kiter, new std::set<unsigned>())).first;
			titer->second->insert(p);
		}
}

void CNFs::print() const
{
	printf("{\n");
	for (const_iterator iter = sentences.begin(); iter != sentences.end(); ++iter)
	{
		(*iter)->print();
		printf("\n");
	}
	printf("}\n");
}

void CNFs::operator&=(const CNFs &b)
{
	sentences.reserve(sentences.size() + b.sentences.size());
	for (const_iterator citer = b.sentences.begin(); citer != b.sentences.end(); ++citer)
		sentences.push_back(new CNF(**citer));
}

void CNFs::combine(const CNFs &a, const CNFs &b)
{
	const std::vector<CNF*> &sa = a.sentences, &sb = b.sentences;
	sentences.reserve(sa.size() + sb.size());
	for (const_iterator citera = sa.begin(); citera != sa.end(); ++citera)
		for (const_iterator citerb = sb.begin(); citerb != sb.end(); ++citerb)
		{
			CNF *cnf = new CNF(**citera);
			*cnf |= **citerb;
			sentences.push_back(cnf);
		}
}
