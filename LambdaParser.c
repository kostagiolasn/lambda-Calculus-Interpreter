#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "LambdaSyntaxTree.h"
#include "LambdaParser.h"
#include "MyUtils.h"


static int Get1stUnmatchedRightParenthesis(int first, int last);
static int Get1stUnmatchedLeftParenthesis(int first, int last);
static int Get1stUnmatchedParenthesis(int first, int last);
static int GetLastNonBlank(int first, int last);
static int Get1stNonBlank(int first, int last);
static Term internal_ParseAbstraction(int first, int last);
static Term internal_ParseApplication(int first, int last);
static Term internal_ParseNumber(int first, int last);
static Term internal_ParseOper(int first, int last);
static Term internal_ParseTerm(int first, int last);

static char *target;
static int errorCode;
static int errorByte;

int not_parenthesized;

Term LambdaParser_Parse(char *input) {
	Term tmp;
	int len;

	errorCode=0;
	errorByte=0;
	if (input==NULL) {
		errorCode=1;	//NULL string parameter
		errorByte = 0;
		return NULL;
	}
	target=input;
	len = strlen(target)-1;
	/*if (Get1stUnmatchedParenthesis(0, len)<=len) {
		errorCode=22;	//unmatched parenthesis
		errorByte = Get1stUnmatchedParenthesis(0, len);
		return NULL;
	}*/

	tmp = internal_ParseTerm(0, len);

	//fix tmp->father to show this term is the root of the syntax tree
	if (tmp!=NULL)
		tmp->father=NULL;
	
	return tmp;
}


static Term internal_ParseTerm(int first, int last) {
	int i, x;

	i=Get1stNonBlank(first, last);
	if (i>last) {
		errorCode=2;	//Term expected, nothing found
		errorByte=last;
		return NULL;
	}
// maybe rewrite next check as switch
	switch (target[i]) {
		case '(':
			x = Get1stNonBlank(i+1, last);
			if (x>last) {
				errorCode=3;	//Term is only a single '('
				errorByte=i;
				return NULL;
			}
			if (target[x]=='\\')
			{
				//printf("First: %d, Last: %d", i, last);
				//printf("Abstraction\n");
				return internal_ParseAbstraction(i, last);
			}
			else
			{
				//printf("First: %d, Last: %d", i, last);
				//printf("Application\n");
				return internal_ParseApplication(i, last);
			}
		case '\\':
			//printf("Abstraction\n");
			x = Get1stNonBlank(i+1, last);
			//printf("Target[x] %c\n", target[x]);
			return internal_ParseAbstraction(i, last);
		case '-':
			if (i+1<=last && isdigit((int)target[i+1]))
				return internal_ParseNumber(i, last);
			else
				return internal_ParseOper(i, last);
		case '*':
		case '/':
		case '+':
		case '%':
			return internal_ParseOper(i, last);
		default:
			if (isdigit((int)target[i]))
				return internal_ParseNumber(i, last);
			else
			{
				//printf("First: %d, Last: %d", i, last);
				//printf("Variable\n");
				return internal_ParseVar(i, last);
			}
	}
}

Term internal_ParseVar(int first, int last) {
	Term tmp;
	char *tmpID;
	int i, x, t;

	i=Get1stNonBlank(first, last);
	if (i>last) {
		errorCode=4;	//Var expected, nothing found
		errorByte=last;
		return NULL;
	}
	if (!isalpha((int)target[i])) {
		if (isdigit((int)target[i])) {
			errorCode=5;	//Var name cannot start with digit
			errorByte=i;
			return NULL;
		}
		errorCode=6;	//Var expected, non-alpha char found
		errorByte=i;
		return NULL;
	}

	//find index after last char of var (stored in x)
	for (x=i+1; x<=last &&
	 (isalpha((int)target[x]) || isdigit((int)target[x])); x++) {}

	//check to see if anything else follows the var
	t=Get1stNonBlank(x,last);
	if (t<=last && !not_parenthesized) {
		errorCode=7;	//Nothing expected, but chars found
		errorByte=t;
		return NULL;
	}

	//create var name in tmpID
	tmpID = (char *)malloc((x-i+1)*sizeof(char));
	for (t=0; t<(x-i); t++)
		tmpID[t]=target[t+i];
	tmpID[t]='\0';

	//create Term
	tmp = (Term)malloc(sizeof(struct term_struct));
	tmp->childL = NULL;
	tmp->childR = NULL;
	tmp->id=tmpID;
	//  tmp->father should be set by caller

	return tmp;
}


