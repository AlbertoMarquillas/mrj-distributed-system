/***********************************************
*
* @Proposit: Fitxer .h de funcions per connexions
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 11/10/2024
* @Data ultima modificacio:
*
************************************************/

//Degine _GNU_SOURCE
#define _GNU_SOURCE

//Create class CONNEXIONS
#ifndef CONNEXIONS_
#define CONNEXIONS_

#define MEDIA "Media"
#define TEXT "Text"

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

/***********************************************
 * @Nom: Connexions_openServer
 * @Definicio: Funció que obre un servidor
 * @Arg: int port: Port del servidor
 * @Arg: char *ip: IP del servidor
 * @Ret: int: El socket del servidor si tot ha anat bé, -1 si hi ha hagut algun error
 * ***********************************************/
int Connexions_openServer(int port, char *ip);

/***********************************************
 * @Nom: Connexions_acceptar
 * @Definicio: Funció que accepta una connexió
 * @Arg: int fd: File descriptor del socket
 * @Ret: int: El file descriptor del client si tot ha anat bé, -1 si hi ha hagut algun error
 * ***********************************************/
int Connexions_acceptar(int fd);

/***********************************************
 * @Nom: Connexions_connect
 * @Definicio: Funció que connecta amb un servidor
 * @Arg: int port: Port del servidor
 * @Arg: char *buffer: Cadena que conte la informacio a llegir
 * @Ret: char *: El missatge rebut
 * ***********************************************/
char* Connexions_read(int fd, char* buffer);

/***********************************************
 * @Nom: Connexions_connectGotham
 * @Definicio: Funció que connecta amb el servidor Gotham
 * @Arg: int port: Port del servidor
 * @Arg: char *ip: IP del servidor
 * @Ret: int: 0 si tot ha anat bé, -1 si hi ha hagut algun error
 ***********************************************/
int Connexions_connectServer(int port, char *ip);

/***********************************************
 * @Nom: Connexions_send
 * @Definicio: Funció que envia un missatge al servidor
 * @Arg: char* msg: Missatge a enviar
 * @Arg: int fd: File descriptor del servidor
 * @Ret: int: 0 si tot ha anat bé, -1 si hi ha hagut algun error
 * ***********************************************/
int Connexions_send(char* msg, int fd);
#endif