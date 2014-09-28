/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  05/02/2014 07:09:05 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IOS - Projekt c. 2
 * =====================================================================
 */

#include "crew.h"

void hacker(param * p, struct semS * sem, struct shmS * shm)
{
  signal(SIGUSR1, sigtermHandler);
//start
  char * whoami = "hacker";
  int id;
  start(sem, shm, &(shm->idH), whoami, &id);
//vstup na molo
  bool captain = false;
  enterMolo(sem, shm, sem->boardH, sem->boardS, &(shm->h), &(shm->s), &captain, whoami, &id);
//vstup na palubu
  enterBoat(sem, shm, sem->boardH, &captain, whoami, &id);
//vypis role
  sayRole(sem, shm, &captain, whoami, &id);
// plavba
  rowBoat(p, sem, shm, &captain, whoami, &id);
//otevreni mola
  sem_wait(sem->shm);
  if (shm->onBoard == 12) { shm->onBoard = 0; sem_post(sem->molo);}
  sem_post(sem->shm);
//finish
  finish(p, sem, shm, "hacker", &id);
  fclose(shm->out);
  closeSem(sem, shm);
  exit(EXIT_SUCCESS);
}

void serf(param * p, struct semS * sem, struct shmS * shm)
{
  signal(SIGUSR1, sigtermHandler);
//start
  char * whoami = "serf";
  int id;
  start(sem, shm, &(shm->idS), whoami, &id);
//vstup na molo
  bool captain = false;
  enterMolo(sem, shm, sem->boardS, sem->boardH, &(shm->s), &(shm->h), &captain, whoami, &id);
//vstup na palubu
  enterBoat(sem, shm, sem->boardS, &captain, whoami, &id);
//vypis role
  sayRole(sem, shm, &captain, whoami, &id);
// plavba
  rowBoat(p, sem, shm, &captain, whoami, &id);
//otevreni mola
  sem_wait(sem->shm);
  if (shm->onBoard == 12) { shm->onBoard = 0; sem_post(sem->molo);}
  sem_post(sem->shm);
//finish
  finish(p, sem, shm, "serf", &id);
  fclose(shm->out);
  closeSem(sem, shm);
  exit(EXIT_SUCCESS);
}
