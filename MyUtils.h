#ifndef MYUTILS_H
#define MYUTILS_H

void MyUtils_TrimStr(char *target);

char* MyUtils_fixStringIfNotParenthesized(char* tmp);

int MyUtils_check_parentheses(char* line, int index);

char* MyUtils_parenthesize(char* line);

extern int not_parenthesized;

#endif

