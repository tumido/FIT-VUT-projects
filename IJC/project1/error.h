/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  22.3.2014 14:04:41
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IZP - Projekt c.1 - priklad B
 * =====================================================================
 */

/*
 * =====================================================================
 *                    Rozhrani pro modul error.c
 * =====================================================================
 */
#ifndef ERROR_INCLUDED
#define ERROR_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*   Varovani
 * ---------------------------------------------------------------------
 * - funkce s promenilivym poctem argumentu
 * - vypise retezec na stderr
 * - dovoluje formatovani
 */
void Warning(const char *fmt, ...);

/*   Chyba
 * ---------------------------------------------------------------------
 * - funkce s promenilivym poctem argumentu
 * - vypise retezec na stderr
 * - dovoluje formatovani
 * - ukonci program volanim exit(1)
 */
void FatalError(const char *fmt, ...);
#endif
