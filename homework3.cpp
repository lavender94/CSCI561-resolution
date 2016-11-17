#include <stdio.h>
#include <list>
#include "Parser.h"
#include "AST.h"
#include "CNF.h"
#include "resolution.h"

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
	/*char sentence[] = "~(((A(x) & B e l(x)) & (Charl( y) | (~Fe n(M n))) ) = > (Del ta(J hon) & E(Mei, z)))";
	Parser p(strlen(sentence) + 5);
	p.feed(sentence);
	Expression *AST = buildAST(p);
	AST->print();
	printf("\n");
	AST->simplify();
	AST->print();
	printf("\n");
	CNFs *cnfs = AST->cnfs();
	cnfs->print();
	cnfs->reindex_variable();
	cnfs->print();*/
	printf("KB:\n");
	kb->print();
	printf("Query:\n");
	for (list<CNF*>::iterator iter = query.begin(); iter != query.end(); ++iter)
	{
		(*iter)->print();
		printf(":");
		CNFs history;
		if (resolution(**iter, *kb, history))
			printf("TRUE\n");
		else
			printf("FALSE\n");
		delete *iter;
	}
	delete kb;
	return 0;
}
