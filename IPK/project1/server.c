/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/03/2015 09:49:27 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IPK - Projekt c.1
 * =====================================================================
 */

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "io.h"
#include "socket.h"

int parse_arguments(int argc, char * argv[], long * port);
int create_listener(int * socket_invite, struct sockaddr_in * socket_in, long * port);
int accept_connection(int * socket_invite, struct sockaddr_in * socket_in);
int reply(int * socket_data);
int form_response(char * in, char * out);

/*   Main
 * ---------------------------------------------------------------------
 * - just calls functions for parsing parameters, socket creation and
 *   for the most important function for accepting connections
 */
int main(int argc, char *argv[])
{
  long port = 0;
  if (parse_arguments(argc, argv, &port) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  struct sockaddr_in socket_in;
  int sock; // filedescriptor pro socket
  if (create_listener(&sock, &socket_in, &port) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  if (accept_connection(&sock, &socket_in) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  close(sock);
  return EXIT_SUCCESS;
}

/*   Create Listener
 * ---------------------------------------------------------------------
 * - creates a socket listening on designated port
 * - using UNIX sockets
 */
int create_listener(int * socket_invite, struct sockaddr_in * socket_in, long * port)
{
  if ((*socket_invite = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    { print_err("Unable to create socket"); return EXIT_FAILURE; }

  socket_in->sin_family = PF_INET;
  socket_in->sin_port = htons(*port);
  socket_in->sin_addr.s_addr = INADDR_ANY;

  if (bind(*socket_invite, (struct sockaddr *) socket_in, sizeof(*socket_in)) < 0)
    { print_err("Unable to bind socket on port nr. '%ld'", *port); return EXIT_FAILURE; }

  if (listen(*socket_invite, 5) != EXIT_SUCCESS)
    { print_err("Listening on port '%ld' failed", *port); return EXIT_FAILURE; }

  return EXIT_SUCCESS;
}

/*  Accept connection
 * ---------------------------------------------------------------------
 * - when data arives it forks itself and child is resolving the quary
 *   (generates the respond) while parent is waiting for next connection
 * - can handle up to MAX_CONCURRENT_CONNECTIONS at the same time
 */
int accept_connection(int * socket_invite, struct sockaddr_in * socket_in)
{
  socklen_t tmp_socket_length = sizeof(socket_in);
  int socket_data = 0;
  int whoami = 0;
  while (true)
  {
    socket_data = accept(*socket_invite, (struct sockaddr *) socket_in, &tmp_socket_length);

    whoami = fork();
    if (whoami == -1) // I'm in trouble
    {
      kill(0, SIGTERM);
      exit(EXIT_FAILURE);
    }
    if (whoami == 0) // I'm my own child
    {
      close(*socket_invite);
      exit(reply(&socket_data));
    }
    else // I'm parent
    {
      close(socket_data);
    }
  }
}

int reply(int *socket_data)
{
  int len = 0;
  ioctl(*socket_data, FIONREAD, &len);
  if (len <= 0) return EXIT_FAILURE;

  char * buffer = NULL;
  if ((buffer = (char *) malloc(sizeof(char) * len) ) == NULL)
    return EXIT_FAILURE;
  read(*socket_data, buffer, len);

//  printf("Process of PID %d recieved data from client:\nmode:%c\noptions:%d\ndata:%s\n", getpid(), buffer[0], buffer[1], &buffer[2]);
//  return EXIT_SUCCESS;

  char * response = NULL;
  if (form_response(buffer, response) != EXIT_SUCCESS) return EXIT_FAILURE;

  if (write(*socket_data, response, strlen(response)) < 0)
    { print_err("Server PID(%d): Unable to send data back to client", getpid());
      return EXIT_FAILURE; }

  close(*socket_data);
  free(buffer);
  return EXIT_SUCCESS;
}

int form_response(char * in, char * out)
{
  char * endptr = NULL;
  char * input_data = &in[2];
  if (in[0] == 'u')
  {
    uid_t uid = 0;
    while (*input_data != '\0')
    {
      uid = strtol(input_data, &endptr, 10);
      printf("%d\n", uid);
      input_data = endptr;
    }
  }
  else
  {
    
  }
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
