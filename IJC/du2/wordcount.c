/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 05:44:34 AM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

/*
 * =====================================================================
 *                   Program pro vypocet vyskytu slov
 * =====================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "htable.h"
#include "io.h"

#define HTABLE_SIZE 12289
/*
 * Abych se priznal tak netusim, jak spravne zvolit velikost hashovaci
 * tabulky. Co jsem se docetl, tak je fajn, kdyz je to prvocislo a kdyz
 * se velikost tabulky drzi do 3/4 obsazenosti (coz zde osetrit nelze).
 * Jelikoz jsem tabulku testoval na nahodne stazene knizce z internetu,
 * konkretne Harry Potter 2. dil (asi proto, ze hashovani taky trochu
 * magie), zvolil jsem velikost 12289. Napovedou mi byla tabulka
 * http://planetmath.org/goodhashtableprimes
 */
#ifndef MAX_LENGTH
#define MAX_LENGTH 127
#endif

/*   Vypis slova
 * ---------------------------------------------------------------------
 * - vypise slovo a pocet, kolikrat se v textu objevilo
 */
void print_item (const char * key, unsigned int value)
{
	printf("%s\t%u\n", key, value);
}

/*   Main
 * ---------------------------------------------------------------------
 * - funkce bere slova ze standardniho vstupu a zapisuje je do hashovaci
 *   tabulky
 * - pote pro kazdy item v hash tabulce vytvori vypis
 */
int main()
{
	htab_t * t;
	if ((t = htab_init(HTABLE_SIZE)) == NULL)
	{
		return EXIT_FAILURE;
	}

	htab_remove(t,"Harry");
	char word [MAX_LENGTH];
	int retVal;
	while ((retVal = fgetw(word, MAX_LENGTH + 1, stdin)) != 0 && retVal != EOF)
	{
		struct htab_listitem * item;
		if ((item = htab_lookup(t, word)) == NULL)
		{
			htab_free(t);
			return EXIT_FAILURE;
		}
		item->data += 1;
	}

	htab_remove(t, "Harry");
	htab_foreach(t, print_item);

	htab_statistics(t);
	htab_free(t);

	return EXIT_SUCCESS;
}

