# Gestione Asta

Applicazione client/server scritta in C che gestisce un'asta.

Enrico Fasoli, Stefano Frittoli, Nicol Galli, Fernando Di Lillo

Il programma e il sorgente sono disponibili [online](https://github.com/fazo96/asta)

### Compilazione

Utilizzare lo script `make.sh` per compilare il software.
Testato su OSX e GNU/Linux.

### Protocollo

Invio e ricezione di numeri interi via socket

A ogni messaggio inviato corrisponde __UN__ messaggio di risposta __SEMPRE__

Codifica:

- Server risponde -2: Ok / Successo / Vero
- Server risponde -1: Errore / No / Falso
- Client manda 0 per chiedere numero offerte
- Client manda -1 per chiedere la disconnessione
- Client manda -2 per chiedere valore della più alta offerta
- Client manda -3 per chiedere proprio ID
- Client manda -4 per chiedere minima offerta valida
- Client manda -5 per chiedere l'ID dell'offerente vincente
- Client manda numero >0 per fare un'offerta

### Licenza

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
