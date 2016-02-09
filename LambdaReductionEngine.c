#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "LambdaSyntaxTree.h"
#include "LambdaReductionEngine.h"

static int count = 1;

static Term getNewVarTerm();
static int isOper(Term var);
static int isNumber(Term var);

// newTerm will not be part of the term, it will be DupTerm()'ed
// occurences of var will be freed, better give a copy of the var...
// ...because if you try to give a var which is part of tree, boom!
// var will not be attached to the tree (of course...)
// father of the newTerm will be set to father of replaced var
void LambdaReductionEngine_Substitution(Term target, char *var, Term newTerm) {
	Term midTerm;
	char *tmp;

	if (target==NULL || var==NULL) {	//for safety
		return;
	}


	// the passed Term is consindered to be correctly setup

	if (target->childL!=NULL) {		// application
		LambdaReductionEngine_Substitution(target->childL, var, newTerm);
		LambdaReductionEngine_Substitution(target->childR, var, newTerm);
	}
	else if (target->childR!=NULL) {	// abstraction
		if (strcmp(var, target->id)!=0) {	//var not in \x
			if (
(LambdaReductionEngine_IsFreeVariable(newTerm, target->id)==0) ||
(LambdaReductionEngine_IsFreeVariable(target->childR, var)==0)
			   ) {
				LambdaReductionEngine_Substitution(target->childR, var, newTerm);
			}
			else {
				midTerm = getNewVarTerm();
				tmp = target->id;
				target->id = midTerm->id;
				LambdaReductionEngine_Substitution(target->childR, tmp, midTerm);
				LambdaReductionEngine_Substitution(target->childR, var, newTerm);
				free(tmp);
				free(midTerm);
			}

		}
	}
	else {					// var
		if (strcmp(var, target->id)==0) {
			//replace "target" content with "newTerm" content
			midTerm = LambdaSyntaxTree_DupTerm(newTerm);
			LambdaSyntaxTree_ReplaceTerm(midTerm, target);
			free(midTerm);
		}
	}


}



// all reductions are performed without checking if reduction applicable
void LambdaReductionEngine_BetaReduction(Term target) {

	if (target==NULL)	//safety check
		return;
	LambdaReductionEngine_Substitution(target->childL->childR, target->childL->id, target->childR);
	LambdaSyntaxTree_ReplaceTerm(target->childL->childR, target);
}


void LambdaReductionEngine_EtaReduction(Term target) {

	if (target==NULL)	//safety check
		return;
	LambdaSyntaxTree_ReplaceTerm(target->childR->childL, target);
}


void LambdaReductionEngine_ArithmeticReduction(Term target) {
	int x, y, r;
	int i, len;
	char *tmp;

	if (target==NULL)	//safety check
		return;

	x = atoi(target->childL->childR->id);
	y = atoi(target->childR->id);

	switch (target->childL->childL->id[0]) {
		case '+':
			r=x+y;
			break;
		case '-':
			r=x-y;
			break;
		case '*':
			r=x*y;
			break;
		case '/':
			r=x/y;
			break;
		case '%':
			r=x%y;
			break;
	}

	//free memory
	LambdaSyntaxTree_EmptyTerm(target);

	//create target->id string
	if (r<0)
		len = 3;
	else
		len = 2;
	for (i=r; i>=10 || i<=-10; i/=10) //calculate decimal digits number
		len++;
	tmp=(char *)malloc(len*sizeof(char));
	sprintf(tmp, "%d", r);
	tmp[len-1]='\0';	//for safety

	//set target->id to created number-string
	target->id = tmp;
}


void LambdaReductionEngine_AlphaReduction(Term target) {
}


Term LambdaReductionEngine_GetNextRedexTerm(Term target) {
	Term tmp;

	// check for outermost redex => higher in the tree
	if (LambdaReductionEngine_GetRedexType(target)!=0)
		return target;
	// check for leftmost redex => left in the tree
	if (
	  target->childL!=NULL &&
	  (tmp=LambdaReductionEngine_GetNextRedexTerm(target->childL))!=NULL
	)
		return tmp;
	// check right
	if (
	  target->childR!=NULL &&
	  (tmp=LambdaReductionEngine_GetNextRedexTerm(target->childR))!=NULL
	)
		return tmp;

	return NULL;
}


