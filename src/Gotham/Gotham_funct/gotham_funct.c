/***********************************************
*
* @Proposit: Fitxer encarrefat de les funcions auxiliars de Gotham.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio: 16/10/2024
*
************************************************/
//Include del .h associat a aquest .c
#include "gotham_funct.h"

/***********************************************
* @Nom: GothamFunct_readConfigGotham
* @Definicio: Funció que llegeix el fitxer de configuració i guarda la informació en una estructura
* @Arg: char* file_name: Nom del fitxer de configuració
* @Arg: GothanConfig *gotham_config_file: Estructura on es guarda la informació del fitxer de configuració
* @Ret: int: 0 si tot ha anat bé, -1 si hi ha hagut algun error
***********************************************/
int GothamFunct_readConfigGotham(char* file_name, GothamConfig *gotham_config_file){
   
    int fd_gotham; //File descriptor del fitxer de configuracio
    fd_gotham = open(file_name, O_RDONLY); //Obrim el fitxer de configuracio en mode lectura
 
    if(fd_gotham < 0){ //Si s'ha produit un error al obrir el fitxer
        perror(ERROR_FITXER); //Es mostra un missatge d'error
        return -1; //Retornem -1
    }
    gotham_config_file->fleck_ip = Utils_readFileLimit(fd_gotham, '\n'); //Llegim la IP de Fleck
    gotham_config_file->fleck_port = Utils_readFileLimit(fd_gotham, '\n'); //Llegim el port de Fleck
    gotham_config_file->worker_ip = Utils_readFileLimit(fd_gotham, '\n'); //Llegim la IP de Harley i Enigma
    gotham_config_file->worker_port = Utils_readFileLimit(fd_gotham, '\n'); //Llegim el port de Harley i Enigma

    close(fd_gotham); //Tanquem el fitxer de configuracio
    return 0;
}