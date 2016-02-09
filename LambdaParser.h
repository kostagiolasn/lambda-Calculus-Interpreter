#ifndef LAMBDAPARSER_H
#define LAMBDAPARSER_H

#include "LambdaSyntaxTree.h"


Term LambdaParser_Parse(char *input);

char *LambdaParser_GetErrorMessage();

int LambdaParser_GetErrorCode();

int LambdaParser_GetErrorByte();

int GetAfterEndOfTerm(int first, int last);

Term internal_ParseVar(int first, int last);

#endif

