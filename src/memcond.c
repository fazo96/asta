#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#define KEY 1234

int creaMemCond(size_t size)
{
    return shmget(KEY,size,IPC_CREAT | 0600);
}

void * agganciaMemCond(int shmid, int shflag)
{
    return shmat(shmid,(char *)0, shflag);
}

int scollegaMemCond(void *shmaddr)
{
    return shmdt(shmaddr);
}

int rimuoviMemCond(int shmid)
{
    return shmctl(shmid,IPC_RMID,NULL);
}
