/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/03/2015 09:49:27 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IPK - Projekt c.1
 * =====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#include "io.h"
#include "socket.h"

int main(int argc, char *argv[])
{
  long port = 0;
  char * endptr = NULL;
  if (argc == 2 && strcmp(argv[1], "--help") == EXIT_SUCCESS)
    { print_help(); return EXIT_SUCCESS; }
  else if (argc == 3 || strcmp(argv[1], "-p") == EXIT_SUCCESS)
    {
      port = strtol(argv[2], &endptr, 10);
      if (*endptr != '\0')
        { print_err("Port is not a number"); return EXIT_FAILURE; }
    }
  else
    { print_err("Wrong argument"); return EXIT_FAILURE; }

  

  return EXIT_SUCCESS;
}
