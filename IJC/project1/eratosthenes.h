/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  25.3.2014 9:58:45
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IZP - Projekt c.1 - priklad A
 * =====================================================================
 */

/* =====================================================================
 *                  Rozhrani pro modul eratosthenes.c
 * =====================================================================
 */
#ifndef ERATOSTHENES_INCLUDED
#define ERATOSTHENES_INCLUDED
#include <stdio.h>
#include <math.h>
#include "bit-array.h"

/*  Eratosthenovo sito
 * ---------------------------------------------------------------------
 * - Eratosthenovou metodou nalezne v bitovem poli prvocisla
 * - vsechna prvocisla maji hodnotu 0, ostatni bity jsou nastaveny na 1
 * - zacina na bitu s indexem 2, bity na indexech 0 a 1 zustavaji taktez
 *   hodnote 0, prestoze prvocisly nejsou
 */
void Eratosthenes(BitArray_t pole);
#endif
