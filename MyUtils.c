#include <stdio.h>
#include "MyUtils.h"
#include <string.h>
#include <stdlib.h>

int not_parenthesized;

void MyUtils_TrimStr(char *target) {
	int first, last, i;

	if (target==NULL)
		return;
	//get the index of first non-blank char (NULL is non-blank)
	for (first=0; target[first]!='\0' &&
         (target[first]==' ' || target[first]=='	'); first++) {}

	//get the index of char next to last non-blank (not of NULL)
	last = first;
	for (i=first; target[i]!='\0'; i++)
		if (target[i]!=' ' && target[i]!='	')
			last = i+1;

	//move the trimmed string
	for (i=first; i<last; i++)
		target[i-first]=target[i];
	target[i-first]='\0';
	return;
}

char* MyUtils_fixStringIfNotParenthesized(char* tmp)
{
		
		not_parenthesized = MyUtils_check_parentheses(tmp, 1);
		
		if(not_parenthesized)
		{
			tmp = MyUtils_parenthesize(tmp);
		}
		
	return tmp;
}

int MyUtils_check_parentheses(char* line, int index)
{
	int i;
	int length;
	
	length =  strlen(line) - 1;
	
	/*for(i = 0; i <= length; i++)
	{
		printf("%c", line[i]);
	}*/
	
	if( (line[0] != '(') && (index == 1) )
	{
		//printf("Not parenthesized\n");
		return 1;
	}
	else
	{
		for(i = index; i <= length; i++)
		{
			//printf("i is : %d\n", i);
			//printf("line[i] %c\n", line[i]);
			if( (line[i] == ')') && (i != length) )
			{
				//printf("Not parenthesized\n");
				return i;
			}
			else if(line[i] == '(')
			{
				i = MyUtils_check_parentheses(line, i+1);
			}
		}
	} 
	
	//printf("Parenthesized\n");
	
	return 0;
	
}

char* MyUtils_parenthesize(char* line)
{
	int i;
	int length = strlen(line) - 1;
	int p_index;
	int size1, size2;
	int parenthesis = 0;
	
	char* beginning;
	char* end;
	
	
	for(i = 0; i <= length; i++)
	{
		if(line[i] == '(')
		{
			p_index = i;
			parenthesis = 1;
			break;
		}
	}
	
	if(parenthesis == 0)
	{
		p_index = 0;
	}
	
	//We've got the index of the first (
	size1 = p_index + 2; //+ 1 for the terminating , +1 for the (
	
	beginning = (char*) malloc(size1 * sizeof(char));
	
	size2 = length + 1 - p_index + 2;
	
	end = (char*) malloc(size2 * sizeof(char));
	
	memset(beginning, '\0', sizeof(beginning));
	memset(end, '\0', sizeof(end));
	
	for(i = 0; i <= length; i++)
	{
		if(i < p_index)
		{
			beginning[i] = line[i];
		}
		else
		{
			end[i-p_index] = line[i];
		}
	}
	
	beginning[p_index] = '(';
	end[length-p_index +1] = ')';
	
	char* result = malloc(strlen(beginning) + strlen(end) + 1);
	
	strcpy(result, beginning);
	strcat(result, end);
	
	return result;
}

