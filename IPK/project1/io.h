/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 10:46:48 AM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IPK - Projekt c. 1
 *       Poznamka:  prevzato z projektu do IJC
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
#include <stdarg.h>
#include <string.h>

/*   print_err
 * ---------------------------------------------------------------------
 * - funkce pro vypsani chyboveho hlaseni na stderr
 */
void print_err(const char * ftm, ...);

void print_help();

void add_to_info(char * dst, char src);
void add_to_criteria(char * dst, char * src, unsigned long bufsize);
#ifdef DEBUG
void printDebug(const char * ftm, ...);
#endif


#endif
