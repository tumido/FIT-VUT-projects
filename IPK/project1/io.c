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
 *                  Modul pro spravu vstupu s vystupu
 * =====================================================================
 */
#include "io.h"

/*   print_err
 * ---------------------------------------------------------------------
 * - funkce pro vypsani chyboveho hlaseni na stderr
 */
void print_err(const char * fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fputs("\x1B[31mError: \033[0m", stderr);
  vfprintf(stderr, fmt, args);
  fputs("\n", stderr);
  va_end(args);
  return;
}

void print_help()
{
  printf("usage: client -p PORT -h HOSTNAME (-l USER | -u UID) [-L –U –G –N –H –S]\n");
  printf("       server -p PORT\n\n");
  printf("author: Tomas Coufal <xcoufa09@stud.fit.vutbr.cz\n");
  exit(EXIT_SUCCESS);
}

void add_to_info(char * dst, char src)
{
  switch (src)
  {
    case 'L':
      *dst =  *dst | (1 << 5);
      break;
    case 'U':
      *dst =  *dst | (1 << 4);
      break;
    case 'G':
      *dst =  *dst | (1 << 3);
      break;
    case 'N':
      *dst =  *dst | (1 << 2);
      break;
    case 'H':
      *dst =  *dst | (1 << 1);
      break;
    case 'S':
      *dst =  *dst | 1;
      break;
    default:
      break;
  }
  return;
}

void add_to_criteria(char * dst, char * src, unsigned long bufsize)
{
  unsigned long i = strlen(dst);
  if (i == bufsize) return;
  if (i > 0) dst[i] = ' ';
  else i--;
  strncpy(&dst[i + 1], src, bufsize - i);
  dst[i + strlen(src) + 1] = '\0';
  return;
}

#ifdef DEBUG
void printDebug(const char * ftm, ...)
{
  va_list args;
  va_start(args, fmt);
  fputs("Debug: ", stderr);
  vfprintf(stderr, fmt, args);
  va_end(args);
  return;
}
#endif
