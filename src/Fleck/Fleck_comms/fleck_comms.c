/***********************************************
 *
 * @Proposit: Fitxer encarregada de les comunicacions de Fleck.
 * @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
 * @Data creacio: 10/10/2024
 * @Data ultima modificacio: 16/10/2024
 *
 ************************************************/
// Include del .h associat a aquest .c
#include "fleck_comms.h"
#include "fleck_comms_functs.h"
#include "../../Moduls_compartits/trames/trames.h" //Include del .h de les trames



int connected = 0; // Variable que indica si l'usuari està connectat o no
int fd_gotham = 0;
LinkedList *info_status = NULL;

//Threads
pthread_t thread_harley;
pthread_t thread_enigma;
pthread_t thread_heartbeats;





void FleckComms_EsperaFinalizacioThreads() {
    int threadAcabat_harley = 0;
    int threadAcabat_enigma = 0;
    
    while(threadAcabat_harley == 0 && threadAcabat_enigma == 0){
        if(pthread_join(thread_harley, NULL) == 0){
            threadAcabat_harley = 1;
        } 
        if(pthread_join(thread_enigma, NULL) == 0){
            threadAcabat_harley = 1;
        } 
    }
}

/***********************************************
 * @Nom: FleckComms_ExecuteCommandFleck
 * @Definicio: Funció que executa la comanda introduida per l'usuari
 * @Arg: char* command: Comanda introduida per l'usuari
 * @Arg: int logout: Valor que indica si l'usuari vol sortir del sistema
 * @Arg: char* path: Path dels fitxers a utilitzar
 * @Ret: int logout: serà 1 si l'usuari vol sortir del sistema
 * ***********************************************/
int FleckComms_ExecuteCommandFleck(char *command, int logout, char *path, char *port, char *ip, char *userName) {
    connected = FleckFunct_checkCommandFleck(connected, command); // Comprovem si la comanda és correcta
    command[strlen(command)] = '\0';

    command = FleckCommsFuncts_CheckSpaces(command); // Comprovem si hi ha espais a la comanda
    //delarar el semafor
    GestioSempahore_initSemaphore(1);
    if(info_status == NULL){
        info_status = LINKEDLIST_create();
    }

    // Comprovem quina comanda ha introduit l'usuari
    if (strcasecmp(command, "CONNECT") == 0) {
        connected = FleckComms_ConnectFleck(connected, port, ip, userName); // Connectem amb el servidor Fleck

    } else if (strcasecmp(command, "LOGOUT") == 0) {
        connected = FleckComms_LogoutFleck(connected, userName);
        logout = 1;
    } else if (strcasecmp(command, "LISTMEDIA") == 0) {
        FleckComms_ListMediaFleck(path, connected);
    }    else if (strcasecmp(command, "LISTTEXT") == 0) {
        FleckComms_ListTextFleck(path, connected);
    } else if (strcasecmp(strndup(command, 7), "DISTORT") == 0) {
        FleckComms_DistortFleck(command, connected, userName);
    } else if (strcasecmp(command, "CHECKSTATUS") == 0) {
        FleckComms_CheckStatusFleck(connected, LINKEDLIST_getSize(info_status));
    } else if (strcasecmp(command, "CLEARALL") == 0) {
        FleckComms_ClearAllFleck(connected);
    } else {
        printF(ERROR_COMMAND); // Mostrem un missatge d'error si la comanda no és correcta
    } 

    return logout; // Retornem el valor de logout
}

