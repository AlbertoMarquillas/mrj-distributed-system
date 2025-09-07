#include "harley_comms_functs.h"

int HarleyCommsFuncts_distorsionarFitxer(char *extensio, int factor, char *FilePath){

    if(strcmp(extensio, "wav") == 0){
        printF("Distorsionant fitxer...\n");
        if(SO_compressAudio(FilePath, factor) != 0){
            printF("Error al distorsionar el fitxer\n");
            return -1;  
        } 

        printF("Fitxer acabat de distorsionar\n");
    } else {
        //distorsionar imatge
        printF("Distorsionant fitxer...\n");
        if(SO_compressImage(FilePath, factor) != 0){
            printF("Error al distorsionar el fitxer\n");
            return -1;
        }
        printF("Fitxer acabat de distorsionar\n");
    }

    // Obtenir el filesize
    int fd_file = open(FilePath, O_RDONLY);
    if (fd_file == -1) {
        perror("Error obrint el fitxer");
    }

    // Usa lseek para moverte al final del archivo y obtener el tamaño
    int fileSizeDist = lseek(fd_file, 0, SEEK_END);
    if (fileSizeDist == -1) {
        perror("Error al calcular el tamaño del fichero");
        close(fd_file);
    }
    close(fd_file);

    return fileSizeDist;
}




int HarleyCommsFuncts_enviarFitxer(int fd_worker, InfoThread *info_thread, CuaInfoFleck cuaMsg, int fleck_id, ContingutFleck contingutFleck){

    printF("\nEnviant fitxer\n");
    char tramaFitxer[256];

    int sizeToRead = 247; // 256 - 9 = 247 -> el -9 es del que ocupen timestamp i type chechsum data_length
    char *data = (char*) malloc (sizeof (char) * sizeToRead);
    int cont = 0;

    int fileSize = utils_getFileSize(contingutFleck.path);

    while (cuaMsg.n_bytes < fileSize) {    

        int fd_file = open(contingutFleck.path, O_RDONLY);
        if (fd_file == -1) {
            perror("Error obrint el fitxer");
            return -1;
        }

        lseek(fd_file, cuaMsg.n_bytes, SEEK_SET);

        memset(data, '\0', sizeToRead);
        cont = read(fd_file, data, sizeToRead);
        cuaMsg.n_bytes = cuaMsg.n_bytes + cont;   
        cuaMsg.mtype = 1;  
        memset(&tramaFitxer, '\0', 256);
        trames_createEnviarFitxer(tramaFitxer, data);
        
        
        GestioSempahore_semaphoreWait();
        LINKEDLIST_update(info_thread->linked_cua, cuaMsg);
        LINKEDLIST_goToHead(info_thread->linked_cua);
        cuaMsg = LINKEDLIST_get(info_thread->linked_cua);  
        while(cuaMsg.fleck_id != fleck_id){
            if(cuaMsg.fleck_id != fleck_id){
                LINKEDLIST_next(info_thread->linked_cua);
            } else {    
                break;  
            }
            cuaMsg = LINKEDLIST_get(info_thread->linked_cua);
        }
    
        sleep(1);
        if(Connexions_send(tramaFitxer, fd_worker)==-1){
            return -1;
        }

        GestioSempahore_semaphoreSignal();
        close(fd_file);

    }

    return 0;
}


CuaInfoFleck HarleyCommsFuncts_actualitzaPunterLinked(LinkedList *linked_cua, int fleck_id) {
    CuaInfoFleck cuaMsg;
    
    GestioSempahore_semaphoreWait();
    LINKEDLIST_goToHead(linked_cua); 
    cuaMsg = LINKEDLIST_get(linked_cua);  
    
    while (cuaMsg.fleck_id != fleck_id) {  
        LINKEDLIST_next(linked_cua);
        cuaMsg = LINKEDLIST_get(linked_cua);
    }
    
    GestioSempahore_semaphoreSignal();
    return cuaMsg;
}




