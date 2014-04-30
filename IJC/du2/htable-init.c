/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 10:32:59 AM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

#include "htable.h"

/*   Inicializace hashovaci tabulky
 * ---------------------------------------------------------------------
 * - allokuje pamet pro hashovaci tabluku
 * - alokuje pole dat hashovaci tabulky
 * - ulozi jeji velikost
 * - vrati ukazatel na nove vzniklou tabulku
 */
htab_t * htab_init(unsigned size)
{
	htab_t * t;

	if (size == 0 || (t = malloc(sizeof(htab_t))) == NULL)
		return NULL;

	if ((t->list = malloc(size * sizeof(struct htab_listitem * )))  == NULL)
	{
		free(t);
		return NULL;
	}

	for(unsigned int i=0; i < size; i++)
	{
		(t->list)[i] = NULL;
	}
	t->size = size;

	return t;
}