static Term internal_ParseNumber(int first, int last) {
	Term tmp;
	char *tmpID;
	int i, x, t;
	int negative;	//=2 if negative, =1 if positive or zero

	i=Get1stNonBlank(first, last);
	if (i>last) {
		errorCode=23;	//Number expected, nothing found
		errorByte=last;
		return NULL;
	}
	if (!isdigit((int)target[i])) {
		if (target[i]!='-') {
			errorCode=24;	//Number doesn't start with digit or -
			errorByte=i;
			return NULL;
		}
		if (i+1>last || !isdigit((int)target[i+1])) {
			errorCode=25;	//In number, not digit after "-"
			errorByte=i+1;
			return NULL;
		}
		negative = 2;
	}
	else
		negative = 1;

	//find index after last digit of number (stored in x)
	for (x=i+negative; x<=last && isdigit((int)target[x]); x++) {
	}

	//check to see if anything else follows the number
	t=Get1stNonBlank(x,last);
	if (t<=last) {
		errorCode=7;	//Nothing expected, but chars found
		errorByte=t;
		return NULL;
	}

	//create number string in tmpID
	tmpID = (char *)malloc((x-i+1)*sizeof(char));
	for (t=0; t<(x-i); t++)
		tmpID[t]=target[t+i];
	tmpID[t]='\0';

	//create Term
	tmp = (Term)malloc(sizeof(struct term_struct));
	tmp->childL = NULL;
	tmp->childR = NULL;
	tmp->id=tmpID;
	//  tmp->father should be set by caller

	return tmp;
}


static Term internal_ParseOper(int first, int last) {
	Term tmp;
	char *tmpID;
	int i, x, t;

	i=Get1stNonBlank(first, last);
	if (i>last) {
		errorCode=26;	//Oper expected, nothing found
		errorByte=last;
		return NULL;
	}

	t=target[i];
	if (t!='+' && t!='-' && t!='*' && t!='/' && t!='%') {
		errorCode=27;	//Oper expected, non-oper char found
		errorByte=i;
		return NULL;
	}

	//set index after char of oper (stored in x)
	x=i+1;

	//check to see if anything else follows the oper
	t=Get1stNonBlank(x,last);
	if (t<=last) {
		errorCode=7;	//Nothing expected, but chars found
		errorByte=t;
		return NULL;
	}

	//create oper string in tmpID
	tmpID = (char *)malloc(2*sizeof(char));
	tmpID[0]=target[i];
	tmpID[1]='\0';

	//create Term
	tmp = (Term)malloc(sizeof(struct term_struct));
	tmp->childL = NULL;
	tmp->childR = NULL;
	tmp->id=tmpID;
	//  tmp->father should be set by caller

	return tmp;
}


static Term internal_ParseApplication(int first, int last) {
	Term tmp;
	int i, x, y, z;

	i=Get1stNonBlank(first, last);
	if (i>last) {
		errorCode=8;	//Application expected, nothing found
		errorByte=last;
		return NULL;
	}

	if (target[i]!='(') {
		errorCode=8;	//Application expected, but no '(' found
		errorByte=i;
		return NULL;
	}


	x=GetAfterEndOfTerm(i+1, last);
	if (x>last) {
		errorCode=9;	//Application has none or only one term
		errorByte=last;
		return NULL;
	}

	y=GetAfterEndOfTerm(x, last);
	if (y>last) {
		errorCode=10;	//Application has only one term
		errorByte=last;
		return NULL;
	}

	z=Get1stNonBlank(y, last);
	if (z>last || target[z]!=')') {
		errorCode=11;	//Application has no closing ')'
		errorByte=y;
		return NULL;
	}

	//check if there is anything following...
	z=Get1stNonBlank(z+1, last);
	if (z<=last) {
		errorCode=12;	//Chars found after application
		errorByte=z;
		return NULL;
	}


	//everything is ok here


	//create Term
	tmp = (Term)malloc(sizeof(struct term_struct));
	tmp->childL = internal_ParseTerm(i+1, x-1);
	tmp->childR = internal_ParseTerm(x, y-1);
	tmp->id=NULL;
	//  tmp->father should be set by caller

	//check for any errors
	if (tmp->childL==NULL || tmp->childR==NULL) {
		LambdaSyntaxTree_FreeTerm(tmp);
		return NULL;
	}

	//as a caller, fix father of children
	tmp->childL->father = tmp;
	tmp->childR->father = tmp;

	return tmp;
}


