#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "InputReader.h"
#include "MyUtils.h"
#include "LambdaParser.h"
#include "LambdaUtils.h"
#include "termDictionary.h"
#include "LambdaSyntaxTree.h"
#include "LambdaReductionEngine.h"

extern int errno;

static int details=0;	//show details in every reduction?
static int limit=0;	//limit of reductions per normalization
static int dot=0;	//use "dot" notation? (else use "arrow" notation)
static int prompt=-1;	//show "lambda> " prompt?
static TermDictionary *bindings;	//bindings dictionary

int not_parenthesized;


int Interpreter_MainLoop(FILE *input);
static int Directive_read(char *param);
static int Directive_prelude(char *param);
static int Directive_details(char *param);
static int Directive_nodetails(char *param);
static int Directive_limit(char *param);
static int Directive_nolimit(char *param);
static int Directive_let(char *param);
static int Directive_unlet(char *param);
static int Directive_unletall(char *param);
static int Directive_reduce(char *param);
static int Directive_dot(char *param);
static int Directive_arrow(char *param);
static int Directive_exit(char *param);
static int Directive_help(char *param);
static int Directive_settings(char *param);
static int Directive_prompt(char *param);
static int Directive_noprompt(char *param);
static int SelectDirective(char *tmp);

static char *redexNames[3] = { "BETA", " ETA", "CALC" };


int main(int argc, char *argv[]){
	int i;

	bindings = TermDictionary_New(10, 10, 10);

	if (argc>1) {
		for (i=1; i<argc; i++)
			if (Directive_read(argv[i])!=0)
				exit(0);
	}

	printf("Use \":help\" to read a short help text.\n");
	Interpreter_MainLoop(stdin);

	printf("\n");
	return 0;
}






static int Directive_read(char *param) {
	FILE *input;
	int keepPrompt;
	int ret=0;

	MyUtils_TrimStr(param);
	if (param==NULL)
		return 0;

	input = fopen(param, "r");
	if (input==NULL)
		printf("Could not read from file '%s'.\n", param);
	else {
		keepPrompt = prompt;
		prompt=0;
		ret = Interpreter_MainLoop(input);
		prompt=keepPrompt;
		fclose(input);
	}
	return ret;
}


static int Directive_prelude(char *param) {
	char **termStrs = TermDictionary_GetAllTermDefKeys(bindings);
	Term *termValues = TermDictionary_GetAllTermDefValues(bindings);
	int  i = TermDictionary_CountTermDefs(bindings);
	int x;

	for (x=0; x<i; x++) {
		printf("%s = ", termStrs[x]);
		if (dot)
			LambdaUtils_PrintTermCompact(stdout, termValues[x]);
		else
			LambdaUtils_PrintTerm(stdout, termValues[x]);
		printf("\n");
	}

	free(termStrs);
	free(termValues);
	return 0;
}

static int Directive_details(char *param) {
	details=-1;
	return 0;
}


static int Directive_nodetails(char *param) {
	details=0;
	return 0;
}


static int Directive_limit(char *param) {
	int tmp;
	char *str;

	errno = 0;
	tmp = strtol(param, &str, 10);
	if (errno!=0)
		printf("Cannot read integer from input '%s'\n", param);
	else
		limit = tmp;
	return 0;
}


static int Directive_nolimit(char *param) {
	limit = 0;
	return 0;
}


static int Directive_let(char *param) {
	int i, t;
	char *key;
	Term tmpTerm;
	Term *termValues;
	char **termStrs;

	if (param==NULL)
		return 0;
	t = strlen(param);

	for (i=0; i<t && param[i]!=' ' && param[i]!='	'; i++)	{
	}
	if (i>=t)
		return 0;
	param[i]='\0';
	if (strlen(param)==0)
		return 0;
	key = strdup(param);
	
	tmpTerm = LambdaParser_Parse(param+i+1);
	if (tmpTerm == NULL) {
		free(key);
		printf("Error parsing Lambda-Term in LET command\n");
		return 0;
	}

	//replace all bindings in input term

	termStrs = TermDictionary_GetAllTermDefKeys(bindings);
	termValues = TermDictionary_GetAllTermDefValues(bindings);
	i = TermDictionary_CountTermDefs(bindings);

	for (t=0; t<i; t++)
		LambdaReductionEngine_Substitution(tmpTerm, termStrs[t], termValues[t]);
	free(termStrs);
	free(termValues);

	//add term in bindings
	TermDictionary_AddTermDef(bindings, key, tmpTerm);
	return 0;
}


