#ifndef DICTIONARY_H
#define DICTIONARY_H


#define DICT_OK 0
#define DICT_ERROR -1
#define DICT_NOTFOUND 1
#define DICT_SHRINK 2
#define DICT_REPLACED 3

typedef struct {
	int dict_size_increasement;
	int dict_size_decreasement;
	int (*compar)(void *element1, void *element2);
	void **array;
	int arraySize;
	int elementCount;
} Dictionary;


Dictionary *Dictionary_New(
	int dict_init_size,
	int dict_size_increasement,
	int dict_size_decreasement,
	int (*compar)(void *element1, void *element2)
);

int Dictionary_AddElement(Dictionary *dict, void *element);

void *Dictionary_FindElement(Dictionary *dict, void *element);

int Dictionary_RemoveElement(Dictionary *dict, void *element);

int Dictionary_CountElements(Dictionary *dict);

void **Dictionary_GetAllElements(Dictionary *dict);

void Dictionary_Destroy(Dictionary *dict);

#endif

