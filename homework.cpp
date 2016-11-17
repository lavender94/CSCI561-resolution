//#define DEBUG
//#define INFO

#include <stdio.h>
#include <list>
#include "Parser.h"
#include "AST.h"
#include "CNF.h"
#include "resolution.h"

#ifdef DEBUG
#define DBG(X) X
#define INF(X) X
#else
#define DBG(X)
#ifdef INFO
#define INF(X) X
#else
#define INF(X)
#endif // INFO
#endif // DEBUG

#define BUFFERSIZE 2*1024*1024

using namespace std;

int read_input(list<CNF*> &query, CNFs *kb)
{
	unsigned n_query, n_kb;
	char *buffer = new char[BUFFERSIZE];
	Parser p(BUFFERSIZE);

	FILE *fp = fopen("input.txt", "r");
	if (!fp)
	{
		printf("Cannot open input.txt\n");
		return 1;
	}
	fscanf(fp, "%u", &n_query);
	while (n_query--)
	{
		buffer[0] = '\0';
		fscanf(fp, "%[^\n]", buffer);
		fscanf(fp, "%*c");
		if (strlen(buffer) == 0)
		{
			n_query += 1;
			continue;
		}
		p.feed(buffer);
		Expression *AST = buildAST(p);
		AST->negation(); // refutation
		AST->simplify();
		CNFs *cnfs = AST->cnfs();
		delete AST;
		CNF *cnf = new CNF(*(cnfs->sentences.begin()));
		delete cnfs;
		cnf->index();
		query.push_back(cnf);
	}
	fscanf(fp, "%u", &n_kb);
	while (n_kb--)
	{
		buffer[0] = '\0';
		fscanf(fp, "%[^\n]", buffer);
		fscanf(fp, "%*c");
		if (strlen(buffer) == 0)
		{
			n_kb += 1;
			continue;
		}
		p.feed(buffer);
		Expression *AST = buildAST(p);
		AST->simplify();
		CNFs *cnfs = AST->cnfs();
		delete AST;
		*kb &= *cnfs;
		delete cnfs;
	}
	fclose(fp);
	delete buffer;
	kb->erase_contradiction();
	kb->reindex_variable();
	kb->index();
	return 0;
}

int main()
{
	list<CNF*> query;
	CNFs *kb = new CNFs();
	int ans = read_input(query, kb);
	if (ans != 0)
		return ans;
	INF(printf("KB:\n"));
	INF(kb->print());
	INF(printf("Query:\n"));
	FILE *fp = fopen("output.txt", "w");
	if (!fp)
	{
		printf("Unable to create output.txt\n");
		for (list<CNF*>::iterator iter = query.begin(); iter != query.end(); ++iter)
			delete *iter;
		delete kb;
		return 2;
	}
	for (list<CNF*>::iterator iter = query.begin(); iter != query.end(); ++iter)
	{
		INF((*iter)->print());
		INF(printf(":"));
		CNFs history;
		if (resolution(**iter, *kb, history))
		{
			fprintf(fp, "TRUE\n");
			INF(printf("TRUE\n"));
		}
		else
		{
			fprintf(fp, "FALSE\n");
			INF(printf("FALSE\n"));
		}
		delete *iter;
	}
	fclose(fp);
	delete kb;
	return 0;
}
