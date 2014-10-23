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
    int socketfd, fd, dato, sem, memcond;
    unsigned int ct_l;
	void * prova_asta;	//non lo so se è giusto
    struct sockaddr_in servizio,ct;   //  record con gli indirizzi
                                            //  del server  e del client
    /* impostazione del socket */
	if((sem=semcreate())==-1) {//creazione semaforo per la memoria codivisa
		perror("creazione semaforo fallita");
		return(3);
	}

	if(seminit(sem,0)==-1){//inizializzazione semaforo
		perror("inizializzazione semaforo fallita");
		return(4);
	}

	if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("chiamata alla system call socket fallita");
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
        perror("chiamata alla system call bind fallita");
        return(2);
    }

    // Asta:
    struct {
      int prezzo;
      int min;
      int offerte[50];
      int numofferte;
    } asta;

	if((memcond=creaMemCond(sizeof(asta)))==-1){
		perror("Creazione memoria condivisa fallita");
		return(5);
	}

	if((prova_asta=agganciaMemCond(memcond,0))==-1){
		perror("non e' possibile collegarsi la memoria condivisa");
		return(6);
	}

  *prova_asta = asta;
	prova_asta->prezzo = 10;
    prova_asta->min = 1;
    prova_asta->numofferte = 0;

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
            perror("connessione non accettata");
            continue;
        }
        myId++;
        printf("Si è collegato %d\n",myId);
        if(fork() == 0){
            /* ----- inizio dialogo tra server e client ------ */
            write(fd,&myId,sizeof(int));
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
				semwait(sem);	// semaforo ROSSO e aspetta se già rosso
                if(prova_asta->numofferte == 0){ // prima offerta
                  if(dato >= prova_asta->prezzo + prova_asta->min){
                    prova_asta->offerte[prova_asta->numofferte] = dato;
                    prova_asta->numofferte++;
                    write(fd,&ok,sizeof(int));
                  } else {
                    printf("Offerta non valida\n");
                    write(fd,&notok,sizeof(int));
                  }
                } else if(dato >= prova_asta->offerte[prova_asta->numofferte-1]+prova_asta->min){
                  // Offerta valida (non e' la prima offerta)
                  prova_asta->offerte[prova_asta->numofferte] = dato;
                  prova_asta->numofferte++;
                  write(fd,&ok,sizeof(int));
                } else {
                  printf("Offerta non valida\n");
                  write(fd,&notok,sizeof(int));
                }
              } else if(dato == 0){
                // Client chiede numero offerte
                write(fd,&prova_asta->numofferte,sizeof(int));
              } else if(dato == -1){
                // Client chiede disconnessione
                printf("%d disconnesso\n",myId);
                break;
              } else write(fd,&notok,sizeof(int));
			fflush(stdout);
			semsignal(sem);	//semaforo VERDE
            }
            /* chusura della connessione */
            close(fd);
			rimuoviMemCond(memcond); //chiusura memoria condivisa
			semdestroy(sem);	//chiusura semaforo
        }
    }
}
