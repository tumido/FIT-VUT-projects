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

#include <arpa/inet.h>
#include <ctype.h>
#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
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


int main (int argc, char * argv[])
{
  struct message data;
  if (parse_arguments(argc, argv, &data) != EXIT_SUCCESS)
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
      msg = "parameter should start with '-'";
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
