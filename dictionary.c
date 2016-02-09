#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

static int compar_adapter(const void *element1, const void *element2);

int Dictionary_errno = 0;

static int (*tmp_compar)(void *element1, void *element2) = NULL;

Dictionary *Dictionary_New(
	int dict_init_size,
	int dict_size_increasement,
	int dict_size_decreasement,
	int (*compar)(void *element1, void *element2)
) {
	Dictionary *tmp;

	/* parameter check */
	if (dict_init_size<0 || dict_size_increasement<1 || dict_size_decreasement<0 || compar==NULL) {
		Dictionary_errno = 1;
		return NULL;
	}
	
	/* memory allocation for structure */
	tmp  = (Dictionary *)malloc(sizeof(Dictionary));
	if (tmp==NULL) {
		Dictionary_errno = 2;
		return NULL;
	}
	
	/* initialization of Dictionary structure */
	tmp->dict_size_increasement = dict_size_increasement;
	tmp->dict_size_decreasement = dict_size_decreasement;
	tmp->compar = compar;
	if (dict_init_size>0) {
		tmp->array = (void **)malloc(dict_init_size * sizeof(void *));
		if (tmp->array==NULL) {
			Dictionary_errno = 3;
			return NULL;
		}
	}
	else
		tmp->array=NULL;
	tmp->arraySize = dict_init_size;
	tmp->elementCount = 0;
	return tmp;
}


int Dictionary_AddElement(Dictionary *dict, void *element) {
	void **tmp = NULL;

	if (dict==NULL || element==NULL) {
		Dictionary_errno = 4;
		return DICT_ERROR;
	}

	if (dict->array != NULL && dict->elementCount>0) {
		tmp_compar = dict->compar;
		tmp = (void **)bsearch(&element, (void *)dict->array, dict->elementCount, sizeof(void *), compar_adapter);
		tmp_compar = NULL;
		if (tmp != NULL) {
			*tmp = element;
			return DICT_REPLACED;
		}
	}
			

	if (dict->elementCount + 1 > dict->arraySize) {
		tmp = (void **)realloc((void *)(dict->array), (dict->arraySize + dict->dict_size_increasement) * sizeof(void *));
		if (tmp==NULL) {
			Dictionary_errno = 5;
			return DICT_ERROR;
		}
		dict->arraySize += dict->dict_size_increasement;
		dict->array = tmp;
	}
	
	*(dict->array + dict->elementCount) = element;
	dict->elementCount++;

	tmp_compar = dict->compar;
	qsort(dict->array, dict->elementCount, sizeof(void *), compar_adapter);
	tmp_compar = NULL;

	return DICT_OK;
}


void *Dictionary_FindElement(Dictionary *dict, void *element) {
	void **tmp_result;

	if (dict==NULL || element==NULL || dict->array==NULL || dict->elementCount<1)
		return NULL;
	tmp_compar = dict->compar;
	tmp_result = (void **)bsearch(&element, (void *)dict->array, dict->elementCount, sizeof(void *), compar_adapter);
	tmp_compar = NULL;

	if (tmp_result == NULL)
		return NULL;

	return *tmp_result;
}


static int compar_adapter(const void *element1, const void *element2) {
	return tmp_compar( *(void **)element1, *(void **)element2);
}


int Dictionary_RemoveElement(Dictionary *dict, void *element) {
	void **tmp_result, **tmp;
	int index, i, k=0;

	if (dict==NULL || element==NULL) {
		Dictionary_errno = 6;
		return DICT_ERROR;
	}

	if (dict->array == NULL || dict->elementCount < 1)
		return DICT_NOTFOUND;

	tmp_compar = dict->compar;
	tmp_result = (void **)bsearch(&element, dict->array, dict->elementCount, sizeof(void *), compar_adapter);
	tmp_compar = NULL;

	if (tmp_result == NULL)
		return DICT_NOTFOUND;

	index = (  ((int)tmp_result) - ((int)(dict->array))  )/ sizeof(void *);
	
	for (i=index+1; i<dict->elementCount; i++) {
		*(dict->array + i - 1) = *(dict->array + i);
	}

	dict->elementCount--;

	if (dict->dict_size_decreasement != 0) {
		k =(dict->arraySize - dict->elementCount) / dict->dict_size_decreasement;
		if (k>0) {
			tmp = (void **)realloc(dict->array, (dict->arraySize - (k * dict->dict_size_decreasement)) * sizeof(void *));
			dict->array = tmp;
			dict->arraySize -= (k * dict->dict_size_decreasement);
		}
	}
	if (k>0)
		return DICT_SHRINK;
	return DICT_OK;
}

int Dictionary_CountElements(Dictionary *dict) {
	if (dict == NULL) {
		Dictionary_errno = 7;
		return DICT_ERROR;
	}

	return dict->elementCount;
}

void **Dictionary_GetAllElements(Dictionary *dict) {
	void **result;
	
	if (dict == NULL) {
		Dictionary_errno = 8;
		return NULL;
	}

	if (dict->elementCount<1) {
		Dictionary_errno = 9;
		return NULL;
	}

	result = (void **)malloc(dict->elementCount * sizeof(void *));

	if (result==NULL) {
		Dictionary_errno = 10;
		return NULL;
	}

	memcpy((void *)result, (void *)(dict->array), dict->elementCount * sizeof(void *));

	return result;
}

void Dictionary_Destroy(Dictionary *dict) {
	if (dict!=NULL) {
		if (dict->array != NULL)
			free(dict->array);
		free(dict);
	}
	return;
}