/***********************************************
 * @Nom: FleckComms_ConnectFleck
 * @Definicio: Funció que connecta amb el servidor Fleck
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
int FleckComms_ConnectFleck(int connected, char *port, char *ip, char *userName) {
    if (connected == 1){
        printF("You are already connected to Mr. J System, chaos lover :)\n");
    }else{
        fd_gotham = Connexions_connectServer(atoi(port), ip);
        userName[strlen(userName)] = '\0';
        ip[strlen(ip)] = '\0';
        port[strlen(port)] = '\0';

        char trama[256];
        memset(&trama, '\0', 256);
        trames_createTramaConnection(trama, userName, ip, port);

        if(Connexions_send(trama, fd_gotham) < 0){
            perror("Error sending the trama to Gotham server\n");
            exit(1);
        }

        char tramaRebuda[256];
        memset(&tramaRebuda, '\0', 256);
        memcpy(&tramaRebuda, Connexions_read(fd_gotham, tramaRebuda), 256);

        if(trames_obteTramaType(tramaRebuda) == CONNEXIO_FLECK_GOTHAM){
            char*data = trames_obteData(tramaRebuda);
            if(strcmp(data, "CON_KO") == 0){
                printF("Connection KO\n");
            } else {
                printF("You connected to Mr. J System. Let the chaos begin!:)\n");
                
                pthread_t gotham_thread;
                GothamThread *info_gotham_thread = malloc(sizeof(GothamThread));
                info_gotham_thread->fd = fd_gotham;
                if(pthread_create(&gotham_thread, NULL, FleckComms_handleHeartbeatsGotham, (void *)info_gotham_thread) != 0){
                    perror("Error al crear el thread de Fleck\n");
                    exit(1);
                }

                connected = 1;

            }

            
        } else {
            printF("Connection KO\n");
        }
    }
    return 1;
}

/***********************************************
 * @Nom: FleckComms_LogoutFleck
 * @Definicio: Funció que desconnecta del servidor Fleck
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: retorna 0 per indicar que l'usuari s'ha desconnectat
 * ***********************************************/
int FleckComms_LogoutFleck(int connected, char *fleck_userName) {

    if (connected == 0){
        printF("You can't disconnect from Mr. J System, because you are not connected, chaos lover :)\n");
    } else {
        
        char trama[256];
        memset(&trama, '\0', 256);
        trames_createTramaLogout(trama, fleck_userName);
        Connexions_send(trama, fd_gotham);

        char tramaRebuda[256];
        memset(&tramaRebuda, '\0', 256);
        memcpy(&tramaRebuda, Connexions_read(fd_gotham, tramaRebuda), 256);

        if(trames_obteTramaType(tramaRebuda) == DISCONNECT){
            printF("You have been disconnected from Mr. J System, chaos lover :)\n");
            connected = 0;
        } else {
            printF("Error disconnecting from Mr. J System, chaos lover :)\n");
        }

        printF("Thanks for using Mr. J System, see you soon, chaos lover :)");
    }
    return 0;
}

/***********************************************
 * @Nom: FleckComms_ListMediaFleck
 * @Definicio: Funció que llista els fitxers multimèdia del servidor Fleck
 * @Arg: char* path: Path dels fitxers multimèdia
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
void FleckComms_ListMediaFleck(char *path, int connected) {
    path[strlen(path)] = '/';
    if (connected == 0){
        printF("You must be connected to Mr. J System to list the media files, chaos lover :)\n");
        return;
    }else{
        FleckCommsFuncts_ListTextMedia(path, MEDIA);
        return;
    }
}

/***********************************************
 * @Nom: FleckComms_ListTextFleck
 * @Definicio: Funció que llista els fitxers de text del servidor Fleck
 * @Arg: char* path: Path dels fitxers de text
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
void FleckComms_ListTextFleck(char *path, int connected){
    path[strlen(path)] = '/';
    if (connected == 0)    {
        printF("You must be connected to Mr. J System to list the text files, chaos lover :)\n");
        return;
    }else{
        FleckCommsFuncts_ListTextMedia(path, TEXT);
        return;
    }
}

/***********************************************
 * @Nom: FleckComms_DistortFleck
 * @Definicio: Funció que distorsiona un fitxer del servidor Fleck
 * @Arg: char* file_name: Nom del fitxer a distorsionar
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Arg: char *userName: Nom de l'usuari
 * @Ret: ----
 * ***********************************************/
