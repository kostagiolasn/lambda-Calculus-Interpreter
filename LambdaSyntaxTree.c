#include <stdlib.h>
#include <string.h>
#include "LambdaSyntaxTree.h"

void LambdaSyntaxTree_FreeTerm(Term target) {
	if (target==NULL)	//not only for safety, used in the recursion
		return;
	LambdaSyntaxTree_FreeTerm(target->childL);
	LambdaSyntaxTree_FreeTerm(target->childR);
	free(target->id);
	free(target);
}

void LambdaSyntaxTree_EmptyTerm(Term target) {
	if (target==NULL)	//not only for safety, used in the recursion
		return;
	LambdaSyntaxTree_FreeTerm(target->childL);
	target->childL = NULL;
	LambdaSyntaxTree_FreeTerm(target->childR);
	target->childR = NULL;
	free(target->id);
	target->id = NULL;
}


//WARNING: its caller must fix the father pointer
Term LambdaSyntaxTree_DupTerm(Term target) {
	Term tmp;

	if (target==NULL)	//only for safety, not used in recursion
		return NULL;
	tmp=(Term)malloc(sizeof(struct term_struct));

	//fix left child
	if (target->childL!=NULL) {
		tmp->childL = LambdaSyntaxTree_DupTerm(target->childL);
		tmp->childL->father = tmp;
	}
	else
		tmp->childL = NULL;

	//fix right child
	if (target->childR!=NULL) {
		tmp->childR = LambdaSyntaxTree_DupTerm(target->childR);
		tmp->childR->father = tmp;
	}
	else
		tmp->childR = NULL;

	//fix variable ID
	if (target->id!=NULL)
		tmp->id = strdup(target->id);	//duplicate var name
	else
		tmp->id = NULL;

	return tmp;	//return duplicated term (with father NOT fixed)
}


//WARNING: oldT contents will be freed
//WARNING: doesn't copy newT->father pointer
//WARNING: doesn't update oldT->father->childX pointer
//newT CAN be in subtree of oldT
//after operation, newT will have no contents (all NULLs)
//WARNING: newT is NOT freed up !!! Caller might need to free it
void LambdaSyntaxTree_ReplaceTerm(Term newT, Term oldT) {
	Term t1, t2;

	if (oldT==NULL || newT==NULL)
		return;

	//keep old subtree to free it up later
	t1 = oldT->childR;
	t2 = oldT->childL;
	free(oldT->id);		//free the var-id now

	//copy content
	oldT->id = newT->id;
	oldT->childR = newT->childR;
	oldT->childL = newT->childL;

	//update childX->father pointers for children of newT
	if (oldT->childR!=NULL)
		oldT->childR->father = oldT;
	if (oldT->childL!=NULL)
		oldT->childL->father = oldT;

	//set content of newT to NULLs
	//this protects the subtree of newT, in case newT is in subtree of oldT
	newT->id = NULL;
	newT->childR = NULL;
	newT->childL = NULL;

	//free the old subtree
	LambdaSyntaxTree_FreeTerm(t1);
	LambdaSyntaxTree_FreeTerm(t2);
}
