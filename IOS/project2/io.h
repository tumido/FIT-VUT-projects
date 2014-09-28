/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  05/02/2014 08:16:12 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IOS - Projekt c. 2
 * =====================================================================
 */

/*
 * =====================================================================
 *                    Rozhrani modulu na IO akce
 * =====================================================================
 */

#ifndef IO_INCLUDED
#define IO_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#define EXIT_SUCCESS 0
#define EXIT_ARG_WRONG 1
#define EXIT_SYSCALL_ERR 2


/*   Struktura pro jednoduchou praci s parametry
 * ---------------------------------------------------------------------
 */
typedef union paramU
{
  struct
  {
    int P, H, S, R;
  } paramS;
  int member[4];
} param;

/*   Zpracovani parametru
 * ---------------------------------------------------------------------
 * - zpracuje parametry a overi meze
 */
int paramHandle(param * p, int * argc, char * argv[]);

/*   Vypis erroroveho hlaseni na stderr
 * ---------------------------------------------------------------------
 */
void printErr(const char * msg);

void sigtermHandler();
#endif
