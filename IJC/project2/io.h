/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 10:46:48 AM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

/*
 * =====================================================================
 *              Rozhrani modulu pro spravu vstupu s vystupu
 * =====================================================================
 */

#ifndef IO_INCLUDED
#define IO_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_LENGTH 127

/*   Warning
 * ---------------------------------------------------------------------
 * - funkce pro vypsani chyboveho hlaseni na stderr
 */
void printErr(const char * msg);

/*   Ziskani slova ze vstupu
 * ---------------------------------------------------------------------
 * - ze souboru nacte slovo o dane maximalni delce
 * - preskakuje whitespace znaky
 * - po nacteni slova korektne ukonci string
 * - vraci delku slova
 */
int fgetw(char *s, int max, FILE *f);
#endif
