/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 09:14:35 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

#include "htable.h"

/*   Vyhledani a zruseni zadane polozky
 * ---------------------------------------------------------------------
 * - spocita si radek tabulky, kde se pozadovany prvek nachazi
 * - pokud jej v tomto radku opravdu nalezne, provede uvolneni pameti a
 *   nastavi ukazatel next predchoziho prvku na prvek nasledujici
 */
void htab_remove(htab_t * t, const char * key)
{
	unsigned int index = hash_function(key, t->size);
	struct htab_listitem * item = t->list[index];

	struct htab_listitem * itemPrev;

	if (item != NULL && strcmp(item->key, key) == EXIT_SUCCESS)
	{
		t->list[index] = item->next;
	} else
	{
		while (item != NULL && strcmp(item->key, key) != EXIT_SUCCESS)
		{
			itemPrev = item;
			item = item->next;
		}
	}

	if (item != NULL)
	{
		itemPrev->next = item->next;
		free(item->key);
		free(item);
	}
}
