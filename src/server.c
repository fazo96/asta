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
#include <sys/ipc.h>
#include <sys/file.h>
#include <signal.h>

#define SERVER_PORT 1313          // numero di porta del server

int main (int argc, char *argv[]) {
  int socketfd, fd, dato, sem, filed;
  unsigned int ct_l;
  struct sockaddr_in servizio,ct;

  if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("chiamata alla system call socket fallita\n");
    return(1);
  }

  servizio.sin_family = AF_INET;
  servizio.sin_addr.s_addr = htonl(INADDR_ANY);
  servizio.sin_port = htons(SERVER_PORT);

  /* leghiamo l'indirizzo al socket */
  if (bind(socketfd, (struct sockaddr *)&servizio, sizeof servizio) == -1)
  {
      printf("Porta %d già occupata da un'altro processo\n",SERVER_PORT);
      return(2);
  }

  printf("Inizializzato server socket su porta %d\n",SERVER_PORT);

  // Asta:
  typedef struct {
    int soldi;
    int id;
  } Offerta;
  typedef struct {
    int prezzo;
    int min;
    Offerta offerte[50];
    int numofferte;
  } Asta;

  Asta asta;

  struct flock fl;
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = sizeof(asta);

  printf("Inizializzate strutture dati\n");
	filed = open("dati.dat",O_CREAT|O_RDWR,S_IRWXU /*| S_IRWXO*/);
  printf("Aperto file\n");
	asta.prezzo = 10;
  asta.min = 1;
  asta.numofferte = 0;
  write(filed,&asta,sizeof(asta));
  /*read(filed,&asta,sizeof(asta));
  printf("%d",asta.min);*/
  close(filed);
  printf("Inizializzato file (%d) con successo\n",filed);

  // Codici protocollo:
  int ok = -2, notok = -1;
  // Altro:
  int myId = 12300, n;

  /* poniamo il server in ascolto delle richieste dei client */
  listen(socketfd, 10);

  /* gestione delle connessioni dei client */
  while (1) {
      ct_l = sizeof(ct);
      if ((fd = accept(socketfd, (struct sockaddr *)&ct, &ct_l)) < 0)
      {
          printf("Connessione non accettata\n");
          continue;
      }
      myId++;
      printf("Si è collegato %d\n",myId);
      if(fork() == 0){ // FORK
          printf("Processo figlio %d iniziato\n",myId);
          filed = open("dati.dat",O_RDWR,S_IRWXU /*| S_IRWXO*/);
          read(filed,&asta,sizeof(asta));
          printf("Aperto file (%d) su %d\n",filed,myId);
          /* ----- inizio dialogo tra server e client ------ */
          write(fd,&myId,sizeof(int)); // invio ID del client al client
          // Inizio contrattazione offerte
          while(1){
            // Leggo dato input dal client
            n = read(fd, &dato, sizeof(int));
            lseek(filed,0,SEEK_SET);
            read(filed,&asta,sizeof(asta));
            lseek(filed,0,SEEK_SET);
            if(n == 0){ // Se ho ricevuto 0 byte
              printf("%d disconnesso\n",myId);
              break; }
            printf("Ricevuto dal client %d dato: %d (%d)\n",myId,dato,n);
            if(dato > 0){
              // Dato > 0: client fa un'offerta
              if(asta.numofferte == 0){ // prima offerta
                if(dato >= asta.prezzo + asta.min){
                  // ROSSO
                  fl.l_type = F_WRLCK;
                  fcntl(filed, F_SETLKW, &fl);
                  asta.offerte[asta.numofferte].soldi = dato;
                  asta.offerte[asta.numofferte].id = myId;
                  asta.numofferte++;
                  write(filed,&asta,sizeof(asta));
                  fl.l_type = F_UNLCK;
                  fcntl(filed, F_SETLKW, &fl);
                  // VERDE
                  write(fd,&ok,sizeof(int));
                } else {
                  //printf("Offerta non valida\n");
                  write(fd,&notok,sizeof(int));
                }
              } else if(dato >= asta.offerte[asta.numofferte-1].soldi+asta.min){
                // Offerta valida (ma non e' la prima offerta)
                // ROSSO
                fl.l_type = F_WRLCK;
                fcntl(filed, F_SETLKW, &fl);
                asta.offerte[asta.numofferte].soldi = dato;
                asta.offerte[asta.numofferte].id = myId;
                asta.numofferte++;
                write(filed,&asta,sizeof(asta));
                fl.l_type = F_UNLCK;
                fcntl(filed, F_SETLKW, &fl);
                // VERDE
                write(fd,&ok,sizeof(int));
              } else {
                printf("Offerta non valida\n");
                write(fd,&notok,sizeof(int));
              }
            } else if(dato == 0){
              // Client chiede numero offerte
              write(fd,&asta.numofferte,sizeof(int));
            } else if(dato == -1){
              // Client chiede disconnessione
              printf("%d disconnesso\n",myId);
              break;
            } else if(dato == -2){
              // Client chiede offerta piu alta
              write(fd,&asta.offerte[asta.numofferte].soldi,sizeof(int));
            } else if(dato == -3){
              // Client chiede proprio ID
              write(fd,&myId,sizeof(int));
            } else if(dato == -4){
              // Client chiede minima offerta valida
              int v = asta.offerte[asta.numofferte-1].soldi+asta.min;
              write(fd,&v,sizeof(int));
            } else if(dato == -5 && asta.numofferte > 0){
              // Client chiede offerente vincente
              write(fd,&asta.offerte[asta.numofferte-1].id,sizeof(int));
            } else write(fd,&notok,sizeof(int));
          }
          close(fd);
          fl.l_type = F_UNLCK;
          fcntl(filed, F_SETLKW, &fl);
          close(filed);
          printf("Processo %d chiuso correttamente!\n",myId);
      }
  }
  // Chiusura server
  close(socketfd);
  printf("Server chiuso correttamente!\n");
}