void FleckComms_DistortFleck(char *file_n, int connected, char *userName){
    if (connected == 0){
        printF("Cannot distort, you are not connected to Mr. J System\n");
        return;
    }

    char *file_name = strdup(file_n);
    // Saltamos los primeros 7 caracteres ("distort")
    char *base_name = file_name + 7;

    // Buscar fins el punt per poder separar
    char *fins_punt = strrchr(base_name, '.');
    if (fins_punt == NULL){
        printF("Error: File without extension.\n");
        return;
    }

    // Buscar el la quantitat de distorsions saben que la distorisió sempre té 4 caràters (.XXX)
    char *num_pos = fins_punt + 4;

    // Assignar longituds i memòria a les cadenes
    size_t name_length = fins_punt - base_name;
    size_t ext_length = 4; // ".png", ".jpg", etc.

    char *file = (char *)malloc(name_length + ext_length + 1);
        if (!file) { perror("malloc file"); exit(1); }

    char *extensio = (char *)malloc(ext_length + 1);
    if (!extensio) { perror("malloc extensio"); exit(1); }

    size_t num_length = strlen(num_pos);
    char *factor = (char *)malloc(num_length + 1);
    if (!factor) { perror("malloc factor"); exit(1); }

    strncpy(file, base_name, name_length);
    file[name_length] = '\0';

    strncpy(extensio, fins_punt, ext_length);
    extensio[ext_length] = '\0';

    strncpy(factor, num_pos, num_length);
    factor[num_length] = '\0';

    // ✅ Concatenar extensio a file de forma segura
    strcat(file, extensio);
    

    char *path = NULL;
    asprintf(&path, "Media_i_text/%s", file);
    // Comprovar si el fitxer existeix
    //abrir el fichero -> si no se puede abrir -> error
    int fd_file = open(path, O_RDONLY);
    if (fd_file == -1){
        printF("El fitxer no existeix\n");
        close(fd_file);
        return;
    }   

    close(fd_file);
    // Enviar tipus de distortsio a Gotham
    char *type = NULL;
    if (strcmp(extensio, ".txt") == 0){
        asprintf(&type, "%s", TEXT);
    } else{
        asprintf(&type, "%s", MEDIA);
    }
    type[strlen(type)] = '\0';
    char trama[256];   

    //1. Crear peticio distort que s'enviarà a gotham
    memset(&trama, '\0', 256);
    trames_createPeticioDistort(0x10, trama, type, file);
    if(fd_gotham > 0) {
        if(Connexions_send(trama, fd_gotham) < 0){
            printF("Error sending the trama to Gotham server (send error)\n");
            return;
        } 
    } else {
        printF("Error enviant la trama a Gotham\n");
        exit(1);
    }

    //2. Llegir resposta de gotham
    char cadena[256];
    memset(&cadena, '\0', 256);
    memcpy(&cadena, Connexions_read(fd_gotham, cadena), 256);
    char *data = trames_obteData(cadena);

    if(strcmp(data, "DISTORT_KO") == 0){
        printF("No hi ha workers disponibles, s'ha aturat la distorsió\n");
        return;
    }

    //Extreure ip i port per poder després crear la trama
    char* ip_hf = strtok(data, "&");
    char* port_hf = strtok(NULL, "&");
    int fd_harley_enigma = Connexions_connectServer(atoi(port_hf), ip_hf); // Connectar a HarleyEnigma
    HarleyEnigmaThread *harleyEnigmaThread;

    harleyEnigmaThread = (HarleyEnigmaThread *)malloc(sizeof(HarleyEnigmaThread));
    harleyEnigmaThread->fd = fd_harley_enigma;
    harleyEnigmaThread->userName = userName;
    harleyEnigmaThread->file = file;
    harleyEnigmaThread->factor = factor;
    if(trames_obteTramaType(cadena) == DISTORT_GOTHAM_FLECK && (trames_verificarChecksum(cadena) == 0)){
        if(pthread_create(&thread_harley, NULL, FleckComms_handleHarleyClient, harleyEnigmaThread) != 0){
            perror("Error creating thread\n");
            return;
        }

        /*if(pthread_join(thread_harley, NULL) != 0){
            perror("Error joining thread\n");
            return;
        }*/

    } else {
        printF("Connection with worker failed\n");
        //close(fd_harley_enigma);
        return;
    }

    return;
}

