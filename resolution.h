#pragma once

#include "common.h"

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
	int *dicta, unsigned n_v_a, int *dictb, unsigned n_v_b);

// the dict size is at least to be n_variables of individual cnf
// dicta[n_v_a], dictb[n_v_b]
// return false if cannot be unified
//        true if can be unified with dicta and dictb
bool unify(const ARG_LIST &a, const ARG_LIST &b,
	int *dicta, unsigned n_v_a, int *dictb, unsigned n_v_b);

bool resolution(CNF &query, const CNFs &kb, CNFs &history);
