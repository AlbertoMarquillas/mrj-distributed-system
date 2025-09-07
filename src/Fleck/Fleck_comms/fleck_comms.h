/***********************************************
*
* @Proposit: Fitxer .h encarregada de les comunicacions de Fleck.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio:
*
************************************************/
#define _GNU_SOURCE
// Definicio de la clase FleckComms
#ifndef FLECKCOMMS_H_
#define FLECKCOMMS_H_

//Definim un missatge per quan no detectem la comanda introduida
#define ERROR_COMMAND "ERROR: Please input a valid command.\n"

#define printF(X) write(1, X, strlen(X)) //Funcio per imprimir per pantalla
#define _POSIX_C_SOURCE 200809L //Definim la versió de la funció asprintf

//Include del modul de funcions de Fleck
#include "../Fleck_funct/fleck_funct.h"
#include "../../Moduls_compartits/connexions/connexions.h" //Include del .h de les funcions auxiliars
#include "../../Moduls_compartits/utils/utils.h" //Include del .h de les funcions auxiliars
#include "../Linkedlist/linkedlist_fleck.h"
#include "../../Moduls_compartits/semaphore/gestio_semaphore.h"
#include <ctype.h> //Include de la llibreria ctype per a les funcions de caràcters
#include "../../Moduls_compartits/semaphore/gestio_semaphore.h"


typedef struct {
    char* type;
    char* ip;
    char* port;
    int fd;
    char *userName;
    char *file;
    char *factor;
} HarleyEnigmaThread;

typedef struct{
    int fd;
} GothamThread;



void FleckComms_EsperaFinalizacioThreads();


/***********************************************
 * @Nom: FleckComms_ConnectFleck
 * @Definicio: Funció que connecta amb el servidor Fleck
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
//int FleckComms_ConnectFleck(int connected, char *port, char *ip, char *userName);
int FleckComms_ConnectFleck(int connected, char *port, char *ip, char *userName);


/***********************************************
 * @Nom: FleckComms_LogoutFleck
 * @Definicio: Funció que desconnecta del servidor Fleck
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
int FleckComms_LogoutFleck(int connected, char *fleck_userName);

/***********************************************
 * @Nom: FleckComms_ListMediaFleck
 * @Definicio: Funció que llista els fitxers multimèdia del servidor Fleck
 * @Arg: char* path: Path dels fitxers multimèdia
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
void FleckComms_ListMediaFleck(char* path, int connected);

/***********************************************
 * @Nom: FleckComms_ListTextFleck
 * @Definicio: Funció que llista els fitxers de text del servidor Fleck
 * @Arg: char* path: Path dels fitxers de text
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
void FleckComms_ListTextFleck(char* path, int connected);

/***********************************************
 * @Nom: FleckComms_DistortFleck
 * @Definicio: Funció que distorsiona un fitxer del servidor Fleck
 * @Arg: char* file_name: Nom del fitxer a distorsionar
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Arg: char *userName: Nom de l'usuari
 * @Ret: ----
 * ***********************************************/
void FleckComms_DistortFleck(char *file_n, int connected, char *userName);


/***********************************************
 * @Nom: FleckComms_CheckStatusFleck
 * @Definicio: Funció que comprova l'estat del servidor Fleck
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
void FleckComms_CheckStatusFleck(int connected, int size);

/***********************************************
 * @Nom: FleckComms_ClearAllFleck
 * @Definicio: Funció que esborra les distorsions ja acabades del servidor Fleck
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
void FleckComms_ClearAllFleck(int connected);

/***********************************************
 * @Nom: FleckComms_ExecuteCommandFleck
 * @Definicio: Funció que executa la comanda introduida per l'usuari
 * @Arg: char* command: Comanda introduida per l'usuari
 * @Arg: int logout: Valor que indica si l'usuari vol sortir del sistema
 * @Ret: ----
 * ***********************************************/
//int FleckComms_ExecuteCommandFleck(char* command, int logout, char* path, char *port, char *ip, char *userName);
int FleckComms_ExecuteCommandFleck(char *command, int logout, char *path, char *port, char *ip, char *userName);



void *FleckComms_handleHarleyClient(void* arg);
void* FleckComms_handleHeartbeatsGotham(void* arg);

#endif