#include "utils.h"
/***********************************************
* @Nom: Utils_readFileLimit
* @Definicio: Funció que llegeix un fitxer fins a un limitador
* @Arg: int fd: File descriptor del fitxer que es vol llegir
* @Arg: char limit: Caràcter que indica el final del fitxer
* @Ret: char *: Cadena de caràcters amb el contingut del fitxer
***********************************************/
char *Utils_readFileLimit(int fd, char limit) {
    char *string; //Cadena de caràcters per guardar el contingut del fitxer
    int lenght = 0, error; //Lenght es la longitud de la cadena que es va llegint
    char c; //C es el caracter que es va llegint
    string = (char *)malloc(sizeof(char)); //String es la cadena que es va llegint
    error = read(fd, &c, sizeof(char)); //Llegim el primer caracter del fitxer

    while (c != limit && error > 0) { //Durant tot el fitxer o fins a trobar el limitador
        if(c == '\r'){
            error = read(fd, &c, sizeof(char));
            continue;
        }
        if(c != '\n' && c!= '\r'){ //Si el caracter no es un salt de linia
            string = (char *)realloc(string, sizeof(char) * (lenght + 2)); //Reservem memoria per al caracter
            string[lenght++] = c; //Guarde el caracter a la cadena
        } 

        error = read(fd, &c, sizeof(char)); //Llegim el següent caracter del fitxer
    }
    string = (char *)realloc(string, sizeof(char) * (lenght + 2)); //Reservem memoria per al caracter de final de cadena

    string[lenght] = '\0'; //Guarde el caracter de final de cadena
    return string; //Retornem la cadena
}

void Utils_printarBenvinguda(char* msg, char* user) {
    char* user_name = (char*)malloc(sizeof(char)*strlen(user)+sizeof(char)*strlen(msg)+2);
    asprintf(&user_name, "%s %s", user, msg);
    printF(user_name);
    if(user_name != NULL){
        free(user_name);
        user_name = NULL;
    }
}






char* utils_getMD5SUMfromFIle(char* path) { 
    char* ERROR_PIPE = "Error al crear el pipe\n";
    int fds[2];
    int pid;
    char *cmd[3];

    cmd[0] = strdup("md5sum");
    cmd[1] = strdup(path);
    cmd[2] = NULL;

    char buffer[33];
    memset(buffer, '\0', 33);
    if (pipe(fds) < 0) {
        write(1, ERROR_PIPE, strlen(ERROR_PIPE));
        return NULL;
    } else {
        pid = fork();
        switch (pid) {
        case -1:
            perror("Error al fer fork");
            return NULL;
        case 0: // Proceso hijo
            close(fds[0]);
            dup2(fds[1], STDOUT_FILENO);
            close(fds[1]);
            execvp(cmd[0], cmd);
            perror("Error executant md5sum");
            exit(1);
            break;
        
        default: // Proceso padre
            close(fds[1]);
            waitpid(pid, NULL, 0); // Esperar al hijo
            int bytes_read = read(fds[0], buffer, 32);
            if (bytes_read != 32) {
                perror("Error al llegir");
                buffer[0] = '\0';
            }
           // buffer[bytes_read] = '\0';
            close(fds[0]);
            break;
        }
    }
    free(cmd[0]);
    free(cmd[1]);

    char *md5sum = strdup(buffer);
    return md5sum;
}


int utils_enviarFitxer(int fd_worker, char *path, int fileSize){
    printF("\nEnviant fitxer\n");
    char tramaFitxer[256];
    int fd_file = open(path, O_RDONLY);
    if (fd_file == -1) {
        perror("Error obrint el fitxer");
        return -1;
    }

    // ir al inicio del archivo
    lseek(fd_file, 0, SEEK_SET);

    int sizeToRead = 247; // 256 - 9 = 247 -> el -9 es del que ocupen timestamp i type chechsum data_length
    int bytesRead = 0;
    char *data = (char*) malloc (sizeof (char) * sizeToRead);
    int cont = 0;

    while (bytesRead < fileSize) {    
        memset(data, '\0', sizeToRead);
        cont = read(fd_file, data, sizeToRead);
        bytesRead = bytesRead + cont;   

        memset(&tramaFitxer, '\0', 256);
        trames_createEnviarFitxer(tramaFitxer, data);
       
        sleep(0.5);
        if(Connexions_send(tramaFitxer, fd_worker)==-1){
            return -1;
        }
    }
    close(fd_file);
    return 0;
}


int utils_getFileSize(char *path){
    int fd_file = open(path, O_RDONLY);
    if (fd_file == -1) {
        perror("Error obrint el fitxer");
    }
    // Usa lseek para moverte al final del archivo y obtener el tamaño
    int fileSize = lseek(fd_file, 0, SEEK_END);
    if (fileSize == -1) {
        perror("Error al calcular el tamaño del fichero");
        close(fd_file);
        return -1;
    }

    close(fd_file);
    return fileSize;
}


char *utils_intToChar(int num){
    char *num_char = NULL;
    asprintf(&num_char, "%d", num);
    return num_char;
}



int utils_comparaMD5SUM(int fd_file, char *MD5SUM, char *path){
    fd_file = open(path, O_WRONLY, 0666);
    lseek(fd_file, 0, SEEK_SET);
    char MD5SUM_nou[32];
    //memset(&MD5SUM_nou, '\0', 32);
    char* md5 = utils_getMD5SUMfromFIle(path);
    if (md5 == NULL) {
        perror("Error: utils_getMD5SUMfromFIle ha retornat NULL");
        pthread_exit(NULL);
    }
    strcpy(MD5SUM_nou, md5);
    free(md5);
    close(fd_file);
    if(strcmp(MD5SUM_nou, MD5SUM) == 0){
        printF("Els MD5SUM coincideixen\n");
    } else {
        return -1;
    }
    return 0;
}




int utils_comparaMD5SUMA(int fd_file, char *MD5SUM, char *path, int fd){
    fd_file = open(path, O_WRONLY, 0666);

    lseek(fd_file, 0, SEEK_SET);
    char MD5SUM_nou[32];

    memset(&MD5SUM_nou, '\0', 32);
    strcpy(MD5SUM_nou, utils_getMD5SUMfromFIle(path));
    close(fd_file);

    if(strcmp(MD5SUM_nou, MD5SUM) == 0){
        return 1;
    } else {
        return 0;
    }
}