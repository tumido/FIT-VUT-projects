/*
 * =====================================================================
 *          Verze:  1.0
 *      Vytvoreno:  05/02/2014 07:01:45 PM
 *          Autor:  Tomáš Coufal
 *          Login:  xcoufa09
 *        Projekt:  IOS - Projekt c. 2
 * =====================================================================
 */

/*
 * =====================================================================
 *                Modul pro spravu synchronizace procesu
 * =====================================================================
 */

#include "sync.h"

/*   Vytvoreni semaforu
 * ---------------------------------------------------------------------
 */
void setSem(struct semS * sem)
{
  sem->molo = sem_open("/xcoufa09-semMolo", O_CREAT | O_EXCL, 0644, 0);
  sem->shm = sem_open("/xcoufa09-semShm", O_CREAT | O_EXCL, 0644, 0);
  sem->boardS = sem_open("/xcoufa09-semBoardSerf", O_CREAT | O_EXCL, 0644, 0);
  sem->boardH = sem_open("/xcoufa09-semBoardHacker", O_CREAT | O_EXCL, 0644, 0);
  sem->role = sem_open("/xcoufa09-semRole", O_CREAT | O_EXCL, 0644, 0);
  sem->row = sem_open("/xcoufa09-semRow", O_CREAT | O_EXCL, 0644, 0);
  sem->land = sem_open("/xcoufa09-semLand", O_CREAT | O_EXCL, 0644, 0);
  sem->finish = sem_open("/xcoufa09-semFinish", O_CREAT | O_EXCL, 0644, 0);
}

/*   Vytvoreni sdilene pameti a vystupniho souboru
 * ---------------------------------------------------------------------
 */
struct shmS * setShm(int * shmid)
{
  *shmid = shm_open("/xcoufa09-shm", O_CREAT | O_EXCL | O_RDWR, 0644);
  ftruncate(*shmid, sizeof(struct shmS));
  struct shmS * shm = mmap(NULL, sizeof(struct shmS), PROT_READ | PROT_WRITE, MAP_SHARED, *shmid, 0);
  return shm;
}

/*   Odemykani semaforu
 * ---------------------------------------------------------------------
 * - cyklicky odemyka semafor pro pozadovany pocet procesu
 */
void unlockSem(int times, sem_t * s)
{
  for (int i=0; i < times; i++)
  {
    sem_post(s);
  }
}

/*   Ukonceni prace se semafory
 * ---------------------------------------------------------------------
 */
void closeSem(struct semS * sem, struct shmS * shm)
{
  sem_close(sem->shm);
  sem_close(sem->molo);
  sem_close(sem->boardS);
  sem_close(sem->boardH);
  sem_close(sem->role);
  sem_close(sem->row);
  sem_close(sem->land);
  sem_close(sem->finish);
  munmap(shm, sizeof(struct shmS));
}

/*   Uklid sdilene pameti, semaforu a zavreni souboru
 * ---------------------------------------------------------------------
 */
void cleanShared(int * shmid)
{
  sem_unlink("/xcoufa09-semMolo");
  sem_unlink("/xcoufa09-semShm");
  sem_unlink("/xcoufa09-semBoardSerf");
  sem_unlink("/xcoufa09-semBoardHacker");
  sem_unlink("/xcoufa09-semRole");
  sem_unlink("/xcoufa09-semRow");
  sem_unlink("/xcoufa09-semLand");
  sem_unlink("/xcoufa09-semFinish");

  shm_unlink("/xcoufa09-shm");
  close(*shmid);
}

void closeAll(struct semS * sem, struct shmS * shm, int * shmid)
{
  fclose(shm->out);
  closeSem(sem, shm);
  cleanShared(shmid);
}
/*   Inicializace osoby
 * ---------------------------------------------------------------------
 */
void start(struct semS * sem, struct shmS * shm, int * idG, const char * who, int * id)
{
  sem_wait(sem->shm);
  *id = *idG;
  fprintf(shm->out, "%d: %s: %d: started\n", shm->step, who, *id);
  (*idG)++;
  shm->step++;
  sem_post(sem->shm);
}

/*   Vstup na molo
 * ---------------------------------------------------------------------
 * - pokud muze proces vstoupit na molo vstoupi a zkontroluje kolik
 *   procesu uz zde ceka
 * - pokud jich ceka dostatek na vytvoreni skupiny, prohlasi se za
 *   kapitana a necha je nastoupit na lod
 */
void enterMolo(struct semS * sem, struct shmS *shm, sem_t * boardG, sem_t * boardOG, int * g, int * og, bool * captain, const char * who, int * id)
{
  sem_wait(sem->molo);
  sem_wait(sem->shm);
  (*g)++;
  fprintf(shm->out, "%d: %s: %d: waiting for boarding: %d: %d\n", shm->step, who, *id, shm->h, shm->s);
  shm->step++;
  if (*g == 4)
  {
    *g -= 4;
    unlockSem(3, boardG);
    *captain = true;
  } else if (*g == 2 && *og >= 2)
  {
    shm->h -= 2;
    shm->s -= 2;
    unlockSem(2, boardOG);
    unlockSem(1, boardG);
    *captain = true;
  } else
  {
    sem_post(sem->molo);
  }
  sem_post(sem->shm);
}

/*   Vstup do lodi
 * ---------------------------------------------------------------------
 * - pokud je procesu dovoleno nastoupit na lod nastoupi a pripocita se
 *   k posadce
 * - pokud jsou jiz vsichni na lodi dovoli dalsi fazi
 */
void enterBoat(struct semS * sem, struct shmS * shm, sem_t * boardG, bool * captain, const char * who, int * id)
{
  if (!(*captain)) { sem_wait(boardG); }
  sem_wait(sem->shm);
  fprintf(shm->out, "%d: %s: %d: boarding: %d: %d\n", shm->step, who, *id, shm->h, shm->s);
  shm->step++;
  shm->onBoard++;
  if (shm->onBoard == 4)
  {
    unlockSem(4, sem->role);
  }
  sem_post(sem->shm);
}

/*   Role v posadce
 * ---------------------------------------------------------------------
 * - kazdy oznami svou roli na lodi
 */
void sayRole(struct semS * sem, struct shmS * shm, bool * captain, const char * who, int * id)
{
  sem_wait(sem->role);
  sem_wait(sem->shm);
  if (*captain)
  {
    fprintf(shm->out, "%d: %s: %d: captain\n", shm->step, who, *id);
  } else
  {
    fprintf(shm->out, "%d: %s: %d: member\n", shm->step, who, *id);
  }
  shm->step++;
  shm->onBoard++;
  sem_post(sem->shm);
}

/*   Projizdka lodi
 * ---------------------------------------------------------------------
 * - az vsichni reknou sve role dostanou povoleni k vypluti
 * - kapitan ridi a uspi se na dobu projizdky, pak dovoli vystupovani
 */
void rowBoat(param * p, struct semS * sem, struct shmS * shm, bool * captain, const char * who, int * id)
{
  sem_wait(sem->shm);
  if (shm->onBoard == 8) { sem_post(sem->row); }
  sem_post(sem->shm);

  if (*captain)
  {
    sem_wait(sem->row);
    srand(time(NULL) * getpid());
    usleep(rand()%(p->paramS.R+1)*1000);
    unlockSem(4, sem->land);
  }

  sem_wait(sem->land);
  sem_wait(sem->shm);
  shm->onBoard++;
  fprintf(shm->out, "%d: %s: %d: landing: %d: %d\n", shm->step, who, *id, shm->h, shm->s);
  shm->step++;
  shm->allDone++;
  sem_post(sem->shm);
}

/*   Konec
 * ---------------------------------------------------------------------
 * - az proces vystoupi z lodi, ceka na skonceni
 * - pokud jde o posledni proces co vystoupil, dovoli ukoncovani procesu
 */
void finish(param * p, struct semS * sem, struct shmS * shm, const char * who, int * id)
{
  sem_wait(sem->shm);
  if( shm->allDone == p->paramS.P * 2)
  {
    sem_post(sem->finish);
  }
  sem_post(sem->shm);
  sem_wait(sem->finish);
  sem_wait(sem->shm);
  fprintf(shm->out, "%d: %s: %d: finished\n", shm->step, who, *id);
  shm->step++;
  sem_post(sem->shm);
  sem_post(sem->finish);
}
