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

/*   Hashovaci funkce
 * ---------------------------------------------------------------------
 * - hasovaci funkce ze zadani
 */
unsigned int hash_function(const char *str, unsigned htab_size)
{
  unsigned int h=0;
  const unsigned char *p;
  for(p=(const unsigned char*)str; *p!='\0'; p++)
    h = 65599*h + *p;
  return h % htab_size;
}
