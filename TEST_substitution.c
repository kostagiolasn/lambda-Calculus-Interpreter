#include <stdio.h>
#include <stdlib.h>
#include "InputReader.h"
#include "MyUtils.h"
#include "LambdaParser.h"
#include "LambdaUtils.h"
#include "LambdaSyntaxTree.h"
#include "LambdaReductionEngine.h"


int main(int argc, char *argv[]){
	FILE *input;
	Term tmpTerm;
	char *tmp;
	int i, count;

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
		LambdaUtils_PrintTermCompact(stdout, tmpTerm);
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
		else {
			//LambdaReductionEngine_Normalize(tmpTerm);
			count = 0;
			while ((i=LambdaReductionEngine_NormalizeStep(tmpTerm))!=0) {
				count++;
				printf("%d) did redex %d: '", count, i);
				LambdaUtils_PrintTermCompact(stdout, tmpTerm);
				printf("'\n");
			}

			printf(" result: ");
			LambdaUtils_PrintTermCompact(stdout, tmpTerm);
		}
		printf("\n");

		free(tmp);
		LambdaSyntaxTree_FreeTerm(tmpTerm);
	};

	return 0;
}
