/***********************************************
*
* @Proposit: Fitxer encarregada de les comunicacions de Gotham.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 21/10/2024
* @Data ultima modificacio:
*
************************************************/
//Include del .h associat a aquest .c
#include "gotham_comms_functs.h"


int GothamCommsFuncts_connectFleck(int fd, char* worker) {
        
    uint8_t type = CONNEXIO_FLECK_GOTHAM;
    char trama[256];
    memset(&trama, '\0', 256);
    trames_verificarTramaConnection(trama, worker, type);
    
    if(Connexions_send(trama, fd) < 0){
        perror("Error al enviar la trama al servidor Gotham\n");
        exit(1);
    }

   return 0;        
}


int GothamCommsFuncts_disconnectWorker(LinkedList *list, int fd_worker, int contWorker) {
    //int FleckNumber = 0;
    char tramaEnviar[256];

    LINKEDLIST_goToHead(list);
    //Ubicar-se a la posició del worker que ha caigut
    while(LINKEDLIST_get(list).number_of_server != contWorker) {
        LINKEDLIST_next(list);
    }

    
    LINKEDLIST_remove(list);
    if(LINKEDLIST_isEmpty(*list)) {
        printF("No hi ha cap worker disponible\n");
        return -2;
    } else {
        LINKEDLIST_goToHead(list);
        trames_createWorkerPrincipal(tramaEnviar);
        if(Connexions_send(tramaEnviar, LINKEDLIST_get(list).fd) < 0) {
            perror("Error al enviar la trama al servidor Gotham\n");
            return -1;
        } 

    } 
    
    return 0;
}


int GothamCommsFuncts_disconnectFleck(int fd) {
    if(fd > 0) {
        if(close(fd) < 0) {
            perror("Error al tancar el socket del client Fleck");
            return -1;
        }
    }
    return 0;
}

int GothamCommsFuncts_connectWorker(char* tramaRebuda, int fd_worker, InfoWorker *info_worker, LinkedList *list, char *typeWorker) {
        int numerServer = LINKEDLIST_getSize(list) + 1; // + 1 pq ara s'afegira aquest 
        
        //Guardar info del worker
        info_worker->fd = fd_worker;

        char *ip_temp = trames_obteIp(tramaRebuda);
        char *port_temp = trames_obtePort(tramaRebuda);

        if(ip_temp != NULL) {
            info_worker->ip = strdup(ip_temp);
        } else {
            info_worker->ip = NULL;
        }
        if(port_temp != NULL) {
            info_worker->port = strdup(port_temp);
        } else {
            info_worker->port = NULL;
        }
        free(ip_temp);
        free(port_temp);
        ip_temp = NULL;
        port_temp = NULL;
        
        typeWorker[strlen(typeWorker)] = '\0';
        info_worker->type = typeWorker;
        info_worker->number_of_server = numerServer;
        
        //Bucle per assegurar que el nou worker s'afageix al final de la linkedlist
        LINKEDLIST_goToHead(list);
        while(!LINKEDLIST_isAtEnd(*list)) {
            LINKEDLIST_next(list);
        }
        
        LINKEDLIST_add(list, *info_worker); //Afegir el worker a la linkedlist
        ///AIXO DE PROVA
        LINKEDLIST_goToHead(list);  
        while(!LINKEDLIST_isAtEnd(*list)) {
            LINKEDLIST_next(list);
        }
        char trama[256];
        memset(&trama, '\0', 256);
        trames_verificarTramaConnection(trama, tramaRebuda, CONNEXIO_WORKER_GOTHAM);

        if(Connexions_send(trama, info_worker->fd) < 0){
            perror("Error al enviar la trama al servidor Gotham\n");
            return -1;
        }


        return 0; // Si typeWorker = 1 -> Harley (Media) || Si typeWorker = 2 -> Enigma (Text)
}


int GothamCommsFuncts_distort(int fd, LinkedList *list) {
    char trama[256];
    memset(&trama, '\0', 256);

    // Print info de la linked list
    LINKEDLIST_goToHead(list);  
    while(!LINKEDLIST_isAtEnd(*list)) {
        LINKEDLIST_next(list);
    }

    if (LINKEDLIST_isEmpty(*list)) {
        trames_createRespostaPeticioDistort(0x10, trama, NULL, NULL, 0);
        Connexions_send(trama, fd);
        return -1;
    } else {
        LINKEDLIST_goToHead(list);

        InfoWorker *info_worker = malloc(sizeof(InfoWorker));
        if (info_worker == NULL) {
            perror("Error al reservar memòria per info_worker");
            return -1;
        }

        *info_worker = LINKEDLIST_get(list);

        trames_createRespostaPeticioDistort(0x10, trama, info_worker->ip, info_worker->port, 1);
        free(info_worker);  // 🧹 Alliberem la memòria
    }
    
    char *data = trames_obteData(trama);
    if (data != NULL) {
        free(data);  // 🧹 Alliberem la memòria obtinguda
    }

    if (Connexions_send(trama, fd) < 0) {
        perror("Error enviando datos al servidor Fleck");
        return -1;
    }

    return 0;
}


