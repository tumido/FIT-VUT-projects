/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  05/02/2014 08:16:07 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IOS - Projekt c. 2
 * =====================================================================
 */

/*
 * =====================================================================
 *                       Modul pro vstup a vystup
 * =====================================================================
 */

#include "io.h"

/*   Zpracovani parametru
 * ---------------------------------------------------------------------
 * - zpracuje parametry a overi meze
 */
int paramHandle(param * p, int * argc, char * argv[])
{
  char * end;

  if (*argc != 5)
  {
    printErr("Number of parameters doesn't match.");
    return EXIT_ARG_WRONG;
  }

  for (int i = 1; i < *argc; i++)
  {
    p->member[i-1] = strtol(argv[i], &end, 0);
    if(*end) { printErr("Argument is not a number."); return EXIT_ARG_WRONG; }
    if ((i > 1)  && !((p->member[i-1] >= 0) && (p->member[i-1] < 5001)))
    { printErr("Argument is not valid."); return EXIT_ARG_WRONG; }
  }

  if (!((p->paramS.P > 0) && (p->paramS.P % 2 == 0)))
  { printErr("Argument is not valid."); return EXIT_ARG_WRONG; }
  return EXIT_SUCCESS;
}

/*   Vypis erroroveho hlaseni na stderr
 * ---------------------------------------------------------------------
 */
void printErr(const char * msg)
{
  fprintf(stderr, "\x1B[31m%s\033[0m\n", msg);
  return;
}

void sigtermHandler()
{
  kill(0, SIGUSR1);
  exit(EXIT_SYSCALL_ERR);
}
