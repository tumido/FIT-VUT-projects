/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  05/02/2014 06:38:17 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IOS - Projekt c. 2
 * =====================================================================
 */

/*
 * =====================================================================
 *                Modul pro generatory hackeru a serfu
 * =====================================================================
 */
#include "generator.h"

/*   Generator hackeru
 * ---------------------------------------------------------------------
 * - po uplynuti nahodne doby z intervalu <0, H> vytvori proces hackera
 * - vytvori P procesu hackera
 */
void hackerGen(param * p, struct semS * sem, struct shmS * shm)
{
  signal(SIGUSR1, sigtermHandler);
  for (int i=0; i < p->paramS.P ; i++)
  {
    srand(time(NULL) * getpid());
    usleep(rand()%(p->paramS.H+1)*1000);
    pid_t hackerPid = fork();
    if (hackerPid == 0)
    {
      setpgrp();
      hacker(p, sem, shm);
    } else if (hackerPid == -1)
    {
      killpg(shm->pgpid, SIGUSR1);
      sigtermHandler();
    }
  }
  fclose(shm->out);
  closeSem(sem, shm);
  exit(EXIT_SUCCESS);
}

/*   generator serfu
 * ---------------------------------------------------------------------
 * - analogicka funkce pro serfy
 */
void serfGen(param * p, struct semS * sem, struct shmS * shm)
{
  signal(SIGUSR1, sigtermHandler);
  for (int i=0; i < p->paramS.P ; i++)
  {
    srand(time(NULL) * getpid());
    usleep(rand()%(p->paramS.S+1)*1000);
    pid_t serfPid = fork();
    if (serfPid == 0)
    {
      setpgrp();
      serf(p, sem, shm);
    } else if (serfPid == -1)
    {
      killpg(shm->pgpid, SIGUSR1);
      sigtermHandler();
    }
  }
  fclose(shm->out);
  closeSem(sem, shm);
  exit(EXIT_SUCCESS);
}