static Term internal_ParseAbstraction(int first, int last) {
	Term tmp;
	int i, x, y, t, c, z;
	char *tmpID;
	int no_parenthesized = 0;
	
	//printf("First: %d, Last: %d", first, last);

	i=Get1stNonBlank(first, last);
	if (i>last) {
		errorCode=13;	//Abstraction expected, nothing found
		errorByte=last;
		return NULL;
	}
	

	if (target[i]!='(') {
		errorCode=14;	//Abstraction expected, but no '(' found
		errorByte=i;
		no_parenthesized++;
		//return NULL;
	}
	
	if(no_parenthesized == 1)
	{
		i = Get1stNonBlank(i, last);
		if (i>last || target[i]!='\\') {
		errorCode=15;	//Abstraction's '\' expected
		errorByte=i;
		return NULL;
		}
	}
	else
	{
		i=Get1stNonBlank(i+1, last);
		if (i>last || target[i]!='\\') {
			errorCode=15;	//Abstraction's '\' expected
			errorByte=i;
			return NULL;
		}
	}

	x=GetAfterEndOfTerm(i+1, last);
	//printf("Target[x]: %c\n", x);
	if (x>last) {
		errorCode=16;	//Abstraction doesn't have '->' symbol
		errorByte=i+1;
		return NULL;
	}

	c=Get1stNonBlank(x, last);
	//printf("Target[c]: %c, Target[c+1]: %c\n", target[c], target[c+1]);
	
	if (c>=last || ((target[c]!='-' || target[c+1]!='>') && target[c]!='.')) {
		errorCode=17;	//Abstraction doesn't have '->' symbol
		errorByte=c;
		return NULL;
	}

	if (target[c]=='.')
		c=Get1stNonBlank(c+1, last);
	else
		c=Get1stNonBlank(c+2, last);
		
		
	if(not_parenthesized)
	{
		if (c>last) {
		errorCode=18;	//Abstraction doesn't have a Term
		errorByte=last;
		return NULL;
		}
	}
	else
	{
		if (c>=last) {
			errorCode=18;	//Abstraction doesn't have a Term
			errorByte=last;
			return NULL;
		}
	}


	if(!not_parenthesized)
	{
		y=GetAfterEndOfTerm(c, last);
		if (y>last) {
			errorCode=19;	//Abstraction has no closing ')'
			errorByte=last;
			return NULL;
		}
		
		z=Get1stNonBlank(y, last);
		if (z>last || target[z]!=')') {
			errorCode=20;	//Abstraction has no closing ')'
			errorByte=y;
			return NULL;
		}
	}
	else
	{
		z=c;
		//no_parenthesized++;
	}


	if(!not_parenthesized)
	{
		//check if there is anything following...
		z=Get1stNonBlank(z+1, last);
		if (z<=last) {
			errorCode=21;	//Chars found after abstraction
			errorByte=z;
			return NULL;
		}
	}
	
	

	//everything is ok here

	//create var name in tmpID
	i=Get1stNonBlank(i+1, last);
	tmpID = (char *)malloc((x-i+1)*sizeof(char));
	for (t=0; t<(x-i); t++)
		tmpID[t]=target[t+i];
	tmpID[t]='\0';

	//create Term
	tmp = (Term)malloc(sizeof(struct term_struct));
	tmp->childL = NULL;
	
	//printf("Target[c] : %c, Target[y-1] : %c", target[c], target[y-1]);
	
	if(!not_parenthesized)
		tmp->childR = internal_ParseTerm(c, y-1);
	else
		tmp->childR = internal_ParseTerm(c, last);
	tmp->id=tmpID;
	//  tmp->father should be set by caller
		
	//check for any errors
	if (tmp->childR==NULL) {
		LambdaSyntaxTree_FreeTerm(tmp);
		return NULL;
	}

	//as a caller, fix father of children
	tmp->childR->father = tmp;

	return tmp;
}





















