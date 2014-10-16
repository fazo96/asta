#include <stdio.h>
#include "Messaggio.c"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	int i=0,c=0,pid=0,id_sem1,id_sem2;
	id_sem1=semcreate();
	id_sem2=semcreate();
	seminit(id_sem1,1);
	seminit(id_sem2,0);

	pid=fork();

	if(pid==0)
	{
		/*FIGLIO*/
		for(i=0;i<100;i++)
		{
		semwait(id_sem1);
		printf("**PING**\n");
		fflush(stdout);
		semsignal(id_sem2);
		}
	exit(0);
	}
	else
	{
		/*PADRE*/
		for(c=0;c<100;c++){
		semwait(id_sem2);
		printf("**PONG**\n");
		fflush(stdout);
		semsignal(id_sem1);
		}
		
		wait(NULL);
		semdestroy(id_sem1);
		semdestroy(id_sem2);
		
	}
	
}

