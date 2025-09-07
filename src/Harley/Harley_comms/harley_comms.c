/***********************************************
*
* @Proposit: Fitxer encarregada de les comunicacions de Harley.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 21/10/2024
* @Data ultima modificacio:
*
************************************************/

//Include del .h associat a aquest .c
#include "harley_comms.h"


void* HarleyComms_threadFleck(void* arg) {
    ContingutFleck contingutFleck;
    InfoThread *info_thread = (InfoThread *)arg;
    
    FleckThread *fleckThread = info_thread->info_fleck;
    int fd = fleckThread->fd;
    int fleck_id = fleckThread->fleck_id;
    char *folderName = info_thread->folderName;
    int disconnect = 0;
    
    CuaInfoFleck cuaMsg;
    cuaMsg.fleck_id = -1;

    if (info_thread == NULL || info_thread->info_fleck == NULL) {
        write(2, "Error: info_thread o info_fleck és NULL\n", 40);
        pthread_exit(NULL);
    }
    

    char tramaRebuda[256], tramaEnviar[256];
    int type = -1;
    while(disconnect == 0) {
        memset(&tramaRebuda, '\0', 256);
        memcpy(&tramaRebuda, Connexions_read(fd, tramaRebuda), 256);
        //Comprovar que el checksum de la trama sigui correcte
        if(trames_verificarChecksum(tramaRebuda) == 0){
            type = trames_obteTramaType(tramaRebuda);       
        } else {
            type = WRONG_CHECKSUM;
        }

        switch(type){
            case CONNEXIO_FLECK_WORKER:
                printF("\nConnexió amb Fleck establerta\n");
                char *dataFitxer = trames_obteData(tramaRebuda);
                memset(&contingutFleck.userName, '\0', 256);
                strcpy(contingutFleck.userName, strtok(dataFitxer, "&"));

                char *fileName = strtok(NULL, "&");
                char *pathAux =NULL;

                char *fileName_modificat = NULL;
                asprintf(&fileName_modificat, "%s_%s", contingutFleck.userName, fileName);
                asprintf(&pathAux, ".%s/%s", folderName, fileName_modificat);
                memset(&contingutFleck.path, '\0', 256);
                strcpy(contingutFleck.path, pathAux);

                memset(&contingutFleck.fileName, '\0', 256);
                strcpy(contingutFleck.fileName, fileName_modificat);

                char *fileSizeST = strtok(NULL, "&");
                int size = atoi(fileSizeST);

                memset(&contingutFleck.md5sum, '\0', 32);
                strcpy(contingutFleck.md5sum, strtok(NULL, "&"));
                contingutFleck.md5sum[strlen(contingutFleck.md5sum)] = '\0';
                
                contingutFleck.factor = atoi(strtok(NULL, "&"));
                contingutFleck.fileSize = size;
                
                char *pantalla = NULL;
                asprintf(&pantalla, "\nCal distorsionar el fitxer %s de l'usuari %s amb factor de distorsio %d\n", fileName, contingutFleck.userName, contingutFleck.factor);
                write(1, pantalla, strlen(pantalla));
                free(pantalla);
                
                cuaMsg = HarleyCommsFuncts_actualitzaPunterLinked(info_thread->linked_cua, fleck_id);
                
                if (cuaMsg.estatDistort == 1) {
                    printF("Reenviant fitxer\n");
                    if(HarleyCommsFuncts_enviarFitxer(fd, info_thread, cuaMsg, fleck_id, contingutFleck)==0){
                        printF("Fitxer enviat correctament, distorsió finalitzada\n");
                    } else {
                        printF("Error enviant el fitxer\n");
                    }
                } else {
                    memset(&tramaEnviar, '\0', 256);
                    trames_verificarTramaConnection(tramaEnviar, tramaRebuda, type);
                    printF("\nRebent fitxer\n");
                    if(Connexions_send(tramaEnviar, fd) < 0){
                        perror("Error al enviar la trama al servidor Gotham 3\n");
                        exit(1);
                    }
                }
                break;

            case DISTORT_DADES_FLECK:
                int sizeToWrite = 247;
                int fd_file = open(contingutFleck.path, O_CREAT | O_WRONLY, 0666);
                lseek(fd_file, cuaMsg.n_bytes, SEEK_SET);
                //printar la poscio del cursor
                if (fd_file == -1) {
                    perror("Error obrint el fitxer");
                    return NULL;
                }
                
                if(cuaMsg.n_bytes + sizeToWrite > contingutFleck.fileSize){
                    sizeToWrite = contingutFleck.fileSize - cuaMsg.n_bytes;
                } 

                char* data = trames_obteData(tramaRebuda);
                if (data == NULL) {
                    perror("Error: trames_obteData ha retornat NULL");
                    close(fd_file);
                    pthread_exit(NULL);
                }

                int contBytes = write(fd_file, data, sizeToWrite);
                free(data);
 
                close(fd_file);
                cuaMsg.n_bytes = cuaMsg.n_bytes + contBytes;
            
                // Busca el nodo con el `fleck_id` correspondiente
                GestioSempahore_semaphoreWait();
                LINKEDLIST_update(info_thread->linked_cua, cuaMsg);
                GestioSempahore_semaphoreSignal();

                cuaMsg = HarleyCommsFuncts_actualitzaPunterLinked(info_thread->linked_cua, fleck_id);
                if(cuaMsg.n_bytes >= contingutFleck.fileSize){
                    if(utils_comparaMD5SUM(fd_file, contingutFleck.md5sum, contingutFleck.path) == 0){
                        printF("Fitxer a distorsionar rebut correctament\n\n");
                        memset(&tramaEnviar, '\0', 256);
                        trames_createFinalFitxer(tramaEnviar, 0);

                        if(Connexions_send(tramaEnviar, fd) < 0){
                            perror("Error al enviar la trama\n");
                            exit(1);
                        }                  

                        //sacar la extension del nombre fel ficher .png .txt
                        strtok(contingutFleck.fileName, ".");
                        char *extensio = strtok(NULL, ".");
                        int newSizeDist = HarleyCommsFuncts_distorsionarFitxer(extensio, contingutFleck.factor, contingutFleck.path);
                        memset(&contingutFleck.md5sum, '\0', 32);
                        strcpy(contingutFleck.md5sum, utils_getMD5SUMfromFIle(contingutFleck.path));
                        
                        //Si no s'ha pogut distorsionar be -> Trama KO
                        if(newSizeDist == -1){
                            memset(&tramaEnviar, '\0', 256);
                            trames_createFinalFitxer(tramaEnviar, -1);
                            break;
                        }

                        char *newFileSize = utils_intToChar(newSizeDist);   
                        memset(&tramaEnviar, '\0', 256);
                        trames_createDistortFile(tramaEnviar, newFileSize, contingutFleck.md5sum);

                        if(Connexions_send(tramaEnviar, fd) < 0){
                            perror("Error al enviar la trama al servidor Gotham 4\n");
                            exit(1);
                        }

                        //Actualitzar la linkedlist
                        contingutFleck.fileSize = newSizeDist;
                        cuaMsg.estatDistort = 1; //Actualitzar pq ara toca rebre el fitxer
                        cuaMsg.n_bytes = 0;
                        cuaMsg.mtype = 1;
    
                        // Busca el nodo con el `fleck_id` correspondiente
                        GestioSempahore_semaphoreWait();
                        LINKEDLIST_update(info_thread->linked_cua, cuaMsg);
                        GestioSempahore_semaphoreSignal();
                        cuaMsg = HarleyCommsFuncts_actualitzaPunterLinked(info_thread->linked_cua, fleck_id);
                        sleep(1);
                        //Enviar tot el fitxer distorsionat
                        if(HarleyCommsFuncts_enviarFitxer(fd, info_thread, cuaMsg, fleck_id, contingutFleck)==0){
                            printF("Fitxer enviat correctament, distorsió finalitzada\n");

                        } else {
                            printF("Error enviant el fitxer\n");
                        }

                        break;

                    } else {
                        printF("MD5SUM incorrecte\n");
                        
                        memset(&tramaEnviar, '\0', 256);
                        trames_createFinalFitxer(tramaEnviar, 1);
                        if(Connexions_send(tramaEnviar, fd) < 0){
                            perror("Error al enviar la trama al servidor Gotham 4\n");
                            exit(1);
                        } else {
                            printF("Trama enviada correctament\n");
                        }
                        pthread_exit(NULL);

                    }
                    SO_deleteImage(contingutFleck.path);
                }

                close(fd_file);
        
                break;
            case DISTORT_MD5SUM_CHECK:
                if(strcmp(trames_obteData(tramaRebuda), "CHECK_OK") == 0){
                } else {
                    printF("Error al distorsionar el fitxer\n");              
                }
                
                break;
                
            case DISCONNECT:
                cuaMsg = HarleyCommsFuncts_actualitzaPunterLinked(info_thread->linked_cua, fleck_id);
                LINKEDLIST_remove(info_thread->linked_cua);

                // Manejo de desconexión
                printF("\nEl client s'ha desconnectat\n");

                SO_deleteImage(contingutFleck.path);
                pthread_exit(NULL);
                break;

            case WRONG_DATA:
                printF("Fleck a rebut una trama errònia, parant la distorsió\n");
                SO_deleteImage(contingutFleck.path);
                if(fd > 0){
                }
                

                pthread_exit(NULL);
                return NULL;

            case WRONG_CHECKSUM:
                printF("WRONG_CHECKSUM, error en el checksum\n");

                //Enviar la trama de error de checksum i parar la distorsió
                memset(&tramaEnviar, '\0', 256);
                trames_TramaErronia(tramaEnviar);
                SO_deleteImage(contingutFleck.path);
                
                if(Connexions_send(tramaEnviar, fd) < 0){
                    perror("Error al enviar la trama al servidor Gotham 5\n");
                    exit(1);
                }
                if(fd > 0){
                    // if(close(fd) < 0){
                    //     perror("Error al tancar el socket\n");
                    //     exit(1);
                    // }
                }

                pthread_exit(NULL);
                return NULL;
            default:
                printF("Trama no reconeguda\n");
                break;
        }
    }

    free(info_thread->info_fleck);  // liberar copia de FleckThread
    free(info_thread);              // liberar copia de InfoThread

    pthread_exit(NULL);
    return NULL;
}


void* HarleyComms_heartbeatThreadGotham(void *arg) {
    GothamThread *gothamThread = (GothamThread *)arg;
    int fd_gotham = gothamThread->fd;
    char trama[256];

    while (1) {
        memset(trama, '\0', 256);
        trames_createHeartBeat(trama);

        if (Connexions_send(trama, fd_gotham) < 0) {
            printF("\nGotham ha caigut\n");
            break;
        }

        sleep(5); 
    }

    kill(getpid(), SIGUSR1); 
    close(fd_gotham);   
    pthread_exit(NULL);
}
 