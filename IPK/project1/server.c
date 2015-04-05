/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/03/2015 09:49:27 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IPK - Projekt c.1
 * =====================================================================
 */

#ifdef __linux__
#define _POSIX_SOURCE
#endif
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
#include <errno.h>
#include <sys/wait.h>

#include <pwd.h>

#include "io.h"
#include "socket.h"

int parse_arguments(int argc, char * argv[], long * port);
int create_listener(int * socket_invite, struct sockaddr_in * socket_in, long * port);
int accept_connection(int * socket_invite, struct sockaddr_in * socket_in);
int reply(int socket_data);
int form_response(char * in, char * out);
void get_data_for_user(char * dst, struct passwd * pw, char * options);
struct passwd * get_next_user(char ** endptr, bool uid_or_name, void * user);

void term_children_and_exit()
{
  kill(0, SIGTERM);
  while (1)
  {
    if ((wait(NULL) == -1) && (errno == ECHILD))
    break;
  }
  exit(EXIT_FAILURE);
}
void wait_for_children()
{
  waitpid(-1, NULL, WNOHANG);
}
void term_child()
{
  exit(EXIT_FAILURE);
}

/*   Main
 * ---------------------------------------------------------------------
 * - just calls functions for parsing parameters, socket creation and
 *   for the most important function for accepting connections
 */
int main(int argc, char *argv[])
{
  signal(SIGCHLD, &wait_for_children);
  signal(SIGTERM, &term_children_and_exit);
  signal(SIGINT, &term_children_and_exit);

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
      signal(SIGTERM, &term_child);
      close(*socket_invite);
      exit(reply(socket_data));
    }
    else // I'm parent
    {
      close(socket_data);
    }
  }
}

int reply(int socket_data)
{
  int len = 0;
  ioctl(socket_data, FIONREAD, &len);
  if (len <= 0) return EXIT_FAILURE;

  char * buffer = NULL;
  if ((buffer = (char *) malloc(sizeof(char) * len) ) == NULL)
    return EXIT_FAILURE;
  read(socket_data, buffer, len);

//  printf("Process of PID %d recieved data from client:\n%s\n", getpid(), buffer);

  char * response = NULL;
  if ((response = (char *) calloc(BUFSIZE, sizeof(char))) == NULL)
    return EXIT_FAILURE;

  if (form_response(buffer, response) != EXIT_SUCCESS) return EXIT_FAILURE;

  if (write(socket_data, response, strlen(response) +1 ) < 0)
    { print_err("Server PID(%d): Unable to send data back to client", getpid());
      return EXIT_FAILURE; }

  free(response);
  close(socket_data);
  free(buffer);
  return EXIT_SUCCESS;
}

int form_response(char * in, char * out)
{
  char * endptr = &in[7];
  struct passwd * pw = NULL;
  uid_t uid = 0;
  char uname[BUFSIZE];
  char * uname_ptr = &uname[0];
  char integer[sizeof(int)];

#ifdef __linux__
  while (*endptr != '\0')
#else
  while (in[0] == 'u' ? *endptr != '\0': endptr != NULL)
#endif
  {
    pw = get_next_user(&endptr, in[0] == 'u', in[0] == 'u' ? (void *) &uid : &uname_ptr);
    if (pw == NULL)
    {
      strcat(out, "\fChyba: neznamy login ");
      if(in[0] == 'u')
      {
        sprintf(integer, "%d", uid);
        strcat(out, integer);
      }
      else
        strcat(out, uname_ptr);
      strcat(out, "\n\f");
    }
    else
    {
      get_data_for_user(out, pw, &in[1]);
      strcat(out, "\n");
    }
  }
  return EXIT_SUCCESS;
}
struct passwd * get_next_user(char ** endptr, bool uid_or_name, void * user)
{
  struct passwd * pw;
  if (uid_or_name)
  {
    *(uid_t *) user = strtol(*endptr, endptr, 10);
    return getpwuid(*(uid_t *) user);
  }
  else
  {
    *(char **) user = strtok_r(*endptr, " ",endptr);
    pw = getpwnam(*(char **) user);
    return pw;
  }
  return NULL;
}

void get_data_for_user(char * dst, struct passwd * pw, char * options)
{
  bool c = true;
  char integer[sizeof(int)];

  for (int i = 0; i < 6; i++)
  {
    if  (options[i] == '\0')
      break;
    if (c) c = false; else strcat(dst , " ");
    switch (options[i])
    {
      case 'L':
        strcat(dst, pw->pw_name);
        break;
      case 'U':
        sprintf(integer, "%d", pw->pw_uid);
        strcat(dst, integer);
        break;
      case 'G':
        sprintf(integer, "%d", pw->pw_gid);
        strcat(dst, integer);
        break;
      case 'N':
        strcat(dst, pw->pw_gecos);
        break;
      case 'H':
        strcat(dst, pw->pw_dir);
        break;
      case 'S':
        strcat(dst, pw->pw_shell);
        break;
    }
  }
  return;
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
