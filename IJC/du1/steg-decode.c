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
 *           Progam pro dekodovani ukrytych zprav z obrazku
 * =====================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ppm.h"
#include "error.h"
#include "eratosthenes.h"


/*   Dekodovani dat
 * ---------------------------------------------------------------------
 * - vytvori si bitove pole, kde nalezne prvocisla
 * - prochazi data obrazku a hleda na prvociselnych bytech posledni
 *   bity, ze kterych poslada pismena
 * - overi tisknutelnost pismene a vytiskne jej
 * - hleda dokud nedojde na konec dat, ci nenarazi na konec retezce
 */
void decodePpm(struct ppm * pic)
{
  const unsigned long sizePic =  3*pic->xsize*pic->ysize*sizeof(char);
  BitArray(primes, MAX_PIC_SIZE);
  Eratosthenes(primes);

  char letter = '\0';
  unsigned long k = 2;
  unsigned long lsb;

  while (k < sizePic)
  {
    for (int i = 0; i < sizeof(char) * 8 ; i++)
    {
      while (GetBit(primes, k) != 0)
        k++;
      lsb = DU1__GET_BIT(&pic->data[k], 0);
      DU1__SET_BIT((&letter), i, lsb);
      k++;
    }
    if (!isprint(letter) && letter != '\0')
    {
      free(pic);
      FatalError("\nNeni mozne vypsat pismeno.\n");
    }
    putchar(letter);

    if (letter == '\0')
      break;
  }
  if (letter != '\0')
  {
    free(pic);
    FatalError("Zasifrovana zprava neni platnym retezcem.\n");
  }

  putchar('\n');
  return;
}
/*   Main
 * ---------------------------------------------------------------------
 * - zpracuje argumenty a pokracuje pouze, kdyz ma prave jeden argument
 * - zavola nacteni obrazku
 * - zavola dekodovani nad obrazkem
 */
int main(int argc, char * argv[])
{
  struct ppm * pic = NULL;
  if (argc != 2)
  {
    FatalError("Tento program potrebuje prave jeden argument, kterym je souboru typu PPM.\n");
  }

  if ( (pic = ppm_read((argv[1]))) == NULL)
  {
    FatalError("Neni mozne nacist obrazek %s\n", argv[1]);
  }

//  ppm_write(pic, "duplic1");

  decodePpm(pic);
  free(pic);

  return EXIT_SUCCESS;
}
