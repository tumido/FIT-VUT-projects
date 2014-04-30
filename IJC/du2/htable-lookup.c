/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 09:20:18 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

#include "htable.h"

/*   Hledani zaznamu v tabulce
 * ---------------------------------------------------------------------
 * - hleda zaznam v tabulce, pokud jej nenajde, necha vytvorit novy
 */
struct htab_listitem * htab_lookup(htab_t *t, const char *key)
{
	unsigned int index = hash_function(key, t->size);
	struct htab_listitem * item = (t->list)[index];

	if (item == NULL) // na indexu neni zadny prvek
	{
		(t->list)[index] = htab_new(key); // pokusim se vytvorit
		return t->list[index];
	}

	struct htab_listitem * itemPrev = item;

	while (item != NULL)
	{
		if (strcmp(item->key, key) == EXIT_SUCCESS) { return item; }
		itemPrev = item;
		item = item->next;
	}

	return itemPrev->next = htab_new(key);
}

/*   Vytvoreni noveho zaznamu
 * ---------------------------------------------------------------------
 * - vytvori novy zaznam pro hash tabulku a vrati ukazatel na ni
 * - zaznam obsahuje key, data = 0 a ukazatel next je NULL
 * - pokud dojde k chybe (nedostatek pameti) vraci NULL
 */
struct htab_listitem * htab_new(const char *key)
{
	struct htab_listitem * item = malloc(sizeof(struct htab_listitem));
	if (item == NULL) { return NULL; }

	item->data = 0;
	item->next = NULL;

	if ((item->key = malloc(sizeof(char) * (strlen(key) + 1))) == NULL)
	{
		free(item);
		return NULL;
	}

	strcpy(item->key, key);
	return item;
}
