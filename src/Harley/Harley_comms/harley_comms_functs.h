
#define _GNU_SOURCE //Definim la versió de la funció asprintf

//Definicio de la clase GothamComms
#ifndef HARLEYCOMMSFUNCTS_H_ 
#define HARLEYCOMMSFUNCTS_H_

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

#include "../../Moduls_compartits/trames/trames.h" //Include del .h de les trames
#include "../../Moduls_compartits/connexions/connexions.h" //Include del .h de les funcions auxiliars
#include "../../Moduls_compartits/compression/so_compression.h" //Include del .h de les trames
#include "../Harley_funct/harley_funct.h"



int HarleyCommsFuncts_ComprovarMD5SUM();
int HarleyCommsFuncts_distorsionarFitxer(char *extensio, int factor, char *FilePath);
int HarleyCommsFuncts_enviarFitxer(int fd_worker, InfoThread *info_thread, CuaInfoFleck msg, int fleck_id, ContingutFleck contingutFleck);
CuaInfoFleck HarleyCommsFuncts_actualitzaPunterLinked(LinkedList *linked_cua, int fleck_id);
#endif