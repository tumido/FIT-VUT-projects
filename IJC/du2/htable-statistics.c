/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 09:18:00 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

#include "htable.h"

/*   Statistika delky zaznamu v tabulce
 * ---------------------------------------------------------------------
 * - ladici funkce slouzici k vypisu statistik hashovaci tabulky
 * - vypisuje minimalni, maximalni a prumerny pocet zaznamu v radku
 */
void htab_statistics(htab_t * t)
{
	long sum = 0;
	long min = -1;
	long max;
	for (unsigned int i=0; i<t->size; i++)
	{
		int length = 0;
		for (struct htab_listitem * item = (t->list)[i]; item != NULL; item = item->next)
		{
			length++;
		}
		if (min == -1) { min = length; max = length; }
		if (length < min) { min = length; }
		if (length > max) { max = length; }
		sum += length;
	}
	printf("\nPrumer: %f\n", (double)sum / t->size  );
	printf("Min: %ld\n", min);
	printf("Max: %ld\n", max);
}
