#pragma once

#include <set>
#include <list>
#include "CNF.h"

void map_variable(int from, int to, 
	int dicta[], unsigned n_v_a, int dictb[], unsigned n_v_b)
{
	for (int i = 0; i < n_v_a; ++i)
		if (dicta[i] == from)
			dicta[i] = to;
	for (int i = 0; i < n_v_b; ++i)
		if (dictb[i] == from)
			dictb[i] = to;
}

// the dict size is at least to be n_variables of individual cnf
// return false if cannot be unified
//        true if can be unified with dicta and dictb
bool unify(const ARG_LIST &a, const ARG_LIST &b,
	int dicta[], unsigned n_v_a, int dictb[], unsigned n_v_b)
{
	const std::list<int> &arga = a.args, &argb = b.args;
	if (arga.size() == argb.size())
	{
		for (int i = 0; i < n_v_a; ++i)
			dicta[i] = ~i;
		for (int i = 0; i < n_v_b; ++i)
			dictb[i] = ~(i + n_v_a);
		for (ARG_LIST::iterator ita = arga.begin(), itb = argb.begin(); ita != ita.end(); ++ita, ++itb)
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

bool refutation(CNF &cnf)
{
	return false;
}
