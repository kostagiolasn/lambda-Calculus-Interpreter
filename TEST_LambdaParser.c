#include <stdio.h>
#include <stdlib.h>
#include "InputReader.h"
#include "MyUtils.h"
#include "LambdaParser.h"
#include "LambdaUtils.h"
#include "LambdaSyntaxTree.h"


int main(int argc, char *argv[]){
	FILE *input;
	Term tmpTerm;
	char *tmp;
	int i;

	if (argc>1) {
		input = fopen(argv[1], "r");
		if (input==NULL) {
			printf("Cannot open input file '%s'!\n", argv[1]);
			exit(0);
		}
	}
	else
		input=stdin;



	while ((tmp=InputReader_ReadData(input))!=NULL) {
		printf("READ: \"%s\"\n", tmp);
		printf("TYPE: %d\n", InputReader_GetTypeOfLastData());
		MyUtils_TrimStr(tmp);
		printf("TRIM: \"%s\"\n", tmp);

		tmpTerm = LambdaParser_Parse(tmp);

		printf("term: '");
		LambdaUtils_PrintTerm(stdout, tmpTerm);
		printf("'\n");

		if (tmpTerm==NULL) {
			printf("ERROR: %s\n", LambdaParser_GetErrorMessage());
			printf("code: %d\n", LambdaParser_GetErrorCode());
			printf("input term: '%s'\n", tmp);
			printf("pos%5d =>  ", LambdaParser_GetErrorByte());
			for (i=0; i<LambdaParser_GetErrorByte(); i++) {
				putchar(' ');
			}
			printf("*\n");
		}
		printf("\n");

		free(tmp);
	};
	return 0;
}
