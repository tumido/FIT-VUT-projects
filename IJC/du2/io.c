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
 *                  Modul pro spravu vstupu s vystupu
 * =====================================================================
 */
#include "io.h"

/*   Warning
 * ---------------------------------------------------------------------
 * - funkce pro vypsani chyboveho hlaseni na stderr
 */
void printErr(const char * msg)
{
  fprintf(stderr, "\x1B[31m%s\033[0m\n", msg);
  return;
}

/*   Ziskani slova ze vstupu
 * ---------------------------------------------------------------------
 * - ze souboru nacte slovo o dane maximalni delce
 * - preskakuje whitespace znaky
 * - po nacteni slova korektne ukonci string
 * - vraci delku slova
 */
int fgetw(char *s, int max, FILE *f)
{
  int length = 0;
  int letter;

  if (max > MAX_LENGTH) {max = MAX_LENGTH; }
  if (s == NULL || max ==0 || f == NULL || feof(f) != 0) { return 0; }

  while ((letter = fgetc(f)) != EOF && isspace(letter)) {}

  while (letter != EOF && !isspace(letter) && length < max)
  {
    s[length] = letter;
    length++;
    letter = fgetc(f);
  }
  s[length] = '\0';

  if (letter == EOF) {return EOF; }

  if (length == max)
  {
    while ((letter = fgetc(f)) != EOF && !isspace(letter)) {}
  }
  ungetc(letter, f);
  return length;
}
