#ifndef LAMBDASYNTAXTREE_H
#define LAMBDASYNTAXTREE_H

typedef struct term_struct *Term;

struct term_struct {
	Term father;	//father term, NULL when it is a root term
	Term childL;	//left child term, valid if term is application
	Term childR;	//right child term, valid if term not a var
	char *id;	//var name, valid if term is not an application
};

//
// so, if syntaxTree is correct, this summarizes to:
// 
// if		childL!=NULL	then	term is an application
// else if	childR!=NULL	then	term is an abstraction
// else 				term is a var
//


void LambdaSyntaxTree_FreeTerm(Term target);

void LambdaSyntaxTree_EmptyTerm(Term target);

Term LambdaSyntaxTree_DupTerm(Term target);

void LambdaSyntaxTree_ReplaceTerm(Term newT, Term oldT);



#endif

