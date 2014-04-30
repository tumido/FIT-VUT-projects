/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  18.3.2014 17:12:12
 *          Autor:  Tomas Coufal
 *          Login:  xcoufa09
 *        Projekt:  IZP - Projekt c.1 - priklad A
 * =====================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "error.h"

/*   Definice maker pro vytvoreni bitoveho pole
 * ---------------------------------------------------------------------
 * - LONGINBITS
 *     je pomocne makro pro spocitani poctu bitu v longu
 * - BitArray(jmeno_pole, velikost)
 *     vytvori pole longu "jmeno_pole" o velikosti "velikost" bitu
 *     (zaokrouhleno nahoru) + jeden prvek navic pro ulozeni velikosti
 * - BitArraySize
 *     vraci hodnotu prvniho prvku pole - velikost BitArray
 */
#define LONGINBITS (sizeof(unsigned long) * CHAR_BIT)
#define TYPEINBITS(type) (sizeof(type) * CHAR_BIT)

#define BitArray(jmeno_pole, velikost) unsigned long (jmeno_pole) [((velikost) / LONGINBITS) + 1 + (((velikost) % LONGINBITS) != 0)] = {0ul}; \
	(jmeno_pole)[0]=(velikost - 1)

typedef unsigned long * BitArray_t;

#define BitArraySize(jmeno_pole) (jmeno_pole)[0]

/*   Definice maker a inline funkci cteni a zapis bitu
 * ---------------------------------------------------------------------
 * - SetBit
 *     nastavi v bitovem poli bit na danem indexu na pozadovanou hodnotu
 *     a zaroven kontroluje meze bitoveho pole
 * - GetBit
 *     ziska hodnotu pozadovaneho bitu z bitoveho pole, makro s
 *     kontrolou mezi
 * - DU1__GET_BIT
 * - DU1__SET_BIT
 *     stejne jako SetBit a GetBit makra bez kontroly mezi a pro
 *     libovolny typ bitoveho pole
 * - USE_INLINE
 *     prepinac podmineneho prekladu, kdy se misto maker GetBit a SetBit
 *     pouziji inline funkce
 */
#ifndef USE_INLINE

#define SetBit(jmeno_pole,index,vyraz) \
	do { \
		if ((index) >= BitArraySize(jmeno_pole)) \
			(FatalError("Index %ld mimo rozsah 0..%ld", (index), (BitArraySize(jmeno_pole)))); \
	(vyraz) ? ((jmeno_pole)[1 + ((index)/LONGINBITS)] |= 1ul << ((index)%LONGINBITS)) : \
	((jmeno_pole)[1 + ((index)/LONGINBITS)] &= ~(1ul << ((index)%LONGINBITS))); \
	} while (0)

#define GetBit(jmeno_pole,index)  (((index) >= BitArraySize(jmeno_pole)) ? \
	(FatalError("Index %ld mimo rozsah 0..%ld", (index), (BitArraySize(jmeno_pole))), 0) : \
	((jmeno_pole)[1 + ((index)/LONGINBITS)] & 1ul << ((index)%LONGINBITS)))

#define DU1__SET_BIT(p,i,b) (b) ? (p)[((i)/TYPEINBITS(p[0]))] |= 1ul << ((i)%TYPEINBITS(p[0])) : \
	((p)[((i)/TYPEINBITS(p[0]))] &= ~(1ul << ((i)%TYPEINBITS(p[0]))))

#define DU1__GET_BIT(p,i) ((p)[((i)/LONGINBITS)] & 1ul << ((i)%LONGINBITS))

#else

inline void SetBit(BitArray_t  p, unsigned long i, bool b)
{
	if (i >= BitArraySize(p))
	{
		FatalError("Index %ld mimo rozsah 0..%ld", i, BitArraySize(p));
	}

	if (b)
	{
		p[1 + i/LONGINBITS] |= 1ul << (i%LONGINBITS);
	} else
	{
		p[1 + i/LONGINBITS] &= ~(1ul << (i%LONGINBITS));
	}
	return;
}

inline bool GetBit(BitArray_t p, unsigned long i)
{
	if (i >= BitArraySize(p))
	{
		FatalError("Index %ld mimo rozsah 0..%ld", i, BitArraySize(p));
	}

	return p[1 + i/LONGINBITS] & 1ul << (i%LONGINBITS);
}

inline void DU1__SET_BIT(BitArray_t p, unsigned long i, bool b)
{
	if (b)
	{
		p[1 + i/TYPEINBITS(p[0])] |= 1ul << (i%TYPEINBITS(p[0]));
	} else
	{
		p[1 + i/TYPEINBITS(p[0])] &= ~(1ul << (i%TYPEINBITS(p[0])));
	}
	return;
}

inline bool DU1__GET_BIT(BitArray_t p, unsigned long i)
{
	return p[1 + i/TYPEINBITS(p[0])] & 1ul << (i%TYPEINBITS(p[0]));
}

#endif