char *LambdaParser_GetErrorMessage() {
	char *errorMessages[26] = {
		"Parser is OK.",			// 0
		"NULL string passed to parser.",	// 1
		"Term expected, nothing found!",	// 2
		"Term is only a single '('.",		// 3
		"Var expected, nothing found!",		// 4
		"Var cannot start with a digit.",	// 5
		"Var expected, non-alphanumeric character found!",  // 6
		"Nothing expected, characters found!",	// 7
		"Application expected, nothing found!",	// 8
		"Application has none or only 1 term!",	// 9
		"Application has only one term!",	// 10
		"Application has no closing ')'.",	// 11
		"Characters found after application!",	// 12
		"Abstraction expected, nothing found!",	// 13
		"Abstraction expected, but no '(' found!",  // 14
		"Abstraction's lambda '\\' expected!",	// 15
		"Abstraction doesn't have '->' symbol!",  // 16
		"Abstraction doesn't have '->' symbol!",  // 17
		"Abstraction doesn't have a Term!",	// 18
		"Abstraction has no closing ')'.",	// 19
		"Abstraction has no closing ')'.",	// 20
		"Characters found after abstraction!",	// 21
		"Unmatched parenthesis!",		// 22
		"Number expected, nothing found!",	// 23
		"Number doesn't start with a digit or '-'",  //24
		"Number expected, found a lonely '-'"	// 25
	};

	return errorMessages[errorCode];
}


int LambdaParser_GetErrorCode() {
	return errorCode;
}

int LambdaParser_GetErrorByte() {
	return errorByte;
}

static int Get1stNonBlank(int first, int last) {
	int x;

	for (x=first; x<=last &&
	 (target[x]==' ' || target[x]=='	'); x++) {}
	return x;
}


static int GetLastNonBlank(int first, int last) {
	int x;

	for (x=last; x>=first &&
	 (target[x]==' ' || target[x]=='	'); x--) {}
	return x;
}


int GetAfterEndOfTerm(int first, int last) {
	int x;

	x=Get1stNonBlank(first, last);
	switch (target[x]) {
		case '(':	//appl or abstraction
			return Get1stUnmatchedRightParenthesis(x+1, last) + 1;
		case '*':
		case '+':
		case '/':
		case '%':
			//index after char of oper
			return x+1;
		case '-':
			if (x+1>last || !isdigit((int)target[x+1]))
				return x+1;	//is oper '-'
		default:
			if (target[x]=='-' || isdigit((int)target[x])) {
				//is a number
				for (x=x+1; x<=last && isdigit((int)target[x]); x++) {
				}
				return x;
			}
			//it's a var
			//find index after last char of var (stored in x)
			for (x=x+1; x<=last &&
			 (isalpha((int)target[x]) || isdigit((int)target[x])); x++) {}
			return x;
	}
	
	return x;
}


static int Get1stUnmatchedParenthesis(int first, int last) {
	int x;

	x=Get1stUnmatchedLeftParenthesis(first, last);
	if (x>=first)
		return x;
	x=Get1stUnmatchedRightParenthesis(first, last);
	return x;
}


static int Get1stUnmatchedLeftParenthesis(int first, int last) {
	int x;
	int parenthesis=0;

	//left-to-right scan
	for (x=GetLastNonBlank(first, last); x>=first; x--) {
		switch (target[x]) {
			case ')':
				parenthesis++;
				break;
			case '(':
				parenthesis--;
				if (parenthesis<0)	//unmatched '('
					return x;
		}
	}

	return x;
}

static int Get1stUnmatchedRightParenthesis(int first, int last) {
	int x;
	int parenthesis=0;

	//left-to-right scan
	for (x=Get1stNonBlank(first, last); x<=last; x++) {
		switch (target[x]) {
			case '(':
				parenthesis++;
				break;
			case ')':
				parenthesis--;
				if (parenthesis<0)	//unmatched ')'
					return x;
		}
	}

	return x;
}


