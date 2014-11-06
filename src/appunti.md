## Documentazione

http://markcloud.meteor.com/d/eNXxxrjE9yqXaPjpD

# To do

- scrivere definizione precisa protocollo (Fasoli)
- organizzare e commentare il codice

## Client

- miglioramenti interfaccia

## Server

- Collegare sottoprocessi alla shared memory e scollegarli quando finiscono (Lillo)
- gestire l'accesso con i semafori
- Gestire offerte con uno struct che segna anche l'ID dell'offerente

### Protocollo

Invio e ricezione di numeri interi via socket
A ogni messaggio inviato corrisponde UN messaggio di risposta __SEMPRE__

Codifica:

- -2 (Si)
- -1 (No)
- Client manda 0 per chiedere numero offerte
- Client manda -1 per chiedere la disconnessione
- Client manda -2 per chiedere valore della più alta offerta
- Client manda -3 per chiedere proprio ID
- Client manda -4 per chiedere minima offerta valida
- Client manda -5 per chiedere l'ID dell'offerente vincente
