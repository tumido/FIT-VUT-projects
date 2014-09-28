/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  05/02/2014 06:42:09 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IOS - Projekt c. 2
 * =====================================================================
 */

/*
 * =====================================================================
 *            Rozhrani modulu pro spravu synchronizace procesu
 * =====================================================================
 */

#ifndef SYNC_INCLUDED
#define SYNC_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include "io.h"

/*   Struktura semaforu
 * ---------------------------------------------------------------------
 */
struct semS
{
  sem_t * molo;
  sem_t * shm;
  sem_t * boardS;
  sem_t * boardH;
  sem_t * role;
  sem_t * row;
  sem_t * land;
  sem_t * finish;
};

/*   Struktura sdilene pameti
 * ---------------------------------------------------------------------
 */
struct shmS
{
  int step;
  int h;
  int idH;
  int s;
  int idS;
  int onBoard;
  int allDone;
  int pgpid;
  FILE * out;
};

/*   Vytvoreni semaforu
 * ---------------------------------------------------------------------
 */
void setSem(struct semS * sem);

/*   Vytvoreni sdilene pameti a vystupniho souboru
 * ---------------------------------------------------------------------
 */
struct shmS * setShm(int * shmid);

/*   Odemykani semaforu
 * ---------------------------------------------------------------------
 * - cyklicky odemyka semafor pro pozadovany pocet procesu
 */
void unlockSem(int times, sem_t * s);

/*   Ukonceni prace se semafory
 * ---------------------------------------------------------------------
 */
void closeSem(struct semS * sem, struct shmS * shm);

/*   Uklid sdilene pameti, semaforu a zavreni souboru
 * ---------------------------------------------------------------------
 */
void cleanShared(int * shmid);

void closeAll(struct semS * sem, struct shmS * shm, int * shmid);
/*   Inicializace osoby
 * ---------------------------------------------------------------------
 */
void start(struct semS * sem, struct shmS * shm, int * idG, const char * who, int * id);

/*   Vstup na molo
 * ---------------------------------------------------------------------
 * - pokud muze proces vstoupit na molo vstoupi a zkontroluje kolik
 *   procesu uz zde ceka
 * - pokud jich ceka dostatek na vytvoreni skupiny, prohlasi se za
 *   kapitana a necha je nastoupit na lod
 */
void enterMolo(struct semS * sem, struct shmS *shm, sem_t * boardG, sem_t * boardOG, int * g, int * og, bool * captain, const char * who, int * id);

/*   Vstup do lodi
 * ---------------------------------------------------------------------
 * - pokud je procesu dovoleno nastoupit na lod nastoupi a pripocita se
 *   k posadce
 * - pokud jsou jiz vsichni na lodi dovoli dalsi fazi
 */
void enterBoat(struct semS * sem, struct shmS * shm, sem_t * boardG, bool * captain, const char * who, int * id);

/*   Role v posadce
 * ---------------------------------------------------------------------
 * - kazdy oznami svou roli na lodi
 */
void sayRole(struct semS *sem, struct shmS * shm, bool * captain, const char * who, int * id);

/*   Projizdka lodi
 * ---------------------------------------------------------------------
 * - az vsichni reknou sve role dostanou povoleni k vypluti
 * - kapitan ridi a uspi se na dobu projizdky, pak dovoli vystupovani
 */
void rowBoat(param * p, struct semS * sem, struct shmS * shm, bool * captain, const char * who, int * id);

/*   Konec
 * ---------------------------------------------------------------------
 * - az proces vystoupi z lodi, ceka na skonceni
 * - pokud jde o posledni proces co vystoupil, dovoli ukoncovani procesu
 */
void finish(param * p, struct semS * sem, struct shmS * shm, const char * who, int * id);

#endif
