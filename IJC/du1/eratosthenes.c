/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  19.3.2014 14:42:02
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IZP - Projekt c.1 - priklad A
 * =====================================================================
 */

/* =====================================================================
 *                  Modul pro Eratosthenovo sito
 * =====================================================================
 */

#include "eratosthenes.h"

/*  Eratosthenovo sito
 * ---------------------------------------------------------------------
 * - Eratosthenovou metodou nalezne v bitovem poli prvocisla
 * - vsechna prvocisla maji hodnotu 0, ostatni bity jsou nastaveny na 1
 */
void Eratosthenes(BitArray_t pole)
{
	const unsigned long sqtrPole = sqrt(BitArraySize(pole) + 1);
	for (unsigned long index = 2; (index <= sqtrPole); index++)
	{
		if (GetBit(pole,index) == 0)
		{
			for (unsigned long n = index * index; n < BitArraySize(pole); n+=index)
			{
				SetBit(pole, n, true);
			}
		}
	}
	return;
}

