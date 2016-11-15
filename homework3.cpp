#include <stdio.h>
#include "Parser.h"
#include "AST.h"
#include "CNF.h"

using namespace std;

int main()
{
	char sentence[] = "(((A(x) & B e l(x)) & (Charl( y) | (~Fe n(M n))) ) = > (Del ta(J hon) & E(Mei, z)))";
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
	cnfs->print();
	return 0;
}
