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

int parse_arguments(int argc, char * argv[], long * port);
int create_listener(int * sock, struct sockaddr_in * socket_in, long * port);

int main(int argc, char *argv[])
{
  long port = 0;
  if (parse_arguments(argc, argv, &port) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  struct sockaddr_in socket_in;
  int sock; // filedescriptor pro socket
  create_listener(&sock, &socket_in, &port);

  close(sock);
  return EXIT_SUCCESS;
}

int create_listener(int * sock, struct sockaddr_in * socket_in, long * port)
{
  if ((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    { print_err("Unable to create socket"); return EXIT_FAILURE; }

  socket_in->sin_family = PF_INET;
  socket_in->sin_port = htons(*port);
  socket_in->sin_addr.s_addr = INADDR_ANY;

  if (bind(*sock, (struct sockaddr *) socket_in, sizeof(socket_in)) < 0)
    { print_err("Unable to bind socket on port %ld", *port); return EXIT_FAILURE; }

  if (listen(*sock, 5) != EXIT_SUCCESS)
    { print_err("Listening failed"); return EXIT_FAILURE; }

  return EXIT_SUCCESS;
}

int parse_arguments(int argc, char * argv[], long * port)
{
  char * endptr = NULL;
  if (argc == 2 && strcmp(argv[1], "--help") == EXIT_SUCCESS)
    { print_help(); return EXIT_SUCCESS; }
  else if (argc == 3 && strcmp(argv[1], "-p") == EXIT_SUCCESS)
    {
      *port = strtol(argv[2], &endptr, 10);
      if (*endptr != '\0')
        { print_err("Port '%s' is not a number", argv[2]); return EXIT_FAILURE; }
    }
  else
    { print_err("Please enter valid arguments"); return EXIT_FAILURE; }

  return EXIT_SUCCESS;
}
