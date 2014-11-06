/*
Gestione Asta - Client

The MIT License (MIT)

Copyright (c) 2014
Enrico Fasoli, Stefano Frittoli, Nicol Galli, Fernando Di Lillo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/times.h>
#include <errno.h>
#include <string.h>

int main(){
    struct hostent *he;
    struct sockaddr_in  client,server;
    int port = 1313,sockfd,bin,con;
    char address[] = "127.0.0.1";

    printf("Benvenuto. Protocollo di comunicazione:\n");
    printf("Server risponde -2: OK\n");
    printf("Server risponde -1: NOT OK\n");
    printf("Invia -4 per avere il valore dell'offerta minima possibile\n");
    printf("Invia -1 per chiedere la disconnessione\n");
    printf("Invia -2 per chiedere il valore dell'offerta piu alta\n");
    printf("Invia -3 per chiedere il tuo ID\n");
    printf("Invia -5 per chiedere l'ID dell'offerente vincente\n");
    printf("Invia un numero >0 per fare un'offerta!\n");
    printf("\nInserisci IP del server: ");
    scanf("%s",address);
    printf("Inserisci porta: ");
    scanf("%d",&port);
    printf("Inizializzazione socket (%s:%d)...\n",address,port);

    // Socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    // Client setup
    memset(&client,0,sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_port = htons(0);
    bin = bind(sockfd, (struct sockaddr*) &client, sizeof(client));
    // Server setup
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(address);
    server.sin_port = htons(port);
    // Connessione
    printf("Tentativo di connessione a %s:%d...\n",address,port);
    con = connect(sockfd,(struct sockaddr*) &server, sizeof(server));
    if(con != 0){
      printf("Connessione fallita\n");
      return -1;
    } else printf("Connesso a %s:%d\n",address,port);
    if(fork()==0){
      int n,r;
      while(1){
        n = read(sockfd,&r,sizeof(int));
        if(n > 0){
          printf("Messaggio dal Server: %d\n",r);
        } else {
          close(sockfd);
          printf("Connessione chiusa\n");
          break;
        }
      }
    }
    int num = 0;
    while(1){
      scanf("%d",&num);
      write(sockfd,&num,sizeof(int));
    }
}
