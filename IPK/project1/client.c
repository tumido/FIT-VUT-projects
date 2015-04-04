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

/*
 *   client -> server:
 * ---------------------------------------------------------------------
 * mode: u|l
 * mode_data: usernames | UIDs
 * data_format: L N G...
 *
 *   server -> client:
 * ---------------------------------------------------------------------
 * data stream -> directly to stdout
 */


int parse_arguments(int argc, char * argv[], struct message * data);
int establish_connection(struct message * data, int * sock);
int send_data(struct message * data, int * sock);
int get_respond();

int main (int argc, char * argv[])
{
  struct message data;
  if (parse_arguments(argc, argv, &data) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  int sock;
  if (establish_connection(&data, &sock) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  send_data(&data, &sock);

  return EXIT_SUCCESS;
}

int establish_connection(struct message * data, int * sock)
{
  struct sockaddr_in socket_in;
  struct hostent * host_ptr;

  if ((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    { print_err("Unable to create socket"); return EXIT_FAILURE; }

  socket_in.sin_family = PF_INET;
  socket_in.sin_port = htons(data->port);

  if ((host_ptr = gethostbyname(data->host)) == NULL)
    { print_err("Host not found"); return EXIT_FAILURE; }

  memcpy(&socket_in.sin_addr, host_ptr->h_addr_list[0], host_ptr->h_length);

  if (connect(*sock, (struct sockaddr *) &socket_in, sizeof(socket_in)) < 0)
    { print_err("Unable to connect"); return EXIT_FAILURE; }

  return EXIT_SUCCESS;
}

int send_data(struct message * data, int * sock)
{
  char * msg = NULL;
  if ((msg = (char *) malloc(sizeof(char) * BUFSIZE)) == NULL) // +5 pro oddelovace
    return EXIT_FAILURE;
  msg[0] = data->criteria;
  msg[1] = '\n';
  strncpy(&msg[2], data->criteria_data, sizeof(char) * BUFSIZE - strlen(msg));
  msg[strlen(msg)] = '\n';
  strncpy(&msg[strlen(msg)], \
          data->info, \
          sizeof(char) * BUFSIZE - strlen(msg));

  if (write(*sock, msg, strlen(msg)+1) < 0)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int parse_arguments(int argc, char * argv[], struct message * data)
{
  bool valid = true;
  bool value = false;
  char * endptr = NULL;
  data->host[0] = data->criteria = data->criteria_data[0] = data->info[0] = '\0';
  data->port = -1;
  char mode = '\0';
  char * msg = "syntax si not valid";

  if (argc == 2 && strcmp(argv[1], "--help") == EXIT_SUCCESS) { print_help(); return EXIT_SUCCESS;}

  for (int i = 1; i < argc; i++)
  {
    if (value) {value = false; continue;} // pokud zpracovavam hodnotu, preskakuju
    if (argv[i][0] != '-' || argv[i][1] == '\0') // volba musi zacinat '-' a nesmi byt prazdna
    {
      valid = false;
      msg = "parameter should start with '-' and should contain some option";
      break;
    }
    switch (argv[i][1])
    {
      case 'p': // port
        if (argv[i][2] != '\0') { valid = false; msg = "unknown parameter"; }
        else if (data->port != -1) { valid = false; msg = "port set more than once"; }
        else if (i + 1 >= argc || argv[i + 1][0] == '-')
          { valid = false; msg = "port not specified or valid"; }
        else
        {
          data->port = strtol(argv[i +1], &endptr, 10);
          if (*endptr != '\0') { valid = false; printf("%s", endptr); msg = "not a port number"; }
          value = true;
        }
        break;
      case 'h': // hostname
        if (argv[i][2] != '\0') { valid = false; msg = "unknown parameter"; }
        else if (data->host[0] != '\0') { valid = false; msg = "hostname set more than once"; }
        else if (i + 1 >= argc || argv[i + 1][0] == '-')
          { valid = false; msg = "hostname not specified or valid"; }
        else
        {
          strncpy(data->host, argv[i + 1], sizeof(data->host));
          value = true;
        }
        break;
      case 'u': //uid
      case 'l': //login
        if (argv[i][2] != '\0') { valid = false; msg = "unknown parameter"; }
        else if (i + 1 >= argc || argv[i + 1][0] == '-')
          { valid = false; msg = "criteria not specified or valid"; }
        else
        {
          mode = data->criteria = argv[i][1];
          for (unsigned long k = 0; k < sizeof(data->criteria_data); k++)
            data->criteria_data[k] = '\0';
          while (i < argc - 1 && argv[i + 1][0] != '-')
          {
            if (mode == 'u')
            {
              strtol(argv[i+1], &endptr, 10);
              if (*endptr != '\0')
                { valid = false; msg = "UID is not a number"; break; }
            }
            add_to_criteria(data->criteria_data, argv[i+1], sizeof(data->criteria_data));
            i++;
          }
        }
        break;
      case 'L': case 'U': case 'G': case 'N': case 'H': case 'S': // data
        add_to_info(&(data->info[0]), sizeof(data->info), argv[i][1]);
        for (int j = 2; argv[i][j] != '\0'; j++)
        {
          switch (argv[i][j])
          {
            case 'L': case 'U': case 'G': case 'N': case 'H': case 'S': // data
              add_to_info(&(data->info[0]), sizeof(data->info), argv[i][j]);
              break;
            default:
              valid = false;
              msg = "unknown parameter";
          }
        }
        break;
      default:
        valid = false;
        msg = "unknown parameter";
    }
    if (!valid) break; // pokud zpracovavam hodnotu ci neni parametr validni, nezajima me
  }

  if (!valid || data->host[0] == '\0' || data->port == -1) // vypis chyby
  {
    print_err("Unable to parse arguments: %s", msg);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
