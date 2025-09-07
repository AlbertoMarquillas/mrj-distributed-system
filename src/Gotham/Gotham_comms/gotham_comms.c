/***********************************************
*
* @Proposit: Fitxer encarregada de les comunicacions de Gotham.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 21/10/2024
* @Data ultima modificacio:
*
************************************************/

//Include del .h associat a aquest .c
#include "gotham_comms.h"

LinkedList *harley_list;
LinkedList *enigma_list;

// Creamos un nuevo thread para manejar esta conexión
pthread_t fleck_client_thread[30];
pthread_t worker_thread;



void GothamComms_freeLists() {
    if (harley_list != NULL) {
        LINKEDLIST_goToHead(harley_list);
        while (!LINKEDLIST_isAtEnd(*harley_list)) {
            LINKEDLIST_remove(harley_list);
            LINKEDLIST_next(harley_list);
        }
        LINKEDLIST_destroy(harley_list);
        harley_list = NULL;
    }
    if (enigma_list != NULL) {
        LINKEDLIST_goToHead(enigma_list);
        while (!LINKEDLIST_isAtEnd(*enigma_list)) {
            LINKEDLIST_remove(enigma_list);
            LINKEDLIST_next(enigma_list);
        }
        LINKEDLIST_destroy(enigma_list);
        enigma_list = NULL;

    }

}

void GothamComms_closeFileDescriptors() {
    // Cerrar cualquier socket (no se si se puede hacer así)
    // for (int fd = 3; fd < 10; fd++) {
    //     close(fd);
    // }
}

void GothamComms_freeThreads() {
    int i = 0;
    while(i < 30) {
        if(fleck_client_thread[i] != 0) {
            if (pthread_join(fleck_client_thread[i], NULL) != 0) { 
            }
        }
        i++;
    }

    if (worker_thread != 0) { 
        if (pthread_join(worker_thread, NULL) != 0) { 
        }
    }
}

void GothamComms_freeMem(){
    GothamComms_freeThreads();
    GothamComms_closeFileDescriptors();
    GothamComms_freeLists();
    
}

void* GothamComms_handleFleckClient(void* arg) {
    InfoThread *info_thread = (InfoThread *)arg;
    InfoClient *info_client = info_thread->info_client;
    char *typeWorker = NULL;
    int fd_fleck = info_client->fd;
    int* gotham_running = info_thread->gotham_running;
    char *userName;
    while (*gotham_running == 1) { //Aqui s'ha de controlar el logout del client Fleck
        char tramaRebuda[256];
        char timeStamp[20];
        memset(&timeStamp, '\0', 20);
        memset(&tramaRebuda, '\0', 256);
        //uint8_t typeHex = trames_obteTramaType(tramaRebuda);
        memcpy(&tramaRebuda, Connexions_read(fd_fleck, tramaRebuda), 256);

        if(strcmp(tramaRebuda, "ERROR") == 0) {
            printF("Ha saltat trama de error a fleck\n");
            pthread_exit(NULL);
        }
        
        
        if(trames_verificarChecksum(tramaRebuda) == 0) {
            int typeHex = trames_obteTramaType(tramaRebuda);
            switch(typeHex) {
                case CONNEXIO_FLECK_GOTHAM:
                    if(GothamCommsFuncts_connectFleck(fd_fleck, tramaRebuda) == 0){
                        printF("\nFleck connectat correctament\n");
                        userName = trames_obteUserName(tramaRebuda);
                        trames_obteTimeStamp(tramaRebuda, timeStamp);
                        GothamCommsFuncts_sendArkam(typeHex, userName, timeStamp, NULL);
                        
                    } else {
                        printF("\nFleck no ha podut connectar-se\n");
                    }
                    break;

                case DISTORT_GOTHAM_FLECK:
                    trames_obteTypeWorker(tramaRebuda, &typeWorker);
                    if((strcasecmp(typeWorker, MEDIA) != 0) && strcasecmp(typeWorker, TEXT) != 0) {
                        printF("El type és incorrecte\n");
                        break;
                    }
                    printF("\nUsuari ha fet una petisió de distorsió\n");
                    if(strcasecmp(typeWorker, MEDIA) == 0) {
                        if(GothamCommsFuncts_distort(fd_fleck, harley_list) == 0){
                            printF("Worker enviat a l'usuari\n");
                        } else {
                            printF("No s'ha pogut trobar cap Worker Disponible\n");
                        }
                    } else{
                        if(GothamCommsFuncts_distort(fd_fleck, enigma_list) == 0){
                            printF("Worker enviat a l'usuari\n");
                        } else {
                            printF("No s'ha pogut trobar cap Worker Disponible\n");
                        }
                    }

                    char *dataArkham = NULL;
                    asprintf(&dataArkham, "username=%s, mediaType=%s, fileName=%s", userName, typeWorker, trames_obteFileName(tramaRebuda));
                    trames_obteTimeStamp(tramaRebuda, timeStamp);
                    GothamCommsFuncts_sendArkam(typeHex, dataArkham, timeStamp, NULL);
                    free(dataArkham);
                    break;
                case DISTORT_RESUME_GOTHAM_FLECK:
                    sleep(5);
                    if(strcasecmp(typeWorker, MEDIA) == 0) {
                        if(GothamCommsFuncts_distortResume(fd_fleck, harley_list) == 0){
                            printF("\nUsuari connectat a un nou Worker de Media\n");
                        } else {
                            printF("La connexió no s'ha podut reanudar");
                        }
                    } else {
                        if(GothamCommsFuncts_distortResume(fd_fleck, enigma_list) == 0){
                            printF("\nUsuari connectat a un nou Worker de Text\n");
                        } else {
                            printF("La connexió no s'ha podut reanudar");
                        }
                    }

                    char *dataArkam = NULL;
                    asprintf(&dataArkham, "username=%s, mediaType=%s, fileName=%s", userName, typeWorker, trames_obteFileName(tramaRebuda));
                    trames_obteTimeStamp(tramaRebuda, timeStamp);
                    GothamCommsFuncts_sendArkam(typeHex, dataArkam, timeStamp, NULL);
                    free(dataArkam);
                 
                    break;

                case DISCONNECT:

                    printF("Desconnectat Fleck\n");
                    if(GothamCommsFuncts_disconnectFleck(fd_fleck) == 0) {
                        printF("Fleck desconnectat\n");

                        //Fleck s'ha desconnectat -> Enviar informació a ARKHAM
                        trames_obteTimeStamp(tramaRebuda, timeStamp);
                        GothamCommsFuncts_sendArkam(typeHex, userName, timeStamp, NULL);

                        if(fd_fleck > 0) {
                            close(fd_fleck);
                        }
                        if(info_client) {
                            free(info_client);
                            info_client = NULL;
                        }
                        if(info_thread) {
                            free(info_thread);
                            info_thread = NULL;
                        }

                        if(userName) {
                            free(userName);
                            userName = NULL;
                        }
                        if(typeWorker) {
                            free(typeWorker);
                            typeWorker = NULL;
                        }


                        pthread_exit(NULL);
                    } else {
                        printF("Error al desconnectar fleck\n");
                    }

                    return NULL;

                    break;
                    
                case WRONG_DATA:
                    printF("Dades incorrectes\n");
                    break;
                case HEARTBEAT:
                    break;
                default:
                    printF("Trama incorrecta\n");
                    break;
            }
        } else {
            printF("Error en el checksum Fleck\n");

            if(fd_fleck > 0) {
                close(fd_fleck);
            }
            if(info_client) {
                free(info_client);
                info_client = NULL;
            }
            if(info_thread) {
                free(info_thread);
                info_thread = NULL;
            }

            pthread_exit(NULL);
        }

    }

    if(typeWorker) {
        free(typeWorker);
        typeWorker = NULL;
    }

    if(info_client) {
        printF("Entra a liberar info client en handle fleck client\n");
        free(info_client);
        info_client = NULL;
    }
    if(info_thread) {
        printF("Entra a liberar info thread en handle fleck client\n");
        free(info_thread);
        info_thread = NULL;
    }


    if(fd_fleck > 0) {
        close(fd_fleck);
    }
    pthread_exit(NULL);
    return NULL;
}

