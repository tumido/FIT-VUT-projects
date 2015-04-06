/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  19.3.2014 22:31:45
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IZP - Projekt c.1 - priklad A
 * =====================================================================
 */

/*
 * =====================================================================
 *                  Program pro vypocet prvocisel
 * =====================================================================
 */

#include <stdio.h>

#define SIZE 101000000L
#include "eratosthenes.h"

/*   Main
 * ---------------------------------------------------------------------
 * - necha vytvorit bitove pole o veliosti SIZE
 * - pomovi Eratosthenova sita nalezne prvocisla
 * - poslednich 10 vypise
 */
int main()
{
  BitArray(pole, SIZE);
  Eratosthenes(pole);

  unsigned long result[10];

  unsigned long k = BitArraySize(pole) - 1;
  for (int i = 0; i < 10; i++)
  {
    while (GetBit(pole, k) != 0)
    {
      k--;
    }
    result[i] = k;
    k--;
  }

  for (int i = 9; i >= 0; i--)
  {
    printf("%lu\n", result[i]);
  }

  return EXIT_SUCCESS;
}
