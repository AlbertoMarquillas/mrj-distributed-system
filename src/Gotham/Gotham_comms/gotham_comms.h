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
#ifndef GOTHAMCOMMS_H_ 
#define GOTHAMCOMMS_H_ 

//Defines
#define ERROR_CONNECT "Error connecting to server\n" //Missatge d'error al connectar-se al servidor
#define ERROR_SOCKET "Error creating socket\n" //Missatge d'error al crear el socket
#define ERROR_BIND "Error binding socket\n" //Missatge d'error al fer el bind del socket
#define ERROR_ACCEPT "Error accepting connection\n" //Missatge d'error al acceptar la connexió

#define printF(X) write(1, X, strlen(X)) //Funcio per imprimir per pantalla

#define CONNEXIO_FLECK_GOTHAM 0x01
#define CONNEXIO_WORKER_GOTHAM 0x02
#define DISTORT_GOTHAM_FLECK 0x10
#define DISTORT_RESUME_GOTHAM_FLECK 0x11
#define DISCONNECT 0x07
#define NEW_WORKER 0x08
#define WRONG_DATA 0x09
#define HEARTBEAT 0x12

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
#include <unistd.h>
#include "gotham_comms_functs.h"
/***********************************************
 * @Nom: GothamConnexions_read
 * @Definicio: Thread que connecta amb el servidor Harley/Enigma
 * @Arg: void *arg: Parametres del thread
 * @Ret: ----
 * ***********************************************/
void* GothamComms_connectWorker(void *arg);

/***********************************************
 * @Nom: GothamComms_connectFleck
 * @Definicio: Thread que connecta amb el servidor Fleck
 * @Arg: void *arg: Parametres del thread
 * @Ret: ----
 * ***********************************************/
void* GothamComms_connectFleck(void *arg);

void* GothamComms_handleFleckClient(void* arg);

void GothamComms_freeLists();

void GothamComms_freeThreads();
void GothamComms_closeFileDescriptors();
void GothamComms_freeMem();
#endif