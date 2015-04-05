/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/04/2015 02:19:34 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IPK - Projekt c.1
 * =====================================================================
 */
#ifndef SOCKET_INCLUDED
#define SOCKET_INCLUDED

#define BUFSIZE 2048
#define BUFSIZE_SMALL 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct message
{
  char mode; // mode 'u' for UID or 'l' for login
  char data[BUFSIZE]; // list of UIDs or logins
  char options[7];
  // each char represents one option: L U G N H S
  //                                  | | | | | +- shell
  //                                  | | | | +- home folder
  //                                  | | | +- gecos
  //                                  | | +- GID
  //                                  | +- UID
  //                                  +- username
};

struct keep_data
{
  char host [BUFSIZE_SMALL];
  long port;
  struct message msg;
};

#endif
