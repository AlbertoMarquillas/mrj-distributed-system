/***********************************************
*
* @Proposit: Fitxer .h encarregada de les funcions auxiliars de Fleck.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio:
*
************************************************/
#define _GNU_SOURCE

// Definicio de la clase FleckFunct
#ifndef FLECKFUNCT_H_
#define FLECKFUNCT_H_

//Definim la constant per indicar error d'arguments
#define ERROR_ARGS "Error, wrong number of arguments\n"  
//Definim la constant per indicar error de fitxer
#define ERROR_FILE "Couldn't read file\n"
//Definim la constant per indicar error de fitxer de configuracio
#define ERROR_FITXER "Error obrint el fitxer de configuracion\n"
#define printF(X) write(1, X, strlen(X)) //Funcio per imprimir per pantalla

#define USR_INIT "user initialized\n" //Missatge de benvinguda

#define MEDIA "Media"
#define TEXT "Text"

//Es crea un struct per guardar les dades del fitxer de configuracio
typedef struct {
    char *user_name;
    char *folder_name;
    char *gotham_ip;
    char *gotham_port;
}FleckConfig;

#include <string.h> //Include de la llibreria string per a les funcions de strings
#include <stdlib.h> //Include de la llibreria stdlib per a les funcions de memoria
#include <stdio.h> //Include de la llibreria stdio per a les funcions d'entrada/sortida
#include <errno.h> //Include de la llibreria errno per a la gestio d'errors
#include <unistd.h> //Include de la llibreria unistd per a la gestio de descriptores de fitxers
#include <fcntl.h> //Include de la llibreria fcntl per a la gestio de fitxers
#include <ctype.h> //Include de la llibreria ctype per a les funcions de caràcters
#include <signal.h> //Include de la llibreria signal per a la gestio de senyals

#include "../../Moduls_compartits/utils/utils.h" //Include del .h de les funcions auxiliars
#include "../../Moduls_compartits/compression/so_compression.h" //Include del .h de les trames

/***********************************************
 * @Nom: FleckFunct_freeMemoryFleck
 * @Definicio: Funció que allibera la memòria
 * @Arg: FleckConfig *fleck_config_file: Estructura on es guarda la informació del fitxer de configuració
 * @Ret: ----
 * ***********************************************/
void FleckFunct_freeMemoryFleck(FleckConfig *fleck_config_file);

/***********************************************
* @Nom: FleckFunct_llegir_comanda
* @Definicio: Funció que llegeix una comanda
* @Arg: char* command: Comanda introduida per l'usuari
* @Ret: char *: Cadena de caràcters amb la comanda
***********************************************/
char *FleckFunct_llegirComanda();

/***********************************************
* @Nom: FleckFunct_comprovarAmpresand
* @Definicio: Funció que comprova si el nom d'usuari conté el caràcter '&'
* @Arg: FleckConfig *fleck_config_file: Estructura on es guarda la informació del fitxer de configuració
* @Ret: FleckConfig amb la informació del fitxer de configuració
***********************************************/
FleckConfig FleckFunct_comprovarAmpresand(FleckConfig fleck_config_file);

/***********************************************
* @Nom: FleckFunct_readConfigFleck
* @Definicio: Funció que llegeix el fitxer de configuració i guarda la informació en una estructura
* @Arg: char* file_name: Nom del fitxer de configuració
* @Arg: FleckConfig *fleck_config_file: Estructura on es guarda la informació del fitxer de configuració
* @Ret: FleckConfig modificada amb la informació del fitxer de configuració
***********************************************/
FleckConfig FleckFunct_readConfigFleck(char* file_name, FleckConfig fleck_config_file);


/***********************************************
 * @Nom: FleckFunct_checkCommandFleck
 * @Definicio: Funció que comprova si la comanda introduida és correcta
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Arg: char* command: Comanda introduida per l'usuari
 * @Ret: int: 0 si la comanda és correcta, -1 si la comanda és incorrecta
 * ***********************************************/
 int FleckFunct_checkCommandFleck(int connected, char* command);



#endif

