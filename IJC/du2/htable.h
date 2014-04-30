/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/27/2014 10:32:59 AM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IJC - Projekt c. 2 - Priklad 2
 * =====================================================================
 */

#ifndef HTABLE_INCLUDED
#define HTABLE_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*   Struktury hashovaci tabulky
 * ---------------------------------------------------------------------
 * - htab_listitem je zaznam v hashovaci tabulce
 * - htab_t je samotna hashovaci tabulka obsahujici ukaztatele na pole
 *   dat a velikost tabulky
 */
struct htab_listitem {
	char * key;
	unsigned data;
	struct htab_listitem * next;
};

typedef struct {
	unsigned size;
	struct htab_listitem ** list;
} htab_t;

typedef void (* callback)(const char * key, unsigned int value);

/*   Inicializace hashovaci tabulky
 * ---------------------------------------------------------------------
 * - allokuje pamet pro hashovaci tabluku
 * - alokuje pole dat hashovaci tabulky
 * - ulozi jeji velikost
 * - vrati ukazatel na nove vzniklou tabulku
 */
htab_t * htab_init(unsigned size);

/*   Zavolani funkce pro kazdy prvek
 * ---------------------------------------------------------------------
 * - vola zadanou funkci typu callback pro kazdy prvek v hashovaci
 *   tabulce
 * - nemeni obsah tabulky
 */
void htab_foreach(htab_t * t, callback function);

/*   Vyhledani a zruseni zadane polozky
 * ---------------------------------------------------------------------
 * - spocita si radek tabulky, kde se pozadovany prvek nachazi
 * - pokud jej v tomto radku opravdu nalezne, provede uvolneni pameti a
 *   nastavi ukazatel next predchoziho prvku na prvek nasledujici
 */
void htab_remove(htab_t * t, const char * key);

/*   Zruseni vsech polozek v tabulce
 * ---------------------------------------------------------------------
 * - pro kazdy radek tabulky zavola rekurzivni funkci htab_clearRow
 * - ta se vola rekurzivne dokud ji neni ke sparacovali predany NULL
 *   pointer
 * - pri vynorovani provadi uvolneni pameti allokovane pro dany item
 */
void htab_clear(htab_t * t);
void htab_clearRow(struct htab_listitem * item);

/*   Zruseni tabulky
 * ---------------------------------------------------------------------
 * - provede uvolneni pameti urcene pro hashovaci tabulku
 */
void htab_free(htab_t * t);

/*   Statistika delky zaznamu v tabulce
 * ---------------------------------------------------------------------
 * - ladici funkce slouzici k vypisu statistik hashovaci tabulky
 * - vypisuje minimalni, maximalni a prumerny pocet zaznamu v radku
 */
void htab_statistics(htab_t * t);

/*   Hledani zaznamu v tabulce
 * ---------------------------------------------------------------------
 * - hleda zaznam v tabulce, pokud jej nenajde, necha vytvorit novy
 */
struct htab_listitem * htab_lookup(htab_t *t, const char *key);

/*   Vytvoreni noveho zaznamu
 * ---------------------------------------------------------------------
 * - vytvori novy zaznam pro hash tabulku a vrati ukazatel na ni
 * - zaznam obsahuje key, data = 0 a ukazatel next je NULL
 * - pokud dojde k chybe (nedostatek pameti) vraci NULL
 */
struct htab_listitem * htab_new(const char *key);

/*   Hashovaci funkce
 * ---------------------------------------------------------------------
 * - hasovaci funkce ze zadani
 */
unsigned int hash_function(const char *str, unsigned htab_size);
#endif