int GothamCommsFuncts_distortResume(int fd_fleck, LinkedList *list) {
    printF("Connectant usuari a un nou Worker...\n");
    char trama[256];
    memset(&trama, '\0', 256);
    LINKEDLIST_goToHead(list);

    if(!LINKEDLIST_isEmpty(*list)){
        InfoWorker info_worker = LINKEDLIST_get(list);
        trames_createRespostaPeticioDistort(0x11, trama, info_worker.ip, info_worker.port, 1);
    } else {
        //Enviar trama dient q la linked esta buida -> No hi ha workers disponibles
        trames_createRespostaPeticioDistort(0x11, trama, NULL, NULL, 0);
    }

    if (Connexions_send(trama, fd_fleck) < 0) {
        perror("Error enviando datos al servidor Fleck");
        return -1;
    }


    return 0;
}




int GothamCommsFuncts_assignaWorkerPrincipal(LinkedList *list, int fd_worker, char *typeWorker) {
    int count = 0;
    char trama[256];
    LINKEDLIST_goToHead(list);
    count = LINKEDLIST_getSize(list);
    char *pantalla = NULL;
    asprintf(&pantalla, "\nWorker %s connectat correctament\n", typeWorker); 
    write(1, pantalla, strlen(pantalla));
    free(pantalla);
    //Si es el primer elemento de la linkedlist -> enviar trama worker principal
    if(count == 1) {
        memset(&trama, '\0', 256);
        trames_createWorkerPrincipal(trama);
        if(Connexions_send(trama, fd_worker) < 0) {
            perror("Error al enviar la trama al servidor Gotham\n");
            return -1;
        }

        //Worker assignat com a principal -> Enviar informació a ARKHAM
        char *dataArkam = NULL;
        char timeStamp[20];
        trames_obteTimeStamp(trama, timeStamp);
        asprintf(&dataArkam, "IP:%s:%s", LINKEDLIST_get(list).ip, LINKEDLIST_get(list).port);
        GothamCommsFuncts_sendArkam(trames_obteTramaType(trama), dataArkam, timeStamp, typeWorker);
        free(dataArkam);    
    }

    return 0;
}

int logger_fd = -1;

void GothamCommsFuncts_setFdArkam(int fd) {
    logger_fd = fd;
}


void GothamCommsFuncts_sendArkam(uint8_t type, const char* data, char *timestamp, char *workerType) {
    if (logger_fd == -1 || data == NULL) return;
    char message[512];
    memset(message, 0, sizeof(message));

    switch (type) {
        case 0x01:
            snprintf(message, sizeof(message), "[%s] Fleck connected: username=%s", timestamp, data);
            break;
        case 0x02:
            snprintf(message, sizeof(message), "[%s] %s connected: %s", timestamp, workerType, data);
            break;
        case 0x10:
            snprintf(message, sizeof(message), "[%s] Fleck requested distortion: %s", timestamp, data);
            break;
        case 0x11:
            snprintf(message, sizeof(message), "[%s] Fleck requested a resumed distortion: %s", timestamp, data);
            break;
        case 0x07:
            if(workerType == NULL) {
                snprintf(message, sizeof(message), "[%s] Fleck disconnected: username=%s", timestamp, data); 
            } else {
                snprintf(message, sizeof(message), "[%s] %s disconnected: %s", timestamp, workerType, data);
            }
            break;
        case 0x08:
            snprintf(message, sizeof(message), "[%s] %s assigned as primary worker:%s", timestamp, workerType, data);
            break;
        default:
            snprintf(message, sizeof(message), "[%s] Unknown event type: 0x%02X, data: %s", timestamp, type, data);
            break;
    }

    write(logger_fd, message, strlen(message) + 1); // Incluye '\0'
    // int fd = open("../Arkham/log.txt", O_RDWR | O_CREAT, 0644);
    // lseek(fd, 0, SEEK_SET);
    // write(fd, message, strlen(message)+1);
}