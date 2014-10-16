/* SemUtils.c */


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


int semwait(int semid)	
	/* operazione wait (ROSSO) sul semaforo numero_sem */
{
  struct sembuf sb;

  sb.sem_num=0; /* n° semaforo nel SET */
  sb.sem_op=-1; /* ROSSO */
  sb.sem_flg=0;
  return semop(semid,&sb,1); /* operazione sul semaforo */
}



int semsignal(int semid)	
	/* operazione signal (VERDE) sul semaforo numero_sem */
{
  struct sembuf sb;

  sb.sem_num=0; /* n° semaforo nel SET */
  sb.sem_op=1; /* VERDE */
  sb.sem_flg=0;
  return semop(semid,&sb,1); /* operazione sul semaforo */
}

int seminit(int semid, int initval)
	/* inizializzazione a initval di numero_sem */
{
  union semun
  {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
  } arg;
  arg.val=initval;
  return semctl(semid,0,SETVAL,arg);
}

int semcreate()
{
	    return semget(IPC_PRIVATE,1,0600);
}

void semdestroy(int semid)
{
	semctl(semid,0,IPC_RMID);
}

