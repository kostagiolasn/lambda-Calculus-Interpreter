#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"
#include "LambdaSyntaxTree.h"
#include "termDictionary.h"

typedef struct {
	char *key;
	Term value;
} bindDef;

static int bindDefCompar(void *tmp1, void *tmp2);

int TermDictionary_errno = 0;

TermDictionary *TermDictionary_New(
	int dict_init_size,
	int dict_size_increasement,
	int dict_size_decreasement
) {
	TermDictionary *tmp;

	tmp = (TermDictionary *)Dictionary_New(
		dict_init_size,
		dict_size_increasement,
		dict_size_decreasement,
		bindDefCompar
	);

	if (tmp==NULL)
		TermDictionary_errno = Dictionary_errno;
	return tmp;
}

int TermDictionary_AddTermDef(TermDictionary *dict, char *key, Term value) {
	bindDef *tmp;
	int i;

	if (dict==NULL || key==NULL || value==NULL) {
		TermDictionary_errno = 21;
		return TERMDICT_ERROR;
	}
	tmp  = (bindDef *)malloc(sizeof(bindDef));
	if (tmp == NULL) {
		TermDictionary_errno = 22;
		return TERMDICT_ERROR;
	}
	tmp->key = key;
	tmp->value = value;
	i = Dictionary_AddElement((Dictionary *)dict, tmp);
	if (i<0)
		TermDictionary_errno = Dictionary_errno;
	return i;
}

Term TermDictionary_FindTermDef(TermDictionary *dict, char *key) {
	bindDef *tmp, *tmp2;

	if (dict==NULL || key==NULL) {
		TermDictionary_errno = 23;
		return NULL;
	}
	tmp  = (bindDef *)malloc(sizeof(bindDef));
	if (tmp == NULL) {
		TermDictionary_errno = 24;
		return NULL;
	}
	tmp->key = key;
	tmp2 = (bindDef *)Dictionary_FindElement((Dictionary *)dict, tmp);
	free(tmp);
	if (tmp2==NULL)
		return NULL;
	return tmp2->value;
}

int TermDictionary_RemoveTermDef(TermDictionary *dict, char *key) {
	int result;
	bindDef *tmp, *tmp2;

	if (dict==NULL || key==NULL) {
		TermDictionary_errno = 23;
		return TERMDICT_ERROR;
	}
	tmp  = (bindDef *)malloc(sizeof(bindDef));
	if (tmp == NULL) {
		TermDictionary_errno = 25;
		return TERMDICT_ERROR;
	}
	tmp->key = key;
	tmp2 = (bindDef *)Dictionary_FindElement((Dictionary *)dict, tmp);
	result = Dictionary_RemoveElement((Dictionary *)dict, tmp);
	if (tmp2!=NULL) {
		free(tmp2->key);
		LambdaSyntaxTree_FreeTerm(tmp2->value);
		free(tmp2);
	}
	free(tmp);

	if (result<0)
		TermDictionary_errno = Dictionary_errno;

	return result;
}

int TermDictionary_CountTermDefs(TermDictionary *dict) {
	int i;

	i = Dictionary_CountElements((Dictionary *)dict);
	if (i<0)
		TermDictionary_errno = Dictionary_errno;
	return i;
}

char **TermDictionary_GetAllTermDefKeys(TermDictionary *dict) {
	int count = TermDictionary_CountTermDefs((Dictionary *)dict);
	int i;
	bindDef **bindDefsArray;
	char **result;

	if (dict==NULL) {
		TermDictionary_errno = 26;
		return NULL;
	}

	bindDefsArray = (bindDef **)Dictionary_GetAllElements((Dictionary *)dict);
	if (bindDefsArray == NULL) {
		TermDictionary_errno = 27;
		return NULL;
	}
	
	result = (char **)malloc(count*sizeof(char *));
	if (result==NULL) {
		TermDictionary_errno = 28;
		return NULL;
	}
	
	for (i=0; i<count; i++)
		*(result+i) = (*(bindDefsArray+i))->key;

	free(bindDefsArray);
	return result;
}


Term *TermDictionary_GetAllTermDefValues(TermDictionary *dict) {
	int count = TermDictionary_CountTermDefs((Dictionary *)dict);
	int i;
	bindDef **bindDefsArray;
	Term *result;

	if (dict==NULL) {
		TermDictionary_errno = 29;
		return NULL;
	}

	bindDefsArray = (bindDef **)Dictionary_GetAllElements((Dictionary *)dict);
	if (bindDefsArray == NULL) {
		TermDictionary_errno = 30;
		return NULL;
	}

	result = (Term *)malloc(count*sizeof(Term));
	if (result==NULL) {
		TermDictionary_errno = 31;
		return NULL;
	}
	
	for (i=0; i<count; i++)
		*(result+i) = (*(bindDefsArray+i))->value;

	free(bindDefsArray);
	return result;
}

void TermDictionary_Destroy(TermDictionary *dict) {
	int count = TermDictionary_CountTermDefs((Dictionary *)dict);
	int i;
	bindDef **bindDefsArray = NULL;

	if (dict!=NULL)
		bindDefsArray = (bindDef **)Dictionary_GetAllElements((Dictionary *)dict);
	
	if (bindDefsArray != NULL) {
		for (i=0; i<count; i++) {
			free( (*(bindDefsArray+i))->key );
			LambdaSyntaxTree_FreeTerm((*(bindDefsArray+i))->value);
			free( *(bindDefsArray+i) );
		}
		free(bindDefsArray);
	}

	Dictionary_Destroy((Dictionary *)dict);
	return;
}

static int bindDefCompar(void *tmp1, void *tmp2) {
	return strcmp(((bindDef *)tmp1)->key, ((bindDef *)tmp2)->key);
}

