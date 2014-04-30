/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  22.3.2014 22:14:54
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c.1 - priklad B
 * =====================================================================
 */

/*
 * =====================================================================
 *         Rozhrani pro modul ppm.c pro zpracovani obrazku PPM
 * =====================================================================
 */
#ifndef PPM_INCLUDED
#define PPM_INCLUDED

#include <stdio.h>
#include <string.h>
#include "error.h"
#define EXIT_FILEFAIL -1
#define MAX_PIC_SIZE 3*1000*1000

/*  Struktura pro obrazek
 * ---------------------------------------------------------------------
 * - struktura do ktere se bude nahravat obrazek
 * - obsahuje rozmery obrazku a obrazova data
 */
struct ppm {
  unsigned xsize;
  unsigned ysize;
  char data[]; // RGB bajty, celkem 3*xsize*ysize }
};

/*   Nacteni obrazku
 * ---------------------------------------------------------------------
 * - nacita ze souboru do struktury ppm, alokuje ji
 * - kontroluje format obrazku
 * - v pripade chyby formatu vola funkci Warning z modulu error.c
 */
struct ppm * ppm_read(const char * filename);

/*   Vytvoreni obrazku
 * ---------------------------------------------------------------------
 * - vytvori obrazek ze struktury PPM
 * - v pripade chyby formatu vola funkci Warning z modulu error.c
 */
int ppm_write(struct ppm *p, const char * filename);

#endif
