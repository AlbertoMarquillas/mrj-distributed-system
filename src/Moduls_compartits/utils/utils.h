/***********************************************
*
* @Proposit: Fitxer .h de funcions auxiliars
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 11/10/2024
* @Data ultima modificacio:
*
************************************************/
//Degine _GNU_SOURCE
#define _GNU_SOURCE

//Create class UTILS
#ifndef UTILS_
#define UTILS_


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
#include <fcntl.h> //Include de la llibreria fcntl per a la gestio de fitxers
#include "../connexions/connexions.h" //Include del .h de les funcions auxiliars
#include "../trames/trames.h" //Include del .h de les trames
#include <sys/wait.h>

#define printF(X) write(1, X, strlen(X)) //Funcio per imprimir per pantalla

/***********************************************
* @Nom: Utils_readFileLimit
* @Definicio: Funció que llegeix un fitxer fins a un limitador
* @Arg: int fd: File descriptor del fitxer que es vol llegir
* @Arg: char limit: Caràcter que indica el final del fitxer
* @Ret: char *: Cadena de caràcters amb el contingut del fitxer
***********************************************/
char *Utils_readFileLimit(int fd, char limit);

void Utils_printarBenvinguda(char* msg, char* user);

char* utils_getMD5SUMfromFIle(char* filename);
int utils_enviarFitxer(int fd_worker, char *path, int fileSize);
int utils_getFileSize(char *path);
char *utils_intToChar(int num);

int utils_comparaMD5SUM(int fd_file, char *MD5SUM, char *path);
int utils_comparaMD5SUMA(int fd_file, char *MD5SUM, char *path, int fd);


#endif