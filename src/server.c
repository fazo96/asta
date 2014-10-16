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

#define SERVER_PORT 1313          // numero di porta del server

int main (int argc, char *argv[]) {
    int socketfd, fd, dato;
    unsigned int ct_l;
    struct sockaddr_in servizio,ct;   //  record con gli indirizzi
                                            //  del server  e del client
    /* impostazione del socket */
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

    /* configurazione asta */
    struct {
      int prezzo;
      int min;
      int offerte[50];
      int numofferte;
    } asta;
    asta.prezzo = 10;
    asta.min = 1;
    asta.numofferte = 0;
    int ok = 1, notok = 0, myId = 12300, n;

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
              n = read(fd, &dato, sizeof(int));
              if(n == 0){
                printf("%d disconnesso\n",myId);
                break; }
              printf("Ricevuto dal client dato: %d (%d)\n",dato,n);
              if(dato > 0){
                // Dato > 0: client fa un'offerta
                //printf("Dato > 0\n");
                if(asta.numofferte == 0){ // prima offerta
                  if(dato >= asta.prezzo + asta.min){
                    asta.offerte[asta.numofferte] = dato;
                    asta.numofferte++;
                    write(fd,&ok,sizeof(int));
                  } else {
                    printf("Offerta non valida\n");
                    write(fd,&notok,sizeof(int));
                  }
                } else if(dato >= asta.offerte[asta.numofferte-1]+asta.min){
                  // Offerta valida non prima
                  asta.offerte[asta.numofferte] = dato;
                  asta.numofferte++;
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
              } else write(fd,&notok,sizeof(int));
            }
            /* chusura della connessione */
            close(fd);
        }
    }
}
