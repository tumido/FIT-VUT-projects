/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 09:13:31 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

#include "htable.h"

/*   Zavolani funkce pro kazdy prvek
 * ---------------------------------------------------------------------
 * - vola zadanou funkci typu callback pro kazdy prvek v hashovaci
 *   tabulce
 * - nemeni obsah tabulky
 */
void htab_foreach(htab_t * t, callback function)
{
	for (unsigned int i=0; i<t->size; i++)
	{
		for (struct htab_listitem * item = (t->list)[i]; item != NULL; item = item->next)
		{
			function(item->key, item->data);
		}
	}
}
