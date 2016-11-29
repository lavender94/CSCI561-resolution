#pragma once

#include <set>
#include <list>
#include <stdio.h>
#include "CNF.h"

#ifdef DEBUG
#define RESOLUTION_DBG(X) X
#define RESOLUTION_INF(X) X
#else
#define RESOLUTION_DBG(X)
#ifdef INFO
#define RESOLUTION_INF(X) X
#else
#define RESOLUTION_INF(X)
#endif // INFO
#endif // DEBUG

void map_variable(int from, int to, 
	int *dicta, unsigned n_v_a, int *dictb, unsigned n_v_b)
{
	for (int i = 0; i < n_v_a; ++i)
		if (dicta[i] == from)
			dicta[i] = to;
	for (int i = 0; i < n_v_b; ++i)
		if (dictb[i] == from)
			dictb[i] = to;
}

// the dict size is at least to be n_variables of individual cnf
// dicta[n_v_a], dictb[n_v_b]
// return false if cannot be unified
//        true if can be unified with dicta and dictb
bool unify(const ARG_LIST &a, const ARG_LIST &b,
	int *dicta, unsigned n_v_a, int *dictb, unsigned n_v_b)
{
	const std::list<int> &arga = a.args, &argb = b.args;
	if (arga.size() == argb.size())
	{
		for (int i = 0; i < n_v_a; ++i)
			dicta[i] = ~i;
		for (int i = 0; i < n_v_b; ++i)
			dictb[i] = ~(i + n_v_a);
		for (ARG_LIST::const_iterator ita = arga.begin(), itb = argb.begin(); ita != arga.end(); ++ita, ++itb)
		{
			int ida = *ita, idb = *itb;
			if (*ita < 0) // original variable
				ida = dicta[~ida];
			if (*itb < 0) // original variable
				idb = dictb[~idb];
			if (ida < 0) // variable a
				if (idb < 0) // variable b
					map_variable(idb, ida,
						dicta, n_v_a, dictb, n_v_b);
				else // constant b
					map_variable(ida, idb,
						dicta, n_v_a, dictb, n_v_b);
			else // constant a
				if (idb < 0) // variable b
					map_variable(idb, ida,
						dicta, n_v_a, dictb, n_v_b);
				else // constant b
					if (ida != idb)
						return false;
		}
		return true;
	}
	return false;
}

bool resolution(CNF &query, const CNFs &kb, CNFs &history)
{
	if (query.clauses.empty())
		return true;
	CNF::iterator_key key = query.key_set.begin();
	CNFs::const_iterator_table entry = kb.func_table.find(~(*key));
	if (entry == kb.func_table.end())
		return false;
	const std::list<CNFs::iterator> *sub_kb = entry->second;
	std::set<ARG_LIST>::const_iterator q_arg = query.clauses[*key]->begin();
	int n_v_q = query.n_variables;
	int *dictq = new int[n_v_q];
	for (std::list<CNFs::iterator>::const_iterator cnf = sub_kb->begin(); cnf != sub_kb->end(); ++cnf)
	{
		const std::set<ARG_LIST> *cnf_al = (*cnf)->clauses.find(~(*key))->second;
		int n_v_s = (*cnf)->n_variables;
		int *dicts = new int[n_v_s];
		for (std::set<ARG_LIST>::const_iterator cnf_arg = cnf_al->begin(); cnf_arg != cnf_al->end(); ++cnf_arg)
			if (unify(*q_arg, *cnf_arg, dictq, n_v_q, dicts, n_v_s))
			{
				CNF uni_q(query), uni_s(**cnf);
				RESOLUTION_DBG(printf("\n:"));
				RESOLUTION_DBG(uni_q.print());
				RESOLUTION_DBG(printf("\n:"));
				RESOLUTION_DBG(uni_s.print());
				uni_q.replace(dictq);
				uni_s.replace(dicts);
				// erase literals
				CNF::iterator entry_q = uni_q.clauses.find(*key);
				std::set<ARG_LIST>::iterator iter_q = entry_q->second->begin();
				CNF::iterator entry_s = uni_s.clauses.find(~(*key));
				std::set<ARG_LIST> *set_arg_s = entry_s->second;
				for (std::set<ARG_LIST>::iterator iter_s = set_arg_s->begin(); iter_s != set_arg_s->end(); ++iter_s)
					if (*iter_q == *iter_s)
					{
						set_arg_s->erase(iter_s);
						if (set_arg_s->empty())
						{
							delete set_arg_s;
							uni_s.clauses.erase(entry_s);
						}
						break;
					}
				entry_q->second->erase(iter_q);
				if (entry_q->second->empty())
				{
					delete entry_q->second;
					uni_q.clauses.erase(entry_q);
				}
				uni_q |= uni_s;
				if (uni_q.erase_contradiction())
					continue;
				uni_q.reindex_variable();
				uni_q.factor();
				RESOLUTION_DBG(printf("\n=>"));
				RESOLUTION_DBG(uni_q.print());
				RESOLUTION_DBG(printf("\n"));
				std::pair<CNFs::iterator, bool> ret = history.sentences.insert(uni_q);
				if (ret.second == false)
					continue;
				uni_q.index();
				if (resolution(uni_q, kb, history))
				{
					delete[] dictq;
					delete[] dicts;
					return true;
				}
			}
		delete[] dicts;
	}
	delete[] dictq;
	return false;
}
