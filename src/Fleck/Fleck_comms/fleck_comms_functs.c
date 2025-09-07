/***********************************************
*
* @Proposit: Fitxer encarregada de les funcions de comunicacions de Fleck.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio: 16/10/2024
*
************************************************/
//Include del .h associat a aquest .c
#include "fleck_comms_functs.h"

//Variables globals


/***********************************************
 * @Nom: FleckCommsFuncts_obrirDirectori
 * @Definicio: Funció que obre un directori
 * @Arg: char* path: Path del directori
 * @Arg: DIR* dir: Punter al directori
 * @Ret: ----
 * ***********************************************/
DIR* FleckCommsFuncts_obrirDirectori(char* path, DIR* dir) {
    char* current_dir = get_current_dir_name();     //Obtenim el directori actual
    

    if (current_dir == NULL) {
        perror("Error getting current directory");
        return NULL;
    }
    char* final_path = (char*)malloc(strlen(current_dir) + strlen(path) + 2);

    strcpy(final_path, current_dir);
    strcat(final_path, "/");
    strcat(final_path, path);
    final_path[strlen(final_path)-1] = '\0';


    dir = opendir(final_path);
    
    if (dir == NULL) {
        perror(ERROR_DIR); 
        if(final_path != NULL){
            free(final_path); //Alliberem la memòria de final_path
            final_path = NULL; //Posem final_path a NULL
        }
        if(current_dir != NULL){
            free(current_dir); //Alliberem la memòria de current_dir
            current_dir = NULL; //Posem current_dir a NULL
        }
        return NULL;
    }
    if(final_path != NULL){
        free(final_path); //Alliberem la memòria de final_path
        final_path = NULL; //Posem final_path a NULL
    }
    if(current_dir != NULL){
        free(current_dir); //Alliberem la memòria de current_dir
        current_dir = NULL; //Posem current_dir a NULL
    }
    return dir;
}


/***********************************************
 * @Nom: FleckComms_IsTextFile
 * @Definicio: Funció que comprova si un fitxer és de text
 * @Arg: char* file_name: Nom del fitxer
 * @Ret: int: 1 si és de text, 0 si no ho és
 * ***********************************************/
int FleckCommsFuncts_IsTextFile(char* file_name) {

    const char* text = strrchr(file_name, '.'); //Busquem l'últim punt de la cadena


    if (text != NULL) { //Si s'ha trobat un punt
        if (strcmp(text, ".txt") == 0) { //Si el fitxer és de text
            return 1; //Retornem 1
        } else {
            return 0; //Retornem 0
        }
    } else {
        return -1; //Retornem 0
    }
}


/***********************************************
 * @Nom: FleckCommsFuncts_ListTextMedia
 * @Definicio: Funció que llista els fitxers multimèdia o de text del servidor Fleck
 * @Arg: Path: Path dels fitxers multimèdia
 * @Arg: tipus: Tipus de fitxer
 * @Ret: ----
 * ***********************************************/ 
void FleckCommsFuncts_ListTextMedia(char *path, char* tipus) {
    DIR* dir = NULL; //Punter al directori
    struct dirent* file;
    int counter_files = 0;
    
    
    //Obrim el absolute path
    dir = FleckCommsFuncts_obrirDirectori(path, dir);

    if(dir == NULL) {
        printF("ERROR\n");
        return;
    }


    //Llegim els fitxers del directori
    while ((file = readdir(dir)) != NULL) {


        char* item = NULL;        
        if(file->d_name[0] == '.') { //Si el fitxer és un directori ocult
            continue; //Saltem el fitxer
        }
        if(!FleckCommsFuncts_IsTextFile(file->d_name) && strcmp(tipus, MEDIA) == 0) { //Si el fitxer no és de text
            counter_files++; //Incrementem el comptador de fitxers
            asprintf(&item, "%d: %s\n", counter_files, file->d_name);
            printF(item);
        }
        if(FleckCommsFuncts_IsTextFile(file->d_name) && strcmp(tipus, TEXT) == 0) { //Si el fitxer és de text
            counter_files++; //Incrementem el comptador de fitxers
            asprintf(&item, "%d: %s\n", counter_files, file->d_name);
            printF(item);     
        }
        if(item != NULL){
            free(item); //Alliberem la memòria de item
            item = NULL; //Posem item a NULL
        }
    }

    char* disponibilitat = NULL;  
    asprintf(&disponibilitat, "There are %d files %s available\n", counter_files, tipus);
    printF(disponibilitat);
    if(disponibilitat != NULL){
        free(disponibilitat); //Alliberem la memòria de disponibilitat
        disponibilitat = NULL; //Posem disponibilitat a NULL
    }

    if(file != NULL){
        free(file); //Alliberem la memòria de file
        file = NULL; //Posem file a NULL
    }  
    //Tanquem el directori
    closedir(dir);
}

/***********************************************
 * @Nom: FleckCommsFuncts_CheckSpaces
 * @Definicio: Funció que comprova si hi ha espais a la comanda i si en troba l'elimina
 * @Arg: char* command: Comanda introduida per l'usuari
 * @Ret: char*: Comanda sense espais
 * ***********************************************/
char* FleckCommsFuncts_CheckSpaces(char* command) {
    if (command == NULL) { //Si la comanda és NULL
        printF("Command is NULL\n");
        return NULL;
    }
    
    int i = 0; // Variable par recòrrer la comanda
    int j = 0; //Variable per recòrrer la comanda sense espais

    //Reservem memòria per a la comanda sense espais
    char *command_aux = (char*)malloc(sizeof(char) * (strlen(command) + 1)); 
    if (command_aux == NULL) { //Si ha fallat el malloc
        printF("Command_aux's malloc failed\n");
        return NULL;
    }

    while (command[i] != '\0') { //Mentre no arribem al final de la comanda
        if (command[i] != ' ') { //Si el caràcter no és un espai
            command_aux[j] = command[i];
            j++;
        }
        i++;
    }

    command_aux[j] = '\0';
    if((strcasecmp(strndup(command_aux, 7), "DISTORT") == 0) && ((strlen(command_aux) +2) == strlen(command))){
        strcpy(command, command_aux);
    
    } else if(((strlen(command_aux) == strlen(command)) && strcasecmp(command_aux, "LOGOUT") && strcasecmp(command, "CONNECT")) || (!(strcasecmp(strndup(command_aux, 7), "DISTORT")))){
        strcpy(command, "ERROR");
    } else {
        strcpy(command, command_aux); //Copiem la comanda sense espais a la comanda original
    }

    command[strlen(command)] = '\0';

    if(command_aux != NULL){
        free(command_aux);
        command_aux = NULL;
    }
    return command; //Retornem la comanda sense espais
}


/***********************************************
 * @Nom: FleckCommsFuncts_CheckFormat
 * @Definicio: Funció que comprova si la comanda té el format correcte
 * @Arg: char* command: Comanda introduida per l'usuari
 * @Ret: int: 1 si la comanda té el format correcte, 0 si no
 * ***********************************************/
int FleckCommsFuncts_CheckFormat(char* command) {

    //Busquem el punt que indica el començament de l'extensió del fitxer
    char* ext_num = strchr(command, '.');
    
    if (ext_num == NULL) { //Si no s'ha trobat el punt
        return 0; 
    }

    //Comprovem que la comanda tingui el format correcte
    if (strlen(ext_num) < 5) {
        return 0; 
    }

    //Comprovem que la resta de la comanda sigui un número
    for (size_t i = 5; i < strlen(ext_num); i++) {
        if(!isdigit(ext_num[i])) {
            return 0;
        }
    }
    return 1; //Retornem 1 si la comanda té el format correcte
}


int FleckCommsFuncts_comprovaMD5SUM(){
    return 0;
    // Si MD5SUM és correcte retorna 0
    // Si MD5SUM és incorrecte retorna -1
}


int FleckCommsFuncts_GestionaCaigudaWorker(char *mediaType, char *fileName, int fd_gotham, int fd_worker, char *userName, char *factorDist, char *fileSize, char *Md5){
    char trama[256];
    memset(&trama, '\0', 256);
    trames_createPeticioDistort(0x11, trama, mediaType, fileName);
    //Enviar la trama a Gotham ResumDistort
    if(Connexions_send(trama, fd_gotham) < 0){
        printF("Error sending the trama to Worker server\n");
        close(fd_gotham);
    }
    //Rebre la resposta de gotham
    char tramaResposta[256];
    memset(&tramaResposta, '\0', 256);
    memcpy(&tramaResposta, Connexions_read(fd_gotham, tramaResposta), 256);
    //Obtenir la IP i port del nou worker

    char *data = trames_obteData(tramaResposta);
    if(strcmp(data, "DISTORT_KO") == 0){
        printF("\nNo hi ha workers disponibles per continuar amb la distorsió\n");
        return -1;
    } else {
        char *newIp = strtok(data, "&"); 
        char *newPort = strtok(NULL, "&");
        char tramaCon[256];
        memset(&tramaCon, '\0', 256);
        trames_createEnviarDistort(tramaCon, userName, fileName, fileSize, Md5, factorDist);
        
        fd_worker = Connexions_connectServer(atoi(newPort), newIp);
        sleep(5);
        if(Connexions_send(tramaCon, fd_worker) < 0){
            printF("Error sending the trama to Worker server 2\n");
            close(fd_worker);

        } else {
            printF("\nUsuari assignat a un nou Worker amb éxit\n");
        }

    }
    return fd_worker;
}





void FleckCommsFuncts_MostraBarraProgres(int percentatge) {
    int size = 25;  // Largo de la barra en caracteres
    int progres = (percentatge * size) / 100;  // Calcula el progres
    printF("|");  // Inicio de la barra
    
    for (int i = 0; i < size; i++) {
        if (i < progres) {
            printF("*");  // Parte completada
        } else {
            printF(" ");  // Parte no completada
        }
        printF("|");
        
    }
    printF("\n\n");
   
}



int FleckCommsFuncts_EnviarDesconnexioWorker(int fd_worker, char *userName, LinkedList *info_status, InfoStatus elementStatus){
    char trama[256];
    memset(&trama, '\0', 256);
    trames_createTramaLogout(trama, userName);

    //Quan s'acaba la distorsió, cal desconnectar-se del worker
    if(Connexions_send(trama, fd_worker) < 0){
        perror("Error sending the trama to Gotham server\n");
        return -1;
    }

    //Actualitzar l'estat de distorsió del fitxer
    GestioSempahore_semaphoreWait();
    elementStatus.estatDistort = 2; //2 = distorsionat al 100%
    LINKEDLIST_update(info_status, elementStatus);
    GestioSempahore_semaphoreSignal();
    return 0;

}



int FleckCommsFuncts_EnviarTramaResumeDistort(int fd_worker){
        //llegir trama OK 0x03
        char trama[256];
        memset(&trama, '\0', 256);
        if(Connexions_read(fd_worker, trama) < 0){
            perror("Error al llegir la trama\n");
            return -1;
        } else {
            if(trames_obteTramaType(trama) != DISTORT_INFO_FITXER_SEND){
                printF("Error al recuperar l'enviament del fitxer\n");
                return -1;
            }
        }

    return 0;
}