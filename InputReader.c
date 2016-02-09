#include <stdio.h>
#include <stdlib.h>
#include "InputReader.h"

#define MEM_QUANTUM 1000	//buffer initial size and increasement...
				//...while reading, must be >=2

static int dataType = -1;

char *InputReader_ReadData(FILE *input) {
	char *buf=NULL;
	int in;
	int count;
	int bufSize=0;

	if (input==NULL)
		return NULL;

	for (count=0; (in=fgetc(input))!='\n' && in!=EOF; count++) {
		if (bufSize<count+2) {
			bufSize += MEM_QUANTUM;
			buf = (char *)realloc(buf, bufSize*sizeof(char));
		}
		buf[count]=in;
	}

	if (count==0) {
		if (in==EOF)
			dataType = 3;	//note EOF
		else if (in=='\n')
			dataType = 4;	//note blank line
		else
			dataType = 5;	//note no input, unknown cause
		return NULL;
	}

	for (in=0; in<count; in++)
		if (buf[in]=='#') {
			count=in;
			break;
		}
	buf[count]='\0';

	for (in=0; in<count; in++) {
		if (buf[in]!=' ' && buf[in]!='	') {
			if (buf[in]==':')
				dataType=2;	//interpreter command
			else
				dataType=1;	//LAMBDA term
			return buf;
		}
	}

	//if we get here, only blanks in input...
	dataType=4;	//note blank line
	return NULL;
}


int InputReader_GetTypeOfLastData() {
	return dataType;
}


