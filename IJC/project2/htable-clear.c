/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 09:15:45 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

#include "htable.h"

/*   Zruseni vsech polozek v tabulce
 * ---------------------------------------------------------------------
 * - pro kazdy radek tabulky zavola rekurzivni funkci htab_clearRow
 * - ta se vola rekurzivne dokud ji neni ke sparacovali predany NULL
 *   pointer
 * - pri vynorovani provadi uvolneni pameti allokovane pro dany item
 */
void htab_clear(htab_t * t)
{
  for (unsigned int i=0; i < t->size; i++)
  {
    htab_clearRow((t->list)[i]);
    t->list[i] = NULL;
  }
}

void htab_clearRow(struct htab_listitem * item)
{
  if (item == NULL)
    return;
  htab_clearRow(item->next);
  free(item->key);
  free(item);
}