/***********************************************
 * @Nom: FleckComms_CheckStatusFleck
 * @Definicio: Funció que comprova l'estat del servidor Fleck
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
void FleckComms_CheckStatusFleck(int connected, int size){
    InfoStatus elementStatus;
    
    int prcntAux, percentatge = 0;
    if (connected == 0){
        printF("You must be connected to Mr. J System to check the status, chaos lover :)\n");

    } else {
        GestioSempahore_semaphoreWait();
        LINKEDLIST_goToHead(info_status);
        if(LINKEDLIST_getSize(info_status) == 0){
            printF("You have no ongoing or finished distorsions\n");
            GestioSempahore_semaphoreSignal();
            return;
        }
        while(!LINKEDLIST_isAtEnd(*info_status)){

            
            elementStatus = LINKEDLIST_get(info_status);
            prcntAux = (elementStatus.n_bytes * 100) / elementStatus.fileSize;

            if(elementStatus.estatDistort == 0){
                percentatge = prcntAux / 2;     //Inferior al 50% perque s'està enviant el fitxer
            } else if(elementStatus.estatDistort == 1) {
                percentatge = (prcntAux/2) + 50; //50% perque ja s'ha enviat tot el file al worker
            } else {
                //Distorsio acabada
                percentatge = 100;
            }


            //Printar "%s (%d%%):\n", elementStatus.fileName, percentatge
            char *pantalla = NULL;
            asprintf(&pantalla, "%s (%d%%):\n", elementStatus.fileName, percentatge);
            write(1, pantalla, strlen(pantalla));
            free(pantalla);
            
            FleckCommsFuncts_MostraBarraProgres(percentatge);
            LINKEDLIST_next(info_status);
        }
        GestioSempahore_semaphoreSignal();
    }

    return;
}

/***********************************************
 * @Nom: FleckComms_ClearAllFleck
 * @Definicio: Funció que esborra les distorsions ja acabades del servidor Fleck
 * @Arg: int connected: Variable que indica si l'usuari està connectat o no
 * @Ret: ----
 * ***********************************************/
void FleckComms_ClearAllFleck(int connected){
    if (connected == 0){
        printF("You must be connected to Mr. J System to clear all the distortions, chaos lover :)\n");
        return;
    }else{

        GestioSempahore_semaphoreWait();
        LINKEDLIST_goToHead(info_status);
        while(!LINKEDLIST_isAtEnd(*info_status)){
            InfoStatus elementStatus = LINKEDLIST_get(info_status);
            if(elementStatus.estatDistort == 2){
                LINKEDLIST_remove(info_status);
            } else {
                LINKEDLIST_next(info_status);
            }
        }
        GestioSempahore_semaphoreSignal();
        printF("All distortions have been cleared, Mr. J System is ready for more chaos! :)\n");
        return;
    }
}

void* FleckComms_handleHeartbeatsGotham(void* arg) { 
    char trama[256];
    GothamThread *gothamThread = (GothamThread *)arg;   
    int fd = gothamThread->fd;
    //Enviar heartbeat a gotham per veure si ha caigut

    while(1){
        memset(&trama, '\0', 256);
        trames_createHeartBeat(trama);
        if(Connexions_send(trama, fd) < 0){
            printF("\nGotham ha caigut\n");
            printF("Finalitzant distorsions...\n");
            break;
        }
        sleep(5);
        //Enviem la trama a Gotham per connectar-nos  
    }

    kill(getpid(), SIGUSR1); 
    
    //return NULL;
    close(fd_gotham);   
    pthread_exit(NULL);
}

