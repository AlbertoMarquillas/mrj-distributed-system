/***********************************************
*
* @Proposit: Fitxer encarregada de les funcions auxiliars de Fleck.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio: 16/10/2024
*
************************************************/
//Include del .h associat a aquest .c
#include "fleck_funct.h"


//Variables globals
int fd_fleck; //File descriptor del fitxer de configuracio
char *command_mays; //Cadena de caràcters per guardar la comanda en majúscules



/***********************************************
 * @Nom: FleckFunct_freeMemoryFleck
 * @Definicio: Funció que allibera la memòria
 * @Arg: FleckConfig *fleck_config_file: Estructura on es guarda la informació del fitxer de configuració
 * @Ret: ----
 * ***********************************************/
void FleckFunct_freeMemoryFleck(FleckConfig *fleck_config_file) {
    if(fleck_config_file->user_name != NULL){
        free(fleck_config_file->user_name);
        fleck_config_file->user_name = NULL;
    }
    if(fleck_config_file->folder_name != NULL){
        free(fleck_config_file->folder_name);
        fleck_config_file->folder_name = NULL;
    }
    if(fleck_config_file->gotham_ip != NULL){
        free(fleck_config_file->gotham_ip);
        fleck_config_file->gotham_ip = NULL;
    }
    if(fleck_config_file->gotham_port != NULL){
        free(fleck_config_file->gotham_port);
        fleck_config_file->gotham_port = NULL;
    }
    if(command_mays != NULL){
        free(command_mays);
        command_mays = NULL;
    }
}


/***********************************************
* @Nom: FleckFunct_llegir_comanda
* @Definicio: Funció que llegeix una comanda
* @Arg: char* command: Comanda introduida per l'usuari
* @Ret: char *: Cadena de caràcters amb la comanda
***********************************************/
char *FleckFunct_llegirComanda() {
    int length = 0, error;
    char *string = (char *)malloc(sizeof(char)); // Asignamos memoria inicial
    char c;

    error = read(0, &c, sizeof(char)); 
    while (c != '\n' && error > 0) { //mentres no sigui un salt de línia o no hi hagi error
        string = (char *)realloc(string, sizeof(char) * (length + 2)); //Reservem memòria per al caràcter
        string[length++] = c; 
        error = read(0, &c, sizeof(char));
    }
    string[length] = '\0';
    return string; //Retornem la cadena
}

/***********************************************
* @Nom: FleckFunct_comprovarAmpresand
* @Definicio: Funció que comprova si el nom d'usuari conté el caràcter '&'
* @Arg: FleckConfig *fleck_config_file: Estructura on es guarda la informació del fitxer de configuració
* @Ret: FleckConfig sense ampresand
***********************************************/
FleckConfig FleckFunct_comprovarAmpresand(FleckConfig fleck_config_file) {
    char *user_name = fleck_config_file.user_name; // Punter a la cadena de nom d'usuari
    int ampersand = 0; //Variable per comptar el número de '&'
    while (*user_name) {
        if (*user_name == '&') {
            // Revisa si el '&' es el primer caràcter de la cadena
            if (*(user_name + 1) != '\0') {
                *user_name = *(user_name + 1); //Si no es el primer caràcter, mou el nom d'usuari un caracter a l'esquerra
                ampersand++;
            } else {
                // Si el '&' es l'últim caracter de la cadena, l'elimina
                *user_name = '\0';
                break;
            }
        } else if (ampersand > 0) {
            *user_name = *(user_name + 1);
        }
        user_name++;
    }
    return fleck_config_file;
}


/***********************************************
* @Nom: FleckFunct_readConfigFleck
* @Definicio: Funció que llegeix el fitxer de configuració i guarda la informació en una estructura
* @Arg: char* file_name: Nom del fitxer de configuració
* @Arg: FleckConfig *fleck_config_file: Estructura on es guarda la informació del fitxer de configuració
* @Ret: FleckConfig modificada amb la informació del fitxer de configuració
***********************************************/
FleckConfig FleckFunct_readConfigFleck(char* file_name, FleckConfig fleck_config_file){
    fd_fleck = open(file_name, O_RDONLY); //Obrim el fitxer de configuracio en mode lectura
 
    if(fd_fleck < 0){ //Si se ha produit un error al obrir el fitxer
        perror(ERROR_FITXER); //Es mostra un missatge d'error
    }
    fleck_config_file.user_name = Utils_readFileLimit(fd_fleck, '\n'); //Llegim el nom d'usuari
    FleckFunct_comprovarAmpresand(fleck_config_file); //Comprovem si el nom d'usuari conté '&'
    fleck_config_file.folder_name = Utils_readFileLimit(fd_fleck, '\n'); //Llegim el nom de la carpeta
    fleck_config_file.gotham_ip = Utils_readFileLimit(fd_fleck, '\n'); //Llegim la IP del Discovery
    fleck_config_file.gotham_port = Utils_readFileLimit(fd_fleck, '\n'); //Llegim el port del Discovery

    close(fd_fleck); //Tanquem el fitxer de configuracio
    return fleck_config_file;
    
}


/***********************************************
 * @Nom: FleckFunct_checkCommandFleck
 * @Definicio: Funció que comprova si la comanda introduida és correcta
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Arg: char* command: Comanda introduida per l'usuari
 * @Ret: int: 0 si la comanda és correcta, -1 si la comanda és incorrecta
 * ***********************************************/
 int FleckFunct_checkCommandFleck(int connected, char* command) {

    if (connected == 0) { //Si l'usuari no està connectat
        if (strcmp(command, "CONNECT") == 0) { //Si la comanda és CONNECT
            return 1; //Es retorna 1
        } else { //Si la comanda no és CONNECT
            return 0; //Es retorna 0
        }
    } else { //Si l'usuari està connectat
        return 1; //Es retorna 1
    }
 }



