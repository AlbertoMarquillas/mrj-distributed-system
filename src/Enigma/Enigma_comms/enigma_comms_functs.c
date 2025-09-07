#include "enigma_comms_functs.h"

#define MAX_WORD 256  // Tamaño máximo de una palabra
#define MAX_LINE 1024  // Tamaño máximo de una línea


void EnigmaCommsFuncts_ajustaNomFitcher(char *nombre) {
    char *p = nombre;
    while (*p) {
        if (*p == '\r') {
            *p = '\0';  // Reemplaza \r con fin de cadena
            break;
        }
        p++;
    }
}

void EnigmaCommsFuncts_eliminaParaules(char *path, int factor) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Error obrint l'archiu");
        return;
    }

    int temp_fd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd == -1) {
        perror("Error creando archivo temporal");
        close(fd);
        return;
    }

    char *paraula = malloc(1);
    int countLletres = 0;
    ssize_t bytes_leidos;
    char caracter;

    while ((bytes_leidos = read(fd, &caracter, 1)) > 0) {
        if (caracter != ' ' && caracter != '\n' && caracter != '\t' && caracter != '\r') {
            paraula = realloc(paraula, countLletres + 2);
            if (!paraula) {
                perror("Error en realloc");
                close(fd);
                close(temp_fd);
                return;
            }
            paraula[countLletres++] = caracter;
            paraula[countLletres] = '\0';
        } else {
            // Eliminar puntuación final para calcular longitud real
            int longitud_real = countLletres;
            while (longitud_real > 0 && !isalnum((unsigned char)paraula[longitud_real - 1])) {
                longitud_real--;
            }

            if (longitud_real >= factor) {
                write(temp_fd, paraula, countLletres); // Escribir la palabra original completa
            }

            write(temp_fd, &caracter, 1); // Escribir el separador original
            free(paraula);
            paraula = malloc(1);
            countLletres = 0;
        }
    }

    // Procesar la última palabra si no terminaba en separador
    if (countLletres > 0) {
        int longitud_real = countLletres;
        while (longitud_real > 0 && !isalnum((unsigned char)paraula[longitud_real - 1])) {
            longitud_real--;
        }
        if (longitud_real >= factor) {
            write(temp_fd, paraula, countLletres);
        }
    }

    free(paraula);
    close(fd);
    close(temp_fd);

    rename("temp.txt", path);
}


int EnigmaCommsFuncts_distorsionarFitxer(char *extensio, int factor, char *FilePath){
    printF("Distorsionant fitxer...\n");
    EnigmaCommsFuncts_eliminaParaules(FilePath, factor);
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
    printF("Fitxer acabat de distorsionar\n");
    return fileSizeDist;
}



int EnigmaCommsFuncts_enviarFitxer(int fd_worker, InfoThread *info_thread, CuaInfoFleck cuaMsg, int fleck_id, ContingutFleck contingutFleck){

    printF("\nEnviant fitxer...\n");
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


CuaInfoFleck EnigmaCommsFuncts_actualitzaPunterLinked(LinkedList *linked_cua, int fleck_id) {
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




