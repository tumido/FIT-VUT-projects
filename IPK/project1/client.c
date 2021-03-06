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
#include <ctype.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>

#include "io.h"
#include "socket.h"

#define TIMEOUT 10

/*
 *   client -> server:
 * ---------------------------------------------------------------------
 * mode: u|l
 * data_format: L N G...
 * mode_data: usernames | UIDs
 *
 *   server -> client:
 * ---------------------------------------------------------------------
 * data stream -> directly to stdout
 * stderr parts are separated by '\f' delimiters
 */

/*
 *  Prototypes
 * ---------------------------------------------------------------------
 */

int parse_arguments(int argc, char * argv[], struct keep_data * data);
int establish_connection(struct keep_data * data, int * sock);
int send_data(struct message * msg_to_send, int * sock);
int get_respose(int * sock, char ** buffer);
void print_result(char * buffer);

/*
 *   Main
 * ---------------------------------------------------------------------
 *  - just a wrapper over the inpotrant functions, as ussual
 */
int main (int argc, char * argv[])
{
  struct keep_data data;
  if (parse_arguments(argc, argv, &data) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  int sock;
  if (establish_connection(&data, &sock) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  if (send_data(&data.msg, &sock) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  char * buffer = NULL;
  if (get_respose(&sock, &buffer) != EXIT_SUCCESS)
    return EXIT_FAILURE;

  print_result(buffer);

  close(sock);
  return EXIT_SUCCESS;
}

/*
 *   Establish connection
 * ---------------------------------------------------------------------
 * - function responsible for creating socket and connection to distant
 *   host
 * - takes parsed data like hostame and port nr. and hooks socket to them
 */
int establish_connection(struct keep_data * data, int * sock)
{
  struct sockaddr_in socket_in;
  struct hostent * host_ptr;

  if ((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    { print_err("Unable to create socket"); return EXIT_FAILURE; }

  socket_in.sin_family = PF_INET;
  socket_in.sin_port = htons(data->port);

  if ((host_ptr = gethostbyname(data->host)) == NULL)
    { print_err("Host '%s' not found", data->host); return EXIT_FAILURE; }

  memcpy(&socket_in.sin_addr, host_ptr->h_addr_list[0], host_ptr->h_length);

  if (connect(*sock, (struct sockaddr *) &socket_in, sizeof(socket_in)) < 0)
    { print_err("Unable to connect to '%s:%ld'", data->host, data->port); return EXIT_FAILURE; }

  return EXIT_SUCCESS;
}

/*
 *   Send Data
 * ---------------------------------------------------------------------
 * - as the name implicates, this transforms data in structure to
 *   message that is sent to the server
 * - data are in this order MOD[1]E | OPTIONS[6] | DATA[n]
 */
int send_data(struct message * msg_to_send, int * sock)
{
  char * msg = NULL;
  if ((msg = (char *) malloc(sizeof(char) * BUFSIZE + sizeof(int))) == NULL)
    // first CHARs are mode (u|l) and options
    return EXIT_FAILURE;
  msg[0] = msg_to_send->mode;
  strcpy(&msg[1], msg_to_send->options);

  for (int i = 0; i < BUFSIZE && msg_to_send->data[i] !='\0' ; i++)
    msg_to_send->data[i] = tolower(msg_to_send->data[i]);

  strcpy(&msg[7], msg_to_send->data);

  int len = strlen(&msg[7]) + 7; // 7 command bytes on top (made and options)

  if (write(*sock, msg, len+1) < 0)
    { free(msg); return EXIT_FAILURE; }
  free(msg);
  return EXIT_SUCCESS;
}

/*
 *   Get Response
 * ---------------------------------------------------------------------
 * - listens for the response for some time - limited by TIMEOUT
 * - pushes data to a buffer, doesn't care what data it is
 */
int get_respose(int * sock, char ** buffer)
{
  int len = 0;
  int countdown = TIMEOUT;
  while (!len && countdown > 0 && ioctl(*sock, FIONREAD, &len) >= 0)
    {countdown--; sleep(1); }
  
  if (len <= 0) { print_err("No reply recieved"); return EXIT_FAILURE; }

  if ((*buffer = (char *) malloc(sizeof(char) * len) ) == NULL)
    return EXIT_FAILURE;


  if (read(*sock, *buffer, len) < 0)
    { print_err("Failed to read respond"); return EXIT_FAILURE; }
  return EXIT_SUCCESS;
}

/*  Parse Arguments
 * ---------------------------------------------------------------------
 * - a huge argument parser
 * - has to deal with all argument acording to synopsis
 * - OPTIONS can be given both long and shor forms
 *   CRITERIA is not limited by length, just UIDs has to be numbers
 *   HOST and PORT has to be parsed also
 */
int parse_arguments(int argc, char * argv[], struct keep_data * data)
{
  bool valid = true;
  bool value = false;
  char * endptr = NULL;
  data->host[0] = data->msg.mode = data->msg.data[0] = '\0';
  for (int i = 0; i < (int) sizeof(data->msg.options); i++) data->msg.options[i] = '\0';
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
          if (*endptr != '\0') { valid = false; msg = "not a port number"; }
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
          mode = data->msg.mode = argv[i][1];
          for (unsigned long k = 0; k < BUFSIZE; k++)
            data->msg.data[k] = '\0';
          while (i < argc - 1 && argv[i + 1][0] != '-')
          {
            if (mode == 'u')
            {
              strtol(argv[i+1], &endptr, 10);
              if (*endptr != '\0')
                { valid = false; msg = "UID is not a number"; break; }
            }
            add_to_criteria(data->msg.data, argv[i+1], sizeof(data->msg.data));
            i++;
          }
        }
        break;
      case 'L': case 'U': case 'G': case 'N': case 'H': case 'S': // data
        add_to_info(&data->msg.options[0], argv[i][1]);
        for (int j = 2; argv[i][j] != '\0'; j++)
        {
          switch (argv[i][j])
          {
            case 'L': case 'U': case 'G': case 'N': case 'H': case 'S': // data
              add_to_info(&data->msg.options[0], argv[i][j]);
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

/*
 *  Print Result
 * ---------------------------------------------------------------------
 * - print function that checkes the data and when it caches the control
 *   character to swithc between STDOUT and STDERR it does so
 */
void print_result(char * buffer)
{
  FILE * current_fd = stdout;
  for(unsigned long i = 0; buffer[i] != '\0'; i++)
  {
    if (buffer[i] == '\f')
      current_fd = (current_fd == stdout) ? stderr : stdout;
    else
      fputc(buffer[i], current_fd);
  }
}
