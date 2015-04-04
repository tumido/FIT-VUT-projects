/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/04/2015 02:19:34 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IPK - Projekt c.1
 * =====================================================================
 */

struct message
{
  char host [256];
  long port;
  char criteria;
  char criteria_data [256];
  char info [1024];
};
