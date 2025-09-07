/***********************************************
*
* @Proposit: Fitxer .h encarregada de les comunicacions de Enigma.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 21/10/2024
* @Data ultima modificacio:
*
************************************************/

#define _GNU_SOURCE //Definim la versió de la funció asprintf

//Definicio de la clase EnigmaComms
#ifndef ENIGMACOMMS_H_
#define ENIGMACOMMS_H_


#define MEDIA "Media"
#define TEXT "Text"

//Defines
#define CONNEXIO_WORKER_GOTHAM 0x02
#define CONNEXIO_FLECK_WORKER 0x03
#define DISTORT_WORKER_FLECK 0x04 //Trama ja distorsionada
#define DISTORT_DADES_FLECK 0x05 //Trama amb dades distorsionades
#define DISTORT_MD5SUM_CHECK 0x06
#define DISCONNECT 0x07
#define NEW_WORKER 0x08
#define WRONG_DATA 0x09
#define HEARTBEAT 0x12
#define WRONG_CHECKSUM -1

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
#include <sys/ipc.h>
#include <sys/msg.h>
#include "../Enigma_funct/enigma_funct.h"

#include "../../Moduls_compartits/connexions/connexions.h" //Include del .h de les funcions auxiliars
#include "../../Moduls_compartits/utils/utils.h" //Include del .h de les funcions auxiliars
#include "../../Moduls_compartits/trames/trames.h" //Include del .h de les trames

#include "enigma_comms_functs.h"

#include "../../Moduls_compartits/compression/so_compression.h" //Include del .h de les trames
#include "../Enigma_funct/enigma_funct.h"
#include "../Linkedlist/linkedlist_enigma.h"
void* EnigmaComms_threadFleck(void* arg);
void* EnigmaComms_heartbeatThreadGotham(void *arg);
#endif