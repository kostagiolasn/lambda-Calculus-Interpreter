#ifndef TERMDICTIONARY_H
#define TERMDICTIONARY_H

#include "dictionary.h"

#define TERMDICT_OK DICT_OK             /* should be 0 */
#define TERMDICT_ERROR DICT_ERROR       /* should be -1 */
#define TERMDICT_NOTFOUND DICT_NOTFOUND  /*should be 1 */
#define TERMDICT_SHRINK DICT_SHRINK      /*should be 2 */
#define TERMDICT_REPLACED DICT_REPLACED  /*should be 3 */

typedef Dictionary TermDictionary;

extern int Dictionary_errno;


TermDictionary *TermDictionary_New(
	int dict_init_size,
	int dict_size_increasement,
	int dict_size_decreasement
);

int TermDictionary_AddTermDef(TermDictionary *dict, char *key, Term value);

Term TermDictionary_FindTermDef(TermDictionary *dict, char *key);

int TermDictionary_RemoveTermDef(TermDictionary *dict, char *key);

int TermDictionary_CountTermDefs(TermDictionary *dict);

char **TermDictionary_GetAllTermDefKeys(TermDictionary *dict);

Term *TermDictionary_GetAllTermDefValues(TermDictionary *dict);

void TermDictionary_Destroy(TermDictionary *dict);

#endif