static int Directive_unlet(char *param) {
	TermDictionary_RemoveTermDef(bindings, param);
	return 0;
}


static int Directive_unletall(char *param) {
	TermDictionary_Destroy(bindings);
	bindings = TermDictionary_New(10, 10, 10);
	return 0;
}


static int Directive_reduce(char *param) {
	Term tmp;

	tmp = TermDictionary_FindTermDef(bindings, param);
	if (tmp==NULL)
		printf("Binding '%s' not found.\n", param);
	else
		LambdaReductionEngine_Normalize(tmp);

	return 0;
}


static int Directive_dot(char *param) {
	dot = -1;
	return 0;
}


static int Directive_arrow(char *param) {
	dot = 0;
	return 0;
}


static int Directive_exit(char *param) {
	return 1;
}


static int Directive_help(char *param) {
	printf(""
"Lambda Interpreter  -  Nikos-Stefanos Kostagiolas, sdi1100039@di.uoa.gr\n"
"Commands	 Description\n"
":read <file>	Read input from filename <file>\n"
":prelude	Show the currently binded variables\n"
":details	Print details for every normalization (redexes, terms, etc)\n"
":nodetails	Don't print details for normalizations.\n"
":limit <num>	Perform a maximum of <num> reductions for every normalization\n"
":nolimit	No limit in number of reductions (same with \":limit 0\")\n"
":let <var> <t>  Bind variable <var> with lambda-term <t>. Every following\n"
"		  occurence of <var> will be replaced by lambda-term <t>\n"
":unlet <var>	Unbind variable <var>\n"
":unletall	Unbind all variables currently binded\n"
":reduce <var>	Normalize binded variable <var>\n"
":dot		Use \"dot\" notation for output, example:  (\\x.x)\n"
":arrow		Use \"arrow\" notation for output, example: (\\x -> x)\n"
":exit		Exit Interpreter, if in input from file then close file\n"
":help		Show this help message\n"
":settings	Print all the interpreter setting (dot,details,limit,prompt)\n"
":prompt		Enable prompt\n"
":noprompt	Disable prompt\n"

"");
	return 0;
}


static int Directive_settings(char *param) {
	printf("Details: ");
	if (details)
		printf("On");
	else
		printf("Off");
	printf("\nLimit: ");
	if (limit>0)
		printf("%d", limit);
	else
		printf("None");
	printf("\nNotation: ");
	if (dot)
		printf("Dot");
	else
		printf("Arrow");
	printf("\nPrompt : ");
	if (prompt)
		printf(" On\n");
	else
		printf(" Off\n");
	return 0;
}


static int Directive_prompt(char *param) {
	prompt = -1;
	return 0;
}

static int Directive_noprompt(char *param) {
	prompt = 0;
	return 0;
}


