/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  05/02/2014 07:05:52 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IOS - Projekt c. 2
 * =====================================================================
 */

#ifndef CREW_INCLUDED
#define CREW_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include "sync.h"
#include "io.h"

void hacker(param * p, struct semS * sem, struct shmS * shm);
void serf(param * p, struct semS * sem, struct shmS * shm);

#endif
