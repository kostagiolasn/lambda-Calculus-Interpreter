#include <stdio.h>
#include <stdlib.h>
#include "InputReader.h"
#include "MyUtils.h"


int main(int argc, char *argv[]){
	char *tmp;



	while ((tmp=InputReader_ReadData(stdin))!=NULL) {
		printf("READ: \"%s\"\n", tmp);
		printf("TYPE: %d\n", InputReader_GetTypeOfLastData());
		MyUtils_TrimStr(tmp);
		printf("TRIM: \"%s\"\n", tmp);
		free(tmp);
	};
	return 0;
}
