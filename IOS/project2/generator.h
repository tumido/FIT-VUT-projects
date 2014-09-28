/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  05/02/2014 06:38:47 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IOS - Projekt c. 2
 * =====================================================================
 */

/*
 * =====================================================================
 *            Rozhrani modulu pro generatory hackeru a serfu
 * =====================================================================
 */
#ifndef GENERATOR_INCLUDED
#define GENERATOR_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include "sync.h"
#include "crew.h"
#include "io.h"

/*   Generator hackeru
 * ---------------------------------------------------------------------
 * - po uplynuti nahodne doby z intervalu <0, H> vytvori proces hackera
 * - vytvori P procesu hackera
 */
void hackerGen(param * p, struct semS * sem, struct shmS * shm);

/*   generator serfu
 * ---------------------------------------------------------------------
 * - analogicka funkce pro serfy
 */
void serfGen(param * p, struct semS * sem, struct shmS * shm);
#endif
