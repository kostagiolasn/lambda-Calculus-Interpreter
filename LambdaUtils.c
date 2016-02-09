#include <stdio.h>
#include "LambdaSyntaxTree.h"
#include "LambdaUtils.h"

void LambdaUtils_PrintTerm(FILE *output, Term target) {
	if (target==NULL) {
		fprintf(output, "NULL");
		return;
	}
	if (target->childL!=NULL) {	//application
		fprintf(output, "(");
		LambdaUtils_PrintTerm(output, target->childL);
		fprintf(output, " ");
		LambdaUtils_PrintTerm(output, target->childR);
		fprintf(output, ")");
	}
	else if (target->childR!=NULL) {	//abstraction
		fprintf(output, "(\\%s -> ", target->id);
		LambdaUtils_PrintTerm(output, target->childR);
		fprintf(output, ")");
	}
	else
		fprintf(output, "%s", target->id);
}

void LambdaUtils_PrintTermCompact(FILE *output, Term target) {
	if (target==NULL) {
		fprintf(output, "NULL");
		return;
	}
	if (target->childL!=NULL) {	//application
		fprintf(output, "(");
		LambdaUtils_PrintTermCompact(output, target->childL);
		if ((target->childL->childL==NULL &&
			target->childL->childR==NULL) ||
			(target->childR->childL==NULL &&
			target->childR->childR==NULL)
		   )
			fprintf(output, " ");
		LambdaUtils_PrintTermCompact(output, target->childR);
		fprintf(output, ")");
	}
	else if (target->childR!=NULL) {	//abstraction
		fprintf(output, "(\\%s.", target->id);
		LambdaUtils_PrintTermCompact(output, target->childR);
		fprintf(output, ")");
	}
	else
		fprintf(output, "%s", target->id);
}

