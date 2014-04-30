/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  22.3.2014 14:04:45
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IZP - Projekt c.1 - priklad B
 * =====================================================================
 */

/*
 * =====================================================================
 *                      Modul pro chybova volani
 * =====================================================================
 */

#include "error.h"

/*   Varovani
 * ---------------------------------------------------------------------
 * - funkce s promenilivym poctem argumentu
 * - vypise retezec na stderr
 * - dovoluje formatovani
 */
void Warning(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fputs("Chyba: ", stderr);
  vfprintf(stderr, fmt, args);
  va_end(args);
  return;
}

/*   Chyba
 * ---------------------------------------------------------------------
 * - funkce s promenilivym poctem argumentu
 * - vypise retezec na stderr
 * - dovoluje formatovani
 * - ukonci program volanim exit(1)
 */
void FatalError(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fputs("Chyba: ", stderr);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(1);
  return;
}
