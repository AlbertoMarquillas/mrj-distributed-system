/***********************************************
*
* @Proposit: Fitxer .h encarregada de les comunicacions de Gotham.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 21/10/2024
* @Data ultima modificacio:
*
************************************************/

#define _GNU_SOURCE //Definim la versió de la funció asprintf

//Definicio de la clase GothamComms
#ifndef GOTHAMCOMMSFUNCTS_H_ 
#define GOTHAMCOMMSFUNCTS_H_

#define printF(X) write(1, X, strlen(X)) //Funcio per imprimir per pantalla

//Includes
#include <string.h> //Include de la llibreria string per a les funcions de strings
#include <stdlib.h> //Include de la llibreria stdlib per a les funcions de memoria
#include <stdio.h> //Include de la llibreria stdio per a les funcions d'entrada/sortida
#include <errno.h> //Include de la llibreria errno per a la gestio d'errors
#include <unistd.h> //Include de la llibreria unistd per a la gestio de descriptores de fitxers
#include <arpa/inet.h> //Include de la llibreria arpa/inet per a la gestio de la xarxa
#include <sys/socket.h> //Include de la llibreria sys/socket per a la gestio de sockets
#include <netinet/in.h> //Include de la llibreria netinet/in per a la gestio de la xarxa
#include <pthread.h> //Include de la llibreria pthread per a la gestio de fils
#include <time.h>
#include <stdint.h>

#include "../Linkedlist/linkedlist_gotham.h" //Include de la llibreria linkedlist per a la gestio de llistes enllaçades
#include "../Gotham_funct/gotham_funct.h"

int GothamCommsFuncts_connectFleck(int fd, char* worker);
int GothamCommsFuncts_connectWorker(char* tramaRebuda, int fd_worker, InfoWorker *info_worker, LinkedList *list, char *typeWorker);
int GothamCommsFuncts_distort(int fd, LinkedList *list);

int GothamCommsFuncts_disconnectWorker(LinkedList *list, int fd_worker, int contWorker);
int GothamCommsFuncts_disconnectFleck(int fd);
int GothamCommsFuncts_distortResume(int fd_fleck, LinkedList *list);

int GothamCommsFuncts_assignaWorkerPrincipal(LinkedList *list, int fd_worker, char *typeWorker);

void GothamCommsFuncts_setFdArkam(int fd);
void GothamCommsFuncts_sendArkam(uint8_t type, const char* data, char *timestamp, char *workerType);
#endif

