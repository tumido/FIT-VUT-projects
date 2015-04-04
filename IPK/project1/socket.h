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
#define BUFSIZE_HALF 1024
#define BUFSIZE_HOST 256
#define BUFSIZE_SMALL 512 + 256
struct message
{
  char host [BUFSIZE_HOST];
  long port;
  char criteria;
  char criteria_data [BUFSIZE_SMALL];
  char info [BUFSIZE_HALF];
};
#endif