int Interpreter_MainLoop(FILE *input) {
	Term tmpTerm;
	char *tmp;
	int i, x, count;
	int dataType;
	Term *termValues;
	char **termStrs;

	if (input==NULL)
		return 0;

	while (1) {
		if (prompt)
			printf("lambda> ");
		tmp=InputReader_ReadData(input);
		dataType = InputReader_GetTypeOfLastData();
		if (tmp==NULL) {
			if (dataType==4)
				continue;	//blank line
			else
				return 0;
		}

		MyUtils_TrimStr(tmp);
		
		//printf("Trimmed string %s", tmp);

		if (dataType == 2) {	//it's a interpreter directive
			if (SelectDirective(tmp)==1) {
				free(tmp);
				break;
			}
			else {
				free(tmp);
				continue;
			}
		}

		//it's a plain lambda term...
		
		tmp = MyUtils_fixStringIfNotParenthesized(tmp);
		
		//printf("New Lambda term : %s\n", tmp);

		tmpTerm = LambdaParser_Parse(tmp);

		if (tmpTerm==NULL) {
			//printf("edw\n");
			printf("ERROR: %s\n", LambdaParser_GetErrorMessage());
			printf("input term: '%s'\n", tmp);
			printf("pos%5d =>  ", LambdaParser_GetErrorByte());
			for (i=0; i<LambdaParser_GetErrorByte(); i++) {
				putchar(' ');
			}
			printf("*\n");
		}
		else {
			//replace all bindings in input term

			termStrs = TermDictionary_GetAllTermDefKeys(bindings);
			termValues = TermDictionary_GetAllTermDefValues(bindings);
			i = TermDictionary_CountTermDefs(bindings);

			for (x=0; x<i; x++)
				LambdaReductionEngine_Substitution(tmpTerm, termStrs[x], termValues[x]);

			free(termStrs);
			free(termValues);


			//process term and do the normalizing
			if (!details) {
				if (limit>0) {
					if (LambdaReductionEngine_LimitedNormalize(tmpTerm, limit) >= limit)
						printf("Reached limit of %d reductions.\n", limit);
				}
				else
					LambdaReductionEngine_Normalize(tmpTerm);
			}
			else {
				count = 0;
				while ( (limit==0 || count<limit) && (i=LambdaReductionEngine_NormalizeStep(tmpTerm))!=0) {
					count++;
					printf("%d) %s-redex: ", count, redexNames[i-1]);
					if (dot)
						LambdaUtils_PrintTermCompact(stdout, tmpTerm);
					else
						LambdaUtils_PrintTerm(stdout, tmpTerm);
					printf("\n");
				}
				printf("normal form: ");
			}

			if (dot)
				LambdaUtils_PrintTermCompact(stdout, tmpTerm);
			else
				LambdaUtils_PrintTerm(stdout, tmpTerm);
		}
		printf("\n");

		free(tmp);
		LambdaSyntaxTree_FreeTerm(tmpTerm);
		
		not_parenthesized = 0;

	};

	return 0;
}


static int SelectDirective(char *tmp) {
	int i, t;
	int (*directiveFunc[17])(char *) = {
		Directive_read,
		Directive_prelude,
		Directive_details,
		Directive_nodetails,
		Directive_limit,
		Directive_nolimit,
		Directive_let,
		Directive_unlet,
		Directive_unletall,
		Directive_reduce,
		Directive_dot,
		Directive_arrow,
		Directive_exit,
		Directive_help,
		Directive_settings,
		Directive_prompt,
		Directive_noprompt
	};
	char *directiveNames[17] = {
		":read",
		":prelude",
		":details",
		":nodetails",
		":limit",
		":nolimit",
		":let",
		":unlet",
		":unletall",
		":reduce",
		":dot",
		":arrow",
		":exit",
		":help",
		":settings",
		":prompt",
		":noprompt"
	};

	if (tmp==NULL)
		return 0;

	t = strlen(tmp);
	for (i=0; i<t && tmp[i]!=' ' && tmp[i]!='	'; i++)	{
	}
	if (i>=t)
		i=t-2;
	else
		tmp[i]='\0';

	//exoume sto (tmp+i+1) tin parametro

	for (t=0; t<17; t++) {
		if (strcasecmp(directiveNames[t], tmp)==0) {
			MyUtils_TrimStr(tmp+i+1);
			return directiveFunc[t](tmp+i+1);
		}
	}

	//den bre8ike to zitoumeno directive....
	printf("Invalid Interpreter directive '%s'\n", tmp);

	return 0;
}