int LambdaReductionEngine_GetRedexType(Term target) {
	if (target==NULL) {	//not only for safety, used also
		return 0;
	}
	else if (target->childL!=NULL) {	// application
		//check for beta-redex
		if (
		  target->childL->childL==NULL &&	//confirm left L 1
		  target->childL->childR!=NULL		//confirm left L 2
		) {
			return 1;	//beta-redex
		}

		//check for arithmetic-redex
		if (
		  isNumber(target->childR) &&		//confirm right num
		  target->childL->childL!=NULL &&	//confirm left appl
		  isNumber(target->childL->childR) &&	//confirm right num
		  isOper(target->childL->childL)	//confirm right num
		) {
			return 3;	//arithmetic-redex
		}
	}
	else if (target->childR!=NULL) {	// abstraction
		//check for eta-redex
		if (
		  target->childR->childL!=NULL &&	//confirm () below
			//2steps to confirm right child of appl is var//
		  target->childR->childR->childL==NULL &&
		  target->childR->childR->childR==NULL &&
			//confirm matching variables in var and L//
		  strcmp(target->childR->childR->id, target->id)==0 &&
			//confirm constrain: var NOT IN FV(M)//
		  LambdaReductionEngine_IsFreeVariable(target->childR->childL, target->id)==0
		) {
			return 2;	//eta-redex
		}
	}
	else {					// var
		//no redex in a variable :)
		return 0;
	}

	return 0;
}


int LambdaReductionEngine_Normalize(Term target) {
	int count=0;

	if (target==NULL)
		return -1;

	while (LambdaReductionEngine_NormalizeStep(target)!=0)
		count++;
	return count;
}


int LambdaReductionEngine_LimitedNormalize(Term target, int redexLimit) {
	int count=0;

	if (target==NULL)
		return -1;

	while (
	  count<redexLimit &&
	  LambdaReductionEngine_NormalizeStep(target)!=0
	  )
		count++;

	return count;
}


int LambdaReductionEngine_NormalizeStep(Term target) {
	Term tmp;
	int t;

	tmp = LambdaReductionEngine_GetNextRedexTerm(target);
	t = LambdaReductionEngine_GetRedexType(tmp);

	switch (t) {
		case 1:		//beta reduction
			LambdaReductionEngine_BetaReduction(tmp);
			break;
		case 2:		//eta reduction
			LambdaReductionEngine_EtaReduction(tmp);
			break;
		case 3:		//arithmetic reduction
			LambdaReductionEngine_ArithmeticReduction(tmp);
			break;
	}

	return t;
}


void LambdaReductionEngine_ResetVarGenerator() {
	count=1;
}


//var->father will not be set
static Term getNewVarTerm() {
	int i, len=3;	//overhead for \0 and 's'
	Term t;
	char *tmp;

	//create var->id string
	for (i=count; i>=10; i/=10)	//calculate decimal digits number
		len++;
	tmp=(char *)malloc(len*sizeof(char));
	tmp[0]='s';
//***	snprintf(tmp+1, len-1, "%d", count++);
	sprintf(tmp+1, "%d", count++);
	tmp[len-1]='\0';	//for safety

	//create new term to be returned
	t = (Term)malloc(sizeof(struct term_struct));
	t->childR = t->childL = NULL;
	t->id=tmp;

	return t;
}


int LambdaReductionEngine_IsFreeVariable(Term target, char *var) {
	if (target==NULL || var==NULL)	//for safety...
		return -1;	//true, might be useful for recursion

	// the passed Term is consindered to be correctly setup

	if (target->childL!=NULL) {		// application
		return 
		  LambdaReductionEngine_IsFreeVariable(target->childL, var) ||
		  LambdaReductionEngine_IsFreeVariable(target->childR, var);
	}
	else if (target->childR!=NULL) {	// abstraction
		return
		  (strcmp(target->id, var)!=0) &&
		  LambdaReductionEngine_IsFreeVariable(target->childR, var);
	}
	else {					// var
		return (strcmp(target->id, var)==0);
	}
}


static int isOper(Term var) {
	return (
	 var!=NULL &&	//safety

	 //2 checks to confirm var
	 var->childL==NULL &&
	 var->childR==NULL &&

	 //2 checks to confirm var is an oper
	 var->id!=NULL &&	//safety

	 (
	  var->id[0]=='+' ||
	  (var->id[0]=='-' && !isdigit((int)var->id[1]))  ||
	  var->id[0]=='*' ||
	  var->id[0]=='/' ||
	  var->id[0]=='%'
	 )
	);
}


static int isNumber(Term var) {

	return (
	 var!=NULL &&	//safety

	 //2 checks to confirm var
	 var->childL==NULL &&
	 var->childR==NULL &&

	 //2 checks to confirm var is number
	 var->id!=NULL &&       //safety
	 (
	  isdigit((int)var->id[0]) ||   //is positive
	  (
	   var->id[0]=='-' &&      //is negative
	   isdigit((int)var->id[1])
	  )
	 )
	);
}

