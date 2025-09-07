/***********************************************
*
* @Proposit: Fitxer .h encarrefat de les funcions auxiliars de Gotham.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio: 16/10/2024
*
************************************************/

//Definim GNU_SOURCE
#define _GNU_SOURCE

// Definicio de la clase GothamFunct
#ifndef GOTHAMFUNCT_H_
#define GOTHAMFUNCT_H_

#define MEDIA "Media"
#define TEXT "Text"

//Defines
#define ERROR_ARGS "Error, wrong number of arguments\n" //Definim la constant per indicar error d'arguments  
#define ERROR_FILE "Couldn't read file\n" //Definim la constant per indicar error de fitxer
#define ERROR_FITXER "Error obrint el fitxer de configuracion\n" //Definim la constant per indicar error de fitxer de configuracio

//Includes
#include <string.h> //Include de la llibreria string per a les funcions de strings
#include <stdlib.h> //Include de la llibreria stdlib per a les funcions de memoria
#include <stdio.h> //Include de la llibreria stdio per a les funcions d'entrada/sortida
#include <errno.h> //Include de la llibreria errno per a la gestio d'errors
#include <unistd.h> //Include de la llibreria unistd per a la gestio de descriptores de fitxers
#include <fcntl.h> //Include de la llibreria fcntl per a la gestio de fitxers
#include <signal.h> //Include de la llibreria signal per a la gestio de senyals

#include "../../Moduls_compartits/utils/utils.h" //Include del .h de les funcions auxiliars
#include "../../Moduls_compartits/connexions/connexions.h" //Include del .h de les funcions auxiliars
#include "../../Moduls_compartits/trames/trames.h" //Include del .h de les trames
#include "../Linkedlist/linkedlist_gotham.h"

//Es crea un struct per guardar les dades del fitxer de configuracio
typedef struct {
    char *fleck_ip;
    char *fleck_port;
    char *worker_ip;
    char *worker_port;
}GothamConfig;

typedef struct {
    char* type;
    char* ip;
    char* port;
    int fd;
    int user_name;
} InfoClient;

typedef struct _InfoThread {
    int thread_index;  // Añadir este campo
    InfoWorker *info_worker;
    InfoClient *info_client;
    int *gotham_running;
} InfoThread;



typedef struct {
    int fd_worker;
    char *type;
} InfoThreadWorker;


/***********************************************
* @Nom: GothamFunct_readConfigGotham
* @Definicio: Funció que llegeix el fitxer de configuració i guarda la informació en una estructura
* @Arg: char* file_name: Nom del fitxer de configuració
* @Arg: GothamConfig *gotham_config_file: Estructura on es guarda la informació del fitxer de configuració
* @Ret: int: 0 si tot ha anat bé, -1 si hi ha hagut algun error
***********************************************/
int GothamFunct_readConfigGotham(char* file_name, GothamConfig *gotham_config_file);

#endif