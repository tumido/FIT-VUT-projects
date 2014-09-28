/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  04/30/2014 02:27:57 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IOS - Projekt c. 2
 * =====================================================================
 */

/*
 * ====================================================================
 *                       Rivercrossing Problem
 * ====================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include "generator.h"
#include "sync.h"
#include "crew.h"
#include "io.h"

int main (int argc, char * argv[])
{
//handler na sigterm
  signal(SIGUSR1, SIG_IGN);
//parametry
  param p;
  if (paramHandle(&p, &argc, argv) == EXIT_ARG_WRONG) { return EXIT_ARG_WRONG; }
/*   Hlavni cast procesu
 * ---------------------------------------------------------------------
 */
//semafory a sdilena pamet
  int shmid;
  struct shmS * shm = setShm(&shmid);
  if (shm == (void *)-1) { printErr("Unable to create shared memory."); return EXIT_SYSCALL_ERR; }
  struct semS sem;
  setSem(&sem);
  shm->out = fopen("rivercrossing.out", "w");
  if (shm->out == NULL) { printErr("Unable to open output file."); closeSem(&sem, shm); cleanShared(&shmid); return EXIT_SYSCALL_ERR; }
  shm->h = shm->s = shm->onBoard = shm->allDone = 0;
  shm->step = shm->idH = shm->idS = 1;
  setbuf(shm->out, NULL);

  shm->pgpid = getpgid(0);
// fork
  pid_t hackerGenPid = fork();
  if (hackerGenPid == 0)
  {
    setpgrp();
    hackerGen(&p, &sem, shm);
  }
  else if (hackerGenPid == -1 )
  {
    printErr("Facing error, cleaning and exiting with code 2...");
    killpg(shm->pgpid, SIGUSR1);
    sigtermHandler();
  }
  else
  {
    pid_t serfGenPid = fork();
    if (serfGenPid == 0)
    {
      setpgrp();
      serfGen(&p,&sem, shm);
    }
    else if (hackerGenPid == -1 )
    {
      printErr("Facing error, cleaning and exiting with code 2...");
      killpg(shm->pgpid, SIGUSR1);
      sigtermHandler();
    }
    else
    {
      sem_post(sem.shm);
      sem_post(sem.molo);
    }
  }
  int status;
  bool cleanOnErr = false;
  for (int i = 0; i < p.paramS.P * 2 + 2; i++)
  {
    wait(&status);
    if (WEXITSTATUS(status) == EXIT_SYSCALL_ERR) { cleanOnErr = true; }
  }
// uklid
  closeAll(&sem , shm, &shmid);
  if (cleanOnErr)
  {
    printErr("Facing error, cleaning and exiting with code 2...");
    killpg(shm->pgpid, SIGUSR1);
    sigtermHandler();
  }
  return EXIT_SUCCESS;
}