void *GothamComms_workerClient(void * arg) {

    InfoThread *info_thread = (InfoThread *)arg;
    InfoWorker *info_worker = info_thread->info_worker;
    int fd_worker = info_worker->fd;
    int* gotham_running = info_thread->gotham_running;

    char tramaRebuda[256];
    char timeStamp[20];
    int countHarley = 0;
    int countEnigma = 0;
    char* typeWorkerAux = NULL;

    while(*gotham_running == 1) {
        memset(&tramaRebuda, '\0', 256);
        memset(&timeStamp, '\0', 20);
        if (Connexions_read(fd_worker, tramaRebuda) == NULL) {
            printF("Error al leer datos del socket\n");
            break; 
        }
        
        if(strcmp(tramaRebuda, "ERROR") == 0) {
            printF("Ha saltat trama de error a worker\n");
            pthread_exit(NULL);
        } 
        if (tramaRebuda[0] == '\0') {
            printF("Trama rebuda buida\n");
            break;
        }

        int type = trames_obteTramaType(tramaRebuda);
        trames_obteTypeWorker(tramaRebuda, &typeWorkerAux);
        char* typeWorker = NULL;
        asprintf(&typeWorker, "%s", typeWorkerAux);
        if(typeWorkerAux){
            free(typeWorkerAux);
            typeWorkerAux = NULL;
        }
        if(typeWorker == NULL) {
            printF("Error al obtenir el typeWorker\n");
            break;
        }

        if(trames_verificarChecksum(tramaRebuda) == 0) {

            switch(type) {
                
                case CONNEXIO_WORKER_GOTHAM:
                    if(strcasecmp(typeWorker, MEDIA) == 0){
                        if(GothamCommsFuncts_connectWorker(tramaRebuda, fd_worker, info_worker, harley_list, typeWorker) == 0) {
                             //S'ha connectat un nou worker -> Enviar informació a ARKAM
                            char *dataArkam = NULL;
                            trames_obteTimeStamp(tramaRebuda, timeStamp);
                            asprintf(&dataArkam, "IP:%s:%s", info_worker->ip, info_worker->port);
                            GothamCommsFuncts_sendArkam(type, dataArkam, timeStamp, typeWorker);
                            free(dataArkam);

                            LINKEDLIST_goToHead(harley_list);
                            while(!LINKEDLIST_isAtEnd(*harley_list)) {
                                LINKEDLIST_next(harley_list);
                    
                            }
                            if(GothamCommsFuncts_assignaWorkerPrincipal(harley_list, fd_worker, typeWorker) != 0) {
                                printF("Error al assignar el worker principal\n");
                            }
                            countHarley = LINKEDLIST_getSize(harley_list); 
                            char *pantalla = NULL;
                            asprintf(&pantalla, "Workers disponibles: %d\n", countHarley);
                            write(1, pantalla, strlen(pantalla));
                            free(pantalla);
                        } else {
                            printF("Error al connectar el worker\n");
                        }
                        
      
                    } else if(strcasecmp(typeWorker, TEXT) == 0) {
                        if(GothamCommsFuncts_connectWorker(tramaRebuda, fd_worker, info_worker, enigma_list, typeWorker) == 0) {
                             //S'ha connectat un nou worker -> Enviar informació a ARKAM
                            char *dataArkam = NULL;
                            trames_obteTimeStamp(tramaRebuda, timeStamp);
                            asprintf(&dataArkam, "IP:%s:%s", info_worker->ip, info_worker->port);
                            GothamCommsFuncts_sendArkam(type, dataArkam, timeStamp, typeWorker);
                            free(dataArkam);

                            if(GothamCommsFuncts_assignaWorkerPrincipal(enigma_list, fd_worker, typeWorker) != 0) {
                                printF("Error al assignar el worker principal\n");
                            }
                            countEnigma = LINKEDLIST_getSize(enigma_list);
                        } else {
                            printF("Error al connectar el worker\n");
                        }
                    }


                   
                    break;


                case DISCONNECT:
               
                    printF("\nHa caigut un worker\n");
                    int disconnect = 0;
                    if(strcasecmp(typeWorker, MEDIA) == 0) {
                        disconnect = GothamCommsFuncts_disconnectWorker(harley_list, fd_worker, countHarley);  
                    } else {
                        disconnect = GothamCommsFuncts_disconnectWorker(enigma_list, fd_worker, countEnigma);  
                    }
                    if(disconnect <= 0) {
                        
                        char *dataArkam = NULL;
                        trames_obteTimeStamp(tramaRebuda, timeStamp);
                        asprintf(&dataArkam, "IP:%s:%s", info_worker->ip, info_worker->port);
                        GothamCommsFuncts_sendArkam(type, dataArkam, timeStamp, typeWorker);
                        free(dataArkam);
                        //pthread_exit(NULL);
                        if(fd_worker > 0) {
                            close(fd_worker);
                        }
                        if (typeWorkerAux) {
                            free(typeWorkerAux);
                        }
                        pthread_exit(NULL);
                        return NULL;                   
                    } else {
                        printF("Error al desconnectar el worker\n");
                    }
                    break;
                case WRONG_DATA:
                    printF("Dades incorrectes\n");
                    break;
                case HEARTBEAT:
                    break;
                default:
                    //Printar type de trama
                    printF("Trama incorrecta\n");                  
                    break;
            }

        } else {
            //printF("Error en el checksum Worker\n");
        }
        if (typeWorker) {
            free(typeWorker);
            typeWorker = NULL;
        }

    }
    if (typeWorkerAux) {
        free(typeWorkerAux);
    }

    if(fd_worker > 0) {
        shutdown(fd_worker, SHUT_RDWR);
        close(fd_worker);
        fd_worker = -1;
    }
    
    if (typeWorkerAux) {
        free(typeWorkerAux);
        typeWorkerAux = NULL;
    }
    if(info_worker->ip) {
        free(info_worker->ip);
        info_worker->ip = NULL;
    }
    if(info_worker->port) {
        free(info_worker->port);
        info_worker->port = NULL;
    }    
    
    pthread_exit(NULL);
    return NULL;
}

/***********************************************
 * @Nom: GothamComms_connectWorker
 * @Definicio: Thread que connecta amb el servidor Harley/Enigma
 * @Arg: void *arg: Parametres del thread
 * @Ret: ----
 * ***********************************************/
void* GothamComms_connectWorker(void *arg) {

    // Creem una instancia de la estructura HarleyEnigmaThread
    InfoThread *info_thread = (InfoThread *)arg;
    InfoWorker *info_worker = info_thread->info_worker;
    int fd_server_worker = info_worker->fd;
    int fd_worker = -1;
    int* gotham_running = info_thread->gotham_running;
    //SI la lista ya esta creada, no fa falta crearla de nou
    if(harley_list == NULL) {
        harley_list = LINKEDLIST_create();
    }
    if(enigma_list == NULL) {
        enigma_list = LINKEDLIST_create();
    }

    printF("\nEsperant connexió de Harley/Enigma\n");

    while(*gotham_running == 1) {
        // Creamos un nuevo thread para manejar esta conexión

        // Aceptamos conexiones de Harley/Enigma y Fleck
        fd_worker = Connexions_acceptar(fd_server_worker);
        //!!!! Si s'accepta bé -> MONOLIT
        if (fd_worker < 0) {
            if (*gotham_running == 0) break;  // Salir si estamos cerrando
            continue;
        }
        if (*gotham_running == 0) {
            close(fd_worker);
            break;
        }
        //Guardar en la linkedlist
        info_worker->fd = fd_worker;
        info_worker->ip = info_worker->ip;
        info_worker->port = info_worker->port;
        info_worker->type = NULL;
        info_worker->number_of_server = 0;

        info_thread->info_worker = info_worker;
        info_thread->gotham_running = gotham_running;
        // //S'ha connectat un nou worker -> Enviar informació a ARKAM
        // char *dataArkam = NULL;
        // snprintf(dataArkam, 256, "IP:%d:%d", info_worker->ip, info_worker->port);
        // GothamLogger_send(dataArkam, fd_worker, 0x01, 0x00, 0x00, 0x00);


        // Creamos un thread para manejar la conexión y enviamos la estructura con los datos
        if(pthread_create(&worker_thread, NULL, GothamComms_workerClient, info_thread) != 0) {
            perror("Error al crear el thread de Harley/Enigma");
            if(fd_worker > 0) {
                close(fd_worker);
            }
            if(info_worker) {
                free(info_worker);
                info_worker = NULL;
            }
            if(info_thread) {
                free(info_thread);
                info_thread = NULL;
            }
            continue;
        } else {
            pthread_detach(worker_thread);
            
        }
    
    }
    if(fd_worker > 0) {
        close(fd_worker);
        fd_worker = -1;
    }
    pthread_exit(NULL);
    return NULL;
}

/***********************************************
 * @Nom: GothamComms_connectFleck
 * @Definicio: Thread que connecta amb el servidor Fleck
 * @Arg: void *arg: Parametres del thread
 * @Ret: ----
 * ***********************************************/
void* GothamComms_connectFleck(void* arg) {
    InfoThread *info_thread = (InfoThread *)arg;
    InfoClient *info_client = info_thread->info_client;
    int n_flecks = -1;
    int fd_server_client = info_client->fd;

    int fd_client = -1;
    int* gotham_running = info_thread->gotham_running;
    while (*gotham_running == 1) {
        fd_client = Connexions_acceptar(fd_server_client);
        n_flecks++;
        if (fd_client < 0) {
            if (*gotham_running == 0) break;
            continue;
        }

        if (*gotham_running == 0) {
            shutdown(fd_client, SHUT_RDWR);
            close(fd_client);
            break;
        }


        // Creamos una instancia de la estructura FleckThread
        InfoThread *new_info_thread = malloc(sizeof(InfoThread));
        InfoClient *new_info_client = malloc(sizeof(InfoClient));
        if(new_info_thread == NULL || new_info_client == NULL) {
            perror("Error al asignar memoria para el nuevo cliente Fleck");
            if (new_info_thread) {
                printF("Entra a liberar info thread en connect fleck creo malloc\n");
                free(new_info_thread);
                new_info_thread = NULL;
            }
            if (new_info_client) {
                printF("Entra a liberar info client en connect fleck creo malloc\n");
                free(new_info_client);
                new_info_client = NULL;
            }
            if(fd_client > 0) {
                shutdown(fd_client, SHUT_RDWR);
                close(fd_client);
            }
            
            continue;
        }

        if(new_info_client != NULL) {
            // Llenamos los datos del nuevo hilo de Fleck
            new_info_client->fd = fd_client;
            new_info_client->ip = info_client->ip;
            new_info_client->port = info_client->port;
            new_info_client->type = info_client->type;
            new_info_client->user_name = info_client->user_name;
        } else {
            perror("Error al asignar memoria para el nuevo cliente Fleck");
            if(new_info_thread) {
                printF("Entra a liberar info thread en connect fleck error malloc\n");
                free(new_info_thread);
                new_info_thread = NULL;
            }
            if(new_info_client) {
                printF("Entra a liberar info client en connect fleck error malloc\n");
                free(new_info_client);
                new_info_client = NULL;
            }
            if(fd_client > 0) {
                shutdown(fd_client, SHUT_RDWR);
                close(fd_client);
            }
            continue;
        }
        if(new_info_thread != NULL) {
            new_info_thread->info_client = new_info_client;
            new_info_thread->gotham_running = gotham_running;
        } else {
            perror("Error al asignar memoria para el nuevo cliente Fleck");
            if(new_info_client) {
                printF("Entra a liberar info client en connect fleck error new_info_thread\n");
                free(new_info_client);
                new_info_client = NULL;
            }
            if(new_info_thread) {
                printF("Entra a liberar info thread en connect fleck error new_info_thread\n");
                free(new_info_thread);
                new_info_thread = NULL;
            }
            if(fd_client > 0) {
                shutdown(fd_client, SHUT_RDWR);
                close(fd_client);
            }
            continue;
        }
        // Creamos un thread para manejar la conexión
        if(pthread_create(&fleck_client_thread[n_flecks], NULL, GothamComms_handleFleckClient, new_info_thread) != 0) {
            perror("Error al crear el thread de Fleck");
            if(new_info_client) {
                printF("Entra a liberar info client en connect fleck thread\n");
                free(new_info_client);
                new_info_client = NULL;
            }
            if(new_info_thread) {
                printF("Entra a liberar info thread en connect fleck thread\n");
                free(new_info_thread);
                new_info_thread = NULL;
            }
            if(fd_client > 0) {
                shutdown(fd_client, SHUT_RDWR);
                close(fd_client);
            }
            continue;
        } else {
            pthread_detach(fleck_client_thread[n_flecks]);
        }


    }
    shutdown(fd_server_client, SHUT_RDWR);
    close(fd_server_client);
    pthread_exit(NULL);

    return NULL;

}
