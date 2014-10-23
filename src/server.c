//
// server
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/times.h>
#include "messaggio.c"
#include "memcond.c"

#define SERVER_PORT 1313          // numero di porta del server

int main (int argc, char *argv[]) {
  printf("0\n");
    int socketfd, fd, dato, sem, mcid;
    unsigned int ct_l;
    struct sockaddr_in servizio,ct;   //  record con gli indirizzi
                                            //  del server  e del client
    /* impostazione del socket */
	/*if((sem=semcreate())==-1) {//creazione semaforo per la memoria codivisa
		printf("creazione semaforo fallita");
		return(3);
	}

	if(seminit(sem,0)==-1){//inizializzazione semaforo
		printf("inizializzazione semaforo fallita");
		return(4);
	}*/
  printf("1\n");
	if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("chiamata alla system call socket fallita");
        return(1);
    }
    /* indicando INADDR_ANY viene collegato il socket all'indirizzo locale IP     */
    /* dell'interaccia di rete che verrâ€¡ utilizzata per inoltrare il datagram IP  */
    servizio.sin_family = AF_INET;
    servizio.sin_addr.s_addr = htonl(INADDR_ANY);
    servizio.sin_port = htons(SERVER_PORT);

    /* leghiamo l'indirizzo al socket */
    if (bind(socketfd, (struct sockaddr *)&servizio, sizeof servizio) == -1)
    {
        printf("chiamata alla system call bind fallita");
        return(2);
    }

    // Asta:
    typedef struct {
      int prezzo;
      int min;
      int offerte[50];
      int numofferte;
    } Asta;
  Asta * asta;

printf("2\n");
	if((mcid=creaMemCond(sizeof(Asta))) == -1){
		printf("Creazione memoria condivisa fallita");
		return(5);
	}
printf("3\n");
	asta=agganciaMemCond(mcid,0);
  if(((int)asta) == -1) printf("NO\n");
  //asta = (Asta *) asta;
printf("3.5\n");
printf("4: %d\n",asta->prezzo);
	asta->prezzo = 10;
    asta->min = 1;
    asta->numofferte = 0;
printf("4.5: %d\n",asta->prezzo);
    if(scollegaMemCond(asta) == -1){
      printf("Errore scollega mcid");
    }

    // Codici protocollo:
    int ok = 1, notok = 0;
    // Altro:
    int myId = 12300, n;

    /* poniamo il server in ascolto delle richieste dei client */
    listen(socketfd, 10);

    /* gestione delle connessioni dei client */
    while (1) {
        ct_l = sizeof(ct);
        if ((fd = accept(socketfd, (struct sockaddr *)&ct, &ct_l)) < 0)
        {
            printf("connessione non accettata");
            continue;
        }
        myId++;
        printf("Si è collegato %d\n",myId);
        if(fork() == 0){
          //RIUSA SHMGET
            if((asta=(Asta *)agganciaMemCond(mcid,0))==-1){
              printf("non e' possibile collegarsi la memoria condivisa");
              return(6);
            }
            /* ----- inizio dialogo tra server e client ------ */
            write(fd,&myId,sizeof(int)); // invio ID del client al client
            // Inizio contrattazione offerte
            while(1){
              // Leggo dato input dal client
              n = read(fd, &dato, sizeof(int));
              if(n == 0){ // Se ho ricevuto 0 byte
                printf("%d disconnesso\n",myId);
                break; }
              printf("Ricevuto dal client dato: %d (%d)\n",dato,n);
              if(dato > 0){
                // Dato > 0: client fa un'offerta
                //printf("Dato > 0\n");
				//semwait(sem);	// semaforo ROSSO e aspetta se già rosso
                if(asta->numofferte == 0){ // prima offerta
                  if(dato >= asta->prezzo + asta->min){
                    asta->offerte[asta->numofferte] = dato;
                    asta->numofferte++;
                    write(fd,&ok,sizeof(int));
                  } else {
                    printf("Offerta non valida\n");
                    write(fd,&notok,sizeof(int));
                  }
                } else if(dato >= asta->offerte[asta->numofferte-1]+asta->min){
                  // Offerta valida (non e' la prima offerta)
                  asta->offerte[asta->numofferte] = dato;
                  asta->numofferte++;
                  write(fd,&ok,sizeof(int));
                } else {
                  printf("Offerta non valida\n");
                  write(fd,&notok,sizeof(int));
                }
              } else if(dato == 0){
                // Client chiede numero offerte
                write(fd,&asta->numofferte,sizeof(int));
              } else if(dato == -1){
                // Client chiede disconnessione
                printf("%d disconnesso\n",myId);
                break;
              } else write(fd,&notok,sizeof(int));
			fflush(stdout);
			//semsignal(sem);	//semaforo VERDE
            }
            /* chusura della connessione */
            close(fd);
			rimuoviMemCond(mcid); //chiusura memoria condivisa
			//semdestroy(sem);	//chiusura semaforo
        }
    }
}
