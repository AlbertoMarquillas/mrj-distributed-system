/***********************************************
*
* @Proposit: Fitxer de funcions auxiliars per a Enigma i Enigma.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 11/10/2024
* @Data ultima modificacio: 16/10/2024
*
************************************************/
//Include del .h associat a aquest .c
#include "enigma_funct.h"


/***********************************************
* @Nom: EnigmaEnigmaFunct_readConfigEnigma
* @Definicio: Funció que llegeix el fitxer de configuració i guarda la informació en una estructura
* @Arg: char* file_name: Nom del fitxer de configuració
* @Arg: EnigmaEnigmaConfig *Enigma_config_file: Estructura on es guarda la informació del fitxer de configuració
* @Ret: int: 0 si tot ha anat bé, -1 si hi ha hagut algun error
***********************************************/
int EnigmaFunct_readConfigEnigma(char* file_name, EnigmaConfig *enigma_config_file) {
    int fd_enigma = open(file_name, O_RDONLY); //Obrim el fitxer de configuracio en mode lectura
    if(fd_enigma < 0){ //Si hi ha hagut algun error
        perror(ERROR_FITXER); //Es mostra un missatge d'error
        return -1; //Retornem -1 per indicar que hi ha hagut un error
    }
    enigma_config_file->gotham_ip = Utils_readFileLimit(fd_enigma, '\n'); //Llegim la IP de Gotham
    enigma_config_file->gotham_port = Utils_readFileLimit(fd_enigma, '\n'); //Llegim el port de Gotham
    enigma_config_file->fleck_ip = Utils_readFileLimit(fd_enigma, '\n'); //Llegim la IP de Fleck
    enigma_config_file->fleck_port = Utils_readFileLimit(fd_enigma, '\n'); //Llegim el port de Fleck
    enigma_config_file->folder_name = Utils_readFileLimit(fd_enigma, '\n'); //Llegim el nom de la carpeta
    enigma_config_file->type = Utils_readFileLimit(fd_enigma, '\n'); //Llegim el tipus de fitxer
    enigma_config_file->type[strlen(enigma_config_file->type)] = '\0'; //Eliminem el salt de línia del tipus de fitxer
    
    close(fd_enigma); //Tanquem el fitxer de configuracio
    return 0;
}



void EnigmaFunct_eliminaCua(int msgid) {
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la cola de mensajes");
        exit(1);
    }
}




int EnigmaFunct_accedeixCua(int type){
    //Si type == 0 -> Cua de Fleck
    //Si type == 1 -> Cua de Enigma
    //generar clau
    key_t key = -1;
    if(type == 0){
        key = ftok(KEY_INFO_FLECK, ID_INFO_FLECK);
    } else {
        key = ftok(KEY_CONT_ENIGMA, ID_CONT_ENIGMA);
    }

    if (key == -1) {
        perror("Error al generar la clau de la cua");
        return -1;
    }

    //Borrar i crear la cua de missatges
    int msgID = msgget(key, IPC_CREAT | 0666);
    if (msgID == -1) {
        perror("Error al crear la cola de mensajes");
        return -1;
    }
    return msgID;
}


int EnigmaFunct_ConnectToGotham(EnigmaConfig enigma_config_file, LinkedList* cua){

    char trama[MAX_PATH];
    memset(&trama, '\0', MAX_PATH);

    int fd_gotham = Connexions_connectServer(atoi(enigma_config_file.gotham_port), enigma_config_file.gotham_ip);
    //Ens connectem a Gotham
    trames_createTramaConnection(trama, enigma_config_file.type, enigma_config_file.fleck_ip, enigma_config_file.fleck_port);

    if(Connexions_send(trama, fd_gotham) < 0){
        perror("Error al enviar la trama al servidor Gotham\n");
        close(fd_gotham);
        return -1;
    } //Enviem la trama a Gotham per connectar-nos

    
    memset(&trama, '\0', MAX_PATH);
    memcpy(&trama, Connexions_read(fd_gotham, trama), MAX_PATH);

    if(trames_obteTramaType(trama) == CONNEXIO_WORKER_GOTHAM){
        char*data = trames_obteData(trama);
        if(strcmp(data, "CON_KO") == 0){
            printF("No ha sigut possible connectar amb el servidor Gotham\n");
            if(close(fd_gotham) < 0){
                perror("Error al tancar el socket\n");
                return -1;
            }
        } else {
            printF("\nConnexió establerta amb Gotham\n");
        }
    } else {
        printF("No ha sigut possible connectar amb el servidor Gotham\n");
        if(close(fd_gotham) < 0){
            perror("Error al tancar el socket\n");
            return -1;
        }
    }

    return fd_gotham;
}


int EnigmaFunct_esperaWorkerPrincipal(int fd_gotham){
    char trama[MAX_PATH];
    memset(&trama, '\0', MAX_PATH);
    memcpy(&trama, Connexions_read(fd_gotham, trama), MAX_PATH);
    if(trames_obteTramaType(trama) == NEW_WORKER){
        printF("Ha sigut assignat com a worker principal\n");
    } else {
        printF("Error al rebre el worker principal\n");
        if(fd_gotham > 0){
            if(close(fd_gotham) < 0){
                perror("Error al tancar el socket\n");
                return -1;
            }

            return -1;
        }
    }

    return 0;
}



void eliminarFitxersMediaFiles() {
    struct dirent *entry;
    char *path = "Text";

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error obrint directori");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Crear el path completo para cada archivo
        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        // Eliminar archivo
        if (remove(fullPath) != 0) {
            perror("Error eliminant fitxer");
        }
    }

    closedir(dir);
}