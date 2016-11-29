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

	bool reindex_variable();
	void index() const; // index predicates

	void replace(const int *dict);
	bool erase_contradiction(); // modified to only find contradiction
	bool factor();

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
	bool _factor(int *dict);
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
		clauses[citer->first] = new std::set<ARG_LIST>(*(citer->second));
}

CNF::~CNF()
{
	for (iterator iter = clauses.begin(); iter != clauses.end(); ++iter)
		delete iter->second;
	clauses.clear();
}

bool CNF::reindex_variable()
{
//	print();
	bool act = false;
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
		if (backup.size() > 0)
		{
			l_args->insert(backup.begin(), backup.end());
			act = true;
		}
	}
	n_variables = reindex.size();
//	printf("  %d\n", n_variables);
	return act;
}

void CNF::index() const
{
	key_set.clear();
	for (const_iterator iter = clauses.begin(); iter != clauses.end(); ++iter)
		key_set.insert(iter->first);
}

void CNF::replace(const int *dict)
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

bool CNF::erase_contradiction()
{
	bool erased = false;
	iterator iter = clauses.begin();
	while (iter != clauses.end() && iter->first < 0)
	{
		iterator _iter = clauses.find(~(iter->first));
		if (_iter == clauses.end())
		{
			++iter;
			continue;
		}
		std::set<ARG_LIST> *al = iter->second, *bl = _iter->second;
		std::set<ARG_LIST>::iterator ai = al->begin(), bi = bl->begin();
		while (ai != al->end() && bi != bl->end())
		{
			if (*ai == *bi)
			{
				return true;
				/*
				std::set<ARG_LIST>::iterator _ai = ai++, _bi = bi++;
				al->erase(_ai);
				bl->erase(_bi);
				erased = true;
				*/
			}
			else if (*ai < *bi) ++ai;
			else ++bi;
		}
		if (bl->empty())
		{
			delete bl;
			clauses.erase(_iter);
		}
		_iter = iter++;
		if (al->empty())
		{
			delete al;
			clauses.erase(_iter);
		}
	}
	return erased;
}

bool CNF::factor()
{
	bool factored = false;
	int *dict = new int[n_variables];
	while (_factor(dict))
	{
		factored = true;
		int _n_variables = n_variables;
		replace(dict);
	}
	delete[] dict;
	reindex_variable();
	return factored;
}

void CNF::print() const
{
	if (clauses.empty())
	{
		printf("FALSE");
		return;
	}
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

inline bool CNF::operator<(const CNF &b) const
{
	const std::map<int, std::set<ARG_LIST>*> &bclauses = b.clauses;
	if (clauses.size() < bclauses.size())
		return true;
	if (clauses.size() > bclauses.size())
		return false;
	for (const_iterator ita = clauses.begin(), itb = bclauses.begin(); ita != clauses.end(); ++ita, ++itb)
	{
		if (ita->first < itb->first)
			return true;
		if (ita->first > itb->first)
			return false;
		const std::set<ARG_LIST> *sa = ita->second, *sb = itb->second;
		if (sa->size() < sb->size())
			return true;
		if (sa->size() > sb->size())
			return false;
		for (std::set<ARG_LIST>::iterator sia = sa->begin(), sib = sb->begin(); sia != sa->end(); ++sia, ++sib)
		{
			if (*sia == *sib)
				continue;
			if (*sia < *sib)
				return true;
			return false;
		}
	}
	return false;
}

static void map_variable(int from, int to, int *dict, unsigned n)
{
	for (int i = 0; i < n; ++i)
		if (dict[i] == from)
			dict[i] = to;
}

bool CNF::_factor(int *dict)
{
	for (iterator func = clauses.begin(); func != clauses.end(); ++func)
	{
		std::set<ARG_LIST>* arg_list = func->second;
		for (std::set<ARG_LIST>::iterator al_a = arg_list->begin(); al_a != arg_list->end(); ++al_a)
		{
			std::set<ARG_LIST>::iterator al_b = al_a;
			++al_b;
			for (; al_b != arg_list->end(); ++al_b)
			{
				//init dict
				for (int i=0; i<n_variables; ++i)
					dict[i] = ~i;
				//unify
				bool success = true;
				for (ARG_LIST::const_iterator ita = al_a->args.begin(), itb = al_b->args.begin(); ita != al_a->args.end(); ++ita, ++itb)
				{
					int ida = *ita, idb = *itb;
					if (*ita < 0) // original variable
						ida = dict[~ida];
					if (*itb < 0) // original variable
						idb = dict[~idb];
					if (ida < 0) // variable a
						if (idb < 0) // variable b
							map_variable(idb, ida, dict, n_variables);
						else // constant b
							map_variable(ida, idb, dict, n_variables);
					else // constant a
						if (idb < 0) // variable b
							map_variable(idb, ida, dict, n_variables);
						else // constant b
							if (ida != idb)
							{
								success = false;
								break; // unification failed
							}
				}
				if (success)
					return true;
			}
		}
	}
	return false;
}

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

CNFs::CNFs(const CNFs &b)
{
	sentences.clear();
	sentences.insert(b.sentences.begin(), b.sentences.end());
}

CNFs::~CNFs()
{
	for (iterator_table iter = func_table.begin(); iter != func_table.end(); ++iter)
		delete iter->second;
	func_table.clear();
}

void CNFs::reindex_variable()
{
	std::list<CNF> backup(sentences.begin(), sentences.end());
	sentences.clear();
	for (std::list<CNF>::iterator iter = backup.begin(); iter != backup.end(); ++iter)
	{
		iter->reindex_variable();
		sentences.insert(*iter);
	}
}

void CNFs::index()
{
	for (const_iterator iter = sentences.begin(); iter != sentences.end(); ++iter)
		iter->index();
	for (iterator_table iter = func_table.begin(); iter != func_table.end(); ++iter)
		delete iter->second;
	func_table.clear();
	for (iterator iter = sentences.begin(); iter != sentences.end(); ++iter)
		for (CNF::iterator_key kiter = iter->key_set.begin(); kiter != iter->key_set.end(); ++kiter)
		{
			iterator_table titer = func_table.find(*kiter);
			if (titer == func_table.end())
				titer = func_table.insert(std::pair<int, std::list<iterator>*>(*kiter, new std::list<iterator>())).first;
			titer->second->push_back(iter);
		}
}

void CNFs::erase_contradiction()
{
	const_iterator iter = sentences.begin();
//	std::list<CNF> backup;
	while (iter != sentences.end())
	{
		CNF cnf(*iter);
		if (cnf.erase_contradiction())
		{
//			backup.push_back(cnf);
			const_iterator _iter = iter;
			++iter;
			sentences.erase(_iter);
		}
		else
			++iter;
	}
//	sentences.insert(backup.begin(), backup.end());
}

void CNFs::factor()
{
	const_iterator iter = sentences.begin();
	std::list<CNF> backup;
	while (iter != sentences.end())
	{
		CNF cnf(*iter);
		if (cnf.factor())
		{
			if (!cnf.erase_contradiction())
			//if (!cnf.clauses.empty())
				backup.push_back(cnf);
		}
		++iter;
	}
	sentences.insert(backup.begin(), backup.end());
}

void CNFs::print() const
{
	printf("{\n");
	for (const_iterator iter = sentences.begin(); iter != sentences.end(); ++iter)
	{
		iter->print();
		printf("\n");
	}
	printf("}\n");
}

void CNFs::operator&=(const CNFs &b)
{
	sentences.insert(b.sentences.begin(), b.sentences.end());
}

void CNFs::combine(const CNFs &a, const CNFs &b)
{
	const std::set<CNF> &sa = a.sentences, &sb = b.sentences;
	for (const_iterator citera = sa.begin(); citera != sa.end(); ++citera)
		for (const_iterator citerb = sb.begin(); citerb != sb.end(); ++citerb)
		{
			CNF cnf(*citera);
			cnf |= *citerb;
			sentences.insert(cnf);
		}
}