void *FleckComms_handleHarleyClient(void* arg) {
    HarleyEnigmaThread *workerThread = (HarleyEnigmaThread *)arg;
    int fd_worker = workerThread->fd;
    char *userName = workerThread->userName;
    char *file = workerThread->file;
    char  *pathDist = NULL;
    char *factor = workerThread->factor;
    int bytesEscrits, contBytes, bytesRead, cont, nTrames = 0;
    bytesRead = 0;
    char tramaRebuda[256];
    char tramaEnviar[256];
    InfoStatus elementStatus;

    memset(&tramaRebuda, '\0', 256);

    //Fleck s'acaba de connectar a Harley
    //Enviar la trama per distorsionar el fitxer
    char *path = NULL;
    asprintf(&path, "Media_i_text/%s", file);
    int fileSize = utils_getFileSize(path);
    char *fileSizeStr = utils_intToChar(fileSize);
    
    char MD5SUM[32];
    strcpy(MD5SUM, utils_getMD5SUMfromFIle(path));
    memset(&tramaEnviar, '\0', 256);
    trames_createEnviarDistort(tramaEnviar, userName, file, fileSizeStr, MD5SUM, factor);

    if(Connexions_send(tramaEnviar, fd_worker) < 0){
        printF("Error sending the trama to Worker server 24\n");
        // close(fd_worker);
    } else {
        printF("Connexió establerta amb un Worker\n");
    }

    //Guardar info a la linked per fer checkstatus
    GestioSempahore_semaphoreWait();
    int idLinked = LINKEDLIST_getSize(info_status);

    //anar fins al final de la linked per afegir el nou element
    while(!LINKEDLIST_isAtEnd(*info_status)){
        LINKEDLIST_next(info_status); 
    }

    elementStatus.fileName = file;
    elementStatus.fileSize = utils_getFileSize(path);
    elementStatus.factor = atoi(factor);
    elementStatus.estatDistort = 0;
    elementStatus.n_bytes = 0;
    elementStatus.id = idLinked;
    LINKEDLIST_add(info_status, elementStatus); //Afegit
    GestioSempahore_semaphoreSignal();

    while(fd_worker > 0) {
        memset(&tramaRebuda, '\0', 256);
        // memcpy(&tramaRebuda, Connexions_read(fd_worker, tramaRebuda), 256);
        if (Connexions_read(fd_worker, tramaRebuda) == NULL) {
            printF("Error al leer datos del socket\n");
            break; // o return / pthread_exit
        }

        int type = trames_obteTramaType(tramaRebuda);
        //Comprovar que el checksum de la trama sigui correcte

        if(trames_verificarChecksum(tramaRebuda) == 0){
            type = trames_obteTramaType(tramaRebuda);       
        } else {
            type = WRONG_CHECKSUM;
        }
    
        switch (type){
            case DISTORT_INFO_FITXER_SEND:
                printF("\nEnviant fitxer...\n");
                int fd_file = open(path, O_RDONLY);
            
                if (fd_file == -1) {
                    perror("Error obrint el fitxer");
                    //return -1;
                }

                // ir al inicio del archivo
                lseek(fd_file, 0, SEEK_SET);

                int sizeToRead = 247; // 256 - 9 = 247 -> el -9 es del que ocupen timestamp i type chechsum data_length
                char *data = (char*) malloc (sizeof (char) * sizeToRead);
                char *data_anterior = (char*) malloc (sizeof (char) * sizeToRead);
                

                while (bytesRead < fileSize) { 
                    memset(data, '\0', sizeToRead);
                    memset(data_anterior, '\0', sizeToRead);
                    strcpy(data_anterior, data);
                    cont = read(fd_file, data, sizeToRead);
                    bytesRead = bytesRead + cont;   
                    
                    memset(&tramaEnviar, '\0', 256);
                    trames_createEnviarFitxer(tramaEnviar, data);
                    char *pathDebug = NULL;
                    asprintf(&pathDebug, "%s", file);

                    sleep(1);
                    if(Connexions_send(tramaEnviar, fd_worker) < 0){
                        printF(". Ha caigut el Worker\n");
                        fd_worker = FleckCommsFuncts_GestionaCaigudaWorker("MEDIA", file, fd_gotham, fd_worker, userName, factor, fileSizeStr, MD5SUM);

                        if(fd_worker == -1){
                            printF("L'usuari ha sigut desconnectat del worker\n");
                            pthread_exit(NULL);
                        } else {
                            if(FleckCommsFuncts_EnviarTramaResumeDistort(fd_worker) != 0){
                                pthread_exit(NULL);
                                return NULL;
                            }
                            printF("Reprenent la distorsió...\n");
                        }

                        sleep(5);
                        bytesRead = 247*(nTrames-1); //Ajustar el cursor pq amb el cntr + c s'han perdut dues trames
                        lseek(fd_file, bytesRead, SEEK_SET); 
                        
                        
                    } else {
                        nTrames++;
                        GestioSempahore_semaphoreWait();   
                        elementStatus.n_bytes = bytesRead;
                        LINKEDLIST_update(info_status, elementStatus);      
                        GestioSempahore_semaphoreSignal();
                    }
                }

                bytesEscrits = contBytes = bytesRead = cont = nTrames = 0;
                close(fd_file);

                break;

            case DISTORT_MD5SUM_CHECK:
                
                if(strcmp(trames_obteData(tramaRebuda), "CHECK_OK") == 0){
                    printF("\nPreparat per rebre fitxer distorsionat\n");
                    bytesEscrits = 0;
                    contBytes = 0;
                    GestioSempahore_semaphoreWait();   
                    elementStatus.n_bytes = 0;
                    elementStatus.estatDistort = 1;
                    LINKEDLIST_update(info_status, elementStatus);       
                    GestioSempahore_semaphoreSignal();
                    
                } else {
                    printF("El Worker ha rebut malament el fitxer a distorsionar\n");
                    memset(&tramaEnviar, '\0', 256);
                    trames_createTramaLogout(tramaEnviar, userName);
                    if(Connexions_send(tramaEnviar, fd_worker) < 0){
                        perror("Error sending the trama to Gotham server\n");
                    } else {
                        printF("Usuari desconnectat del Worker\n");
                    }
                    close(fd_worker);
                    close(fd_file);
                    pthread_exit(NULL);
                    return NULL;
                }

                break;

            case DISTORT_INFO_FITXER_RECIVE:
                char *fileSizeDist, *md5Dist = NULL;
                char *dataDist = trames_obteData(tramaRebuda);
                //obtenir el fileSize i el md5sum de la trama rebuda
                fileSizeDist = strtok(dataDist, "&");
                md5Dist = strtok(NULL, "&");
                GestioSempahore_semaphoreWait();
                elementStatus.fileSize = atoi(fileSizeDist); //Modificar el nou tamany del fitxer (servirà pel check status)
                LINKEDLIST_update(info_status, elementStatus);       
                GestioSempahore_semaphoreSignal();
                asprintf(&pathDist, "Distorsions/%s_%s", userName, file);
                break;

            case DISTORT_RECIVE_FITXER:
                int sizeToWrite = 247;
                
                fd_file = open(pathDist, O_CREAT | O_APPEND | O_WRONLY, 0666);

                if (fd_file == -1) {
                    perror("Error obrint el fitxer");  
                    exit(1);
                    return NULL;
                }
            
                if(bytesEscrits + sizeToWrite > atoi(fileSizeDist)){
                    sizeToWrite = atoi(fileSizeDist) - bytesEscrits;
                }   

                contBytes = write(fd_file, trames_obteData(tramaRebuda), sizeToWrite);

                if(contBytes == -1){
                    perror("Error al escriure al fitxer\n");
                    exit(1);
                }

                bytesEscrits = bytesEscrits + contBytes;

                if(bytesEscrits >= atoi(fileSizeDist)){
                    if(utils_comparaMD5SUM(fd_file, md5Dist, pathDist) == 0){
                        printF("Fitxer a distorsionar rebut correctament, distorsió finalitzada\n");
                        memset(&tramaEnviar, '\0', 256);
                        trames_createFinalFitxer(tramaEnviar, 0);
                    } else {
                        SO_deleteImage(pathDist);
                        printF("MD5SUM incorrecte\n");
                        printF("S'ha rebut malament el fitxer a distorsionar, distorsió finalitzada\n");
                        memset(&tramaEnviar, '\0', 256);
                        trames_createFinalFitxer(tramaEnviar, 0);
                    }

                    //Enviar la trama corresponent
                    if(Connexions_send(tramaEnviar, fd_worker) < 0){
                        perror("Error al enviar la trama\n");
                        exit(1);
                    }

                    //Enviar trama disconnect quan s'acaba la distorsió
                    if(FleckCommsFuncts_EnviarDesconnexioWorker(fd_worker, userName, info_status, elementStatus) == 0){
                        printF("\nL'usuari ha sigut desconnectat del worker\n");
                    } else {
                        printF("Error al desconnectar l'usuari del worker\n");
                    }                    
                    close(fd_file);
                    close(fd_worker);
                    // close(fd_worker);
                    pthread_exit(NULL);
                    return NULL;
                }

                //Actualitzat estat distort
                GestioSempahore_semaphoreWait();
                elementStatus.n_bytes = bytesEscrits;
                LINKEDLIST_update(info_status, elementStatus);
                GestioSempahore_semaphoreSignal();

                close(fd_file);
                break;

            case WRONG_DATA:
                printF("El worker a rebut una trama errònia, parant la distorsió\n");
                close(fd_worker);
                if(close(fd_worker) < 0){
                    perror("Error al tancar el socket\n");
                    exit(1);
                }

                pthread_exit(NULL);
                return NULL;
                break;


            case WRONG_CHECKSUM:
                printF("WRONG_CHECKSUM, error en el checksum\n");

                //Enviar la trama de error de checksum i parar la distorsió
                memset(&tramaEnviar, '\0', 256);
                trames_TramaErronia(tramaEnviar);
                
                if(Connexions_send(tramaEnviar, fd_worker) < 0){
                    perror("Error al enviar la trama al servidor Gotham 5\n");
                    exit(1);
                }
                close(fd_worker);
                if(close(fd_worker) < 0){
                    perror("Error al tancar el socket\n");
                    exit(1);
                }

                pthread_exit(NULL);
                return NULL;
                break;
        }
        
    }
    close(fd_worker);
    pthread_exit(NULL);
    return NULL;
}


