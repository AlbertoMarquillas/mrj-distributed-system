/***********************************************
*
* @Proposit: Fitxer .h encarregada de les funcions de comunicacions de Fleck.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio:
*
************************************************/

#define _GNU_SOURCE

// Definicio de la classe FleckCommsFuncts
#ifndef FLECKCOMMSFUNCTS_H_
#define FLECKCOMMSFUNCTS_H_

#define CONNEXIO_FLECK_GOTHAM 0x01
#define DISTORT_GOTHAM_FLECK 0x10
#define DISTORT_RESUME_GOTHAM_FLECK 0x11
#define DISTORT_INFO_FITXER_SEND 0x03
#define DISTORT_INFO_FITXER_RECIVE 0x04 //Trama ja distorsionada
#define DISTORT_RECIVE_FITXER 0x05 //Trama amb dades distorsionades
#define DISTORT_MD5SUM_CHECK 0x06
#define DISCONNECT 0x07
#define WRONG_DATA 0x09
#define HEARTBEAT 0x12
#define WRONG_CHECKSUM -1

#define ERROR_DIR "Error al obrir el directori\n" //Missatge d'error al obrir el directori

#define printF(X) write(1, X, strlen(X)) //Funcio per imprimir per pantalla
#define _POSIX_C_SOURCE 200809L //Definim la versió de la funció asprintf

//Includes
#include <string.h> //Include de la llibreria string per a les funcions de strings
#include <stdlib.h> //Include de la llibreria stdlib per a les funcions de memoria
#include <stdio.h> //Include de la llibreria stdio per a les funcions d'entrada/sortida
#include <errno.h> //Include de la llibreria errno per a la gestio d'errors
#include <dirent.h> //Include de la llibreria dirent per a la gestio de directoris
#include <unistd.h> //Include de la llibreria unistd per a la gestio de processos
#include <sys/types.h> //Include de la llibreria sys/types per a la gestio de processos
#include <ctype.h> //Include de la llibreria ctype per a les funcions de caràcters
#include "../Fleck_funct/fleck_funct.h" //Include del modul de funcions de Fleck
#include "../../Moduls_compartits/connexions/connexions.h" //Include del .h de les funcions auxiliars
#include "../../Moduls_compartits/trames/trames.h" //Include del .h de les trames

#include "../Linkedlist/linkedlist_fleck.h" //Include del .h de les linkedlist
#include "../../Moduls_compartits/semaphore/gestio_semaphore.h" //Include del .h de les funcions de semàfors
/***********************************************
 * @Nom: FleckCommsFuncts_obrirDirectori
 * @Definicio: Funció que obre un directori
 * @Arg: char* path: Path del directori
 * @Arg: DIR* dir: Punter al directori
 * @Ret: ----
 * ***********************************************/
DIR* FleckCommsFuncts_obrirDirectori(char* path, DIR* dir);

/***********************************************
 * @Nom: FleckComms_IsTextFile
 * @Definicio: Funció que comprova si un fitxer és de text
 * @Arg: char* file_name: Nom del fitxer
 * @Ret: int: 1 si és de text, 0 si no ho és
 * ***********************************************/
int FleckCommsFuncts_IsTextFile(char* file_name);

/***********************************************
 * @Nom: FleckCommsFuncts_ListTextMedia
 * @Definicio: Funció que llista els fitxers multimèdia o de text del servidor Fleck
 * @Arg: Path: Path dels fitxers multimèdia
 * @Ret: ----
 * ***********************************************/ 
void FleckCommsFuncts_ListTextMedia(char *path, char* tipus);

/***********************************************
 * @Nom: FleckCommsFuncts_CheckSpaces
 * @Definicio: Funció que comprova si hi ha espais a la comanda
 * @Arg: char* command: Comanda introduida per l'usuari
 * @Ret: char*: Comanda sense espais
 * ***********************************************/
char* FleckCommsFuncts_CheckSpaces(char* command);

/***********************************************
 * @Nom: FleckCommsFuncts_CheckFormat
 * @Definicio: Funció que comprova si la comanda té el format correcte
 * @Arg: char* command: Comanda introduida per l'usuari
 * @Ret: int: 1 si la comanda té el format correcte, 0 si no
 * ***********************************************/
int FleckCommsFuncts_CheckFormat(char* command);
int FleckCommsFuncts_comprovaMD5SUM();
int FleckCommsFuncts_GestionaCaigudaWorker(char *mediaType, char *fileName, int fd_gotham, int fd_worker, char *userName, char *factorDist, char *fileSize, char *Md5);
void FleckCommsFuncts_MostraBarraProgres(int percentatge);

int FleckCommsFuncts_EnviarDesconnexioWorker(int fd_worker, char *userName, LinkedList *info_status, InfoStatus elementStatus);


int FleckCommsFuncts_EnviarTramaResumeDistort(int fd_worker);
#endif /* FLECKCOMMSFUNCTS_H_ */