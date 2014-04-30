/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 09:17:09 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

#include "htable.h"

/*   Zruseni tabulky
 * ---------------------------------------------------------------------
 * - provede uvolneni pameti urcene pro hashovaci tabulku
 */
void htab_free(htab_t * t)
{
	htab_clear(t);
	free(t->list);
	free(t);
	t = NULL;
}
