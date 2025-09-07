/***********************************************
*
* @Proposit: Fitxer .h de funcions auxiliars per a Harley i Harley.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 11/10/2024
* @Data ultima modificacio:
*
************************************************/

// Definim GNU_SOURCE
#define _GNU_SOURCE

// Definicio de la clase HarleyFunct
#ifndef HARLEYFUNCT_H_
#define HARLEYFUNCT_H_

//Defines
#define ERROR_ARGS "Error, wrong number of arguments\n" //Definim la constant per indicar error d'arguments  
#define ERROR_FITXER "Error obrint el fitxer de configuracion\n" //Definim la constant per indicar error de fitxer

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
#include <fcntl.h> //Include de la llibreria fcntl per a la gestio de fitxers
#include "../../Moduls_compartits/utils/utils.h" //Include del .h de les funcions auxiliars
#include <signal.h> //Include de la llibreria signal per a la gestio de senyals

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <dirent.h>



#include "../Linkedlist/linkedlist_harley.h"
#include "../../Moduls_compartits/semaphore/gestio_semaphore.h"


#define MAX_PATH 256

//INITS CUES
#define ERROR_FILE "Error al obrir el fitxer de configuracio\n" //Missatge d'error al obrir el fitxer de configuracio
#define KEY_INFO_FLECK "/tmp/INFO_FLECK_x" 
#define ID_INFO_FLECK '8'

#define KEY_CONT_HARLEY "/tmp/CONT_HAREY_x" 
#define ID_CONT_HARLEY '9'


#define MAX_FLECKS 30
//Es crea un struct per guardar les dades del fitxer de configuracio
typedef struct {
    char *gotham_ip;
    char *gotham_port;
    char* fleck_ip;
    char* fleck_port;
    char* folder_name;
    char* type;
}HarleyConfig;


typedef struct {
    HarleyConfig harley_config_file;
    int fd;
    int fleck_id;
}FleckThread;

typedef struct {
    int fd;
} GothamThread;

typedef struct {
    FleckThread *info_fleck;
    LinkedList *linked_cua;
    char *folderName;
}InfoThread;

typedef struct {
    char path[256];
    char userName[256];
    char md5sum[256];
    int fileSize;
    int factor;
    char fileName[256];
} ContingutFleck;


typedef struct {
    long mtype;         // Tipus de missatge (per la cua de missatges)
    int n_Harley;
} CuaContHarley;

typedef struct {
    int n_flecks;
} FleckContCua;

/***********************************************
* @Nom: HarleyHarleyFunct_readConfigHarley
* @Definicio: Funció que llegeix el fitxer de configuració i guarda la informació en una estructura
* @Arg: char* file_name: Nom del fitxer de configuració
* @Arg: HarleyHarleyConfig *HarleyHarley_config_file: Estructura on es guarda la informació del fitxer de configuració
* @Ret: int: 0 si tot ha anat bé, -1 si hi ha hagut algun error
***********************************************/
int HarleyFunct_readConfigHarley(char* file_name, HarleyConfig *harley_config_file);   
int HarleyFunct_accedeixCua(int type);
void HarleyFunct_eliminaCua(int msgid);
int HarleyFunct_ConnectToGotham(HarleyConfig harley_config_file, LinkedList* cua);
int HarleyFunct_esperaWorkerPrincipal(int fd_gotham);
void eliminarFitxersMediaFiles();
#endif