/***********************************************
*
* @Proposit: Fitxer encarrefat de les connexions
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio: 16/10/2024
*
************************************************/
//Include del .h associat a aquest .c
#include "connexions.h"

/***********************************************
 * @Nom: Connexions_openServer
 * @Definicio: Funció que obre un servidor
 * @Arg: int port: Port del servidor
 * @Arg: char *ip: IP del servidor
 * @Ret: int: El socket del servidor si tot ha anat bé, -1 si hi ha hagut algun error
 * ***********************************************/
int Connexions_openServer(int port, char *ip) {

    struct sockaddr_in serv_addr;

    int fd_socket = socket(AF_INET, SOCK_STREAM, 0); // 0 porque es TCP
    if (fd_socket < 0) {
        perror("Error al abrir el socket");
        return -1;
    }

    bzero((char*) &serv_addr, sizeof(serv_addr)); // Inicializa la estructura a cero
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);  // Asignar la IP
    serv_addr.sin_port = htons(port);  // Asignar el puerto

    if (bind(fd_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket");
        close(fd_socket);  // Cerrar el socket si falla el bind
        return -1;
    }

    if (listen(fd_socket, 5) < 0) { // Escuchar conexiones entrantes
        perror("Error en listen");
        close(fd_socket);  // Cerrar el socket si falla listen
        return -1;
    }
    return fd_socket;
}


/***********************************************
 * @Nom: Connexions_acceptar
 * @Definicio: Funció que accepta una connexió
 * @Arg: int fd: File descriptor del socket
 * @Ret: int: El file descriptor del client si tot ha anat bé, -1 si hi ha hagut algun error
 * ***********************************************/
int Connexions_acceptar(int fd) {
    struct sockaddr_in cli_addr;
    socklen_t clilen;

    clilen = sizeof(cli_addr); // Tamaño de la estructura del cliente

    int fd_client = accept(fd, (struct sockaddr*) &cli_addr, &clilen); // Aceptar conexión
    // int fd_client = accept(fd, (struct sockaddr*) &cli_addr, &clilen); // Aceptar conexión
    if (fd_client < 0) {
        perror("Error al aceptar la conexión");
        // close(fd_client);  // Cerrar el socket de escucha si falla accept
        return -1;
    }
    return fd_client;
}

/***********************************************
 * @Nom: Connexions_read
 * @Definicio: Funció que reb un missatge del servidor
 * @Arg: int port: Port del servidor
 * @Arg: char *buffer: Cadena que conte la informacio a llegir
 * @Ret: char *: El missatge rebut
 * ***********************************************/
char *Connexions_read(int fd, char *buffer) {
    memset(buffer, '\0', 256);  // Inicializar el buffer a 0
    int bytes_read = read(fd, buffer, 256);

    if(bytes_read < 0) {
        perror("Error al leer del socket");
        return "ERROR";
    } else if (bytes_read == 0) {
        return "ERROR";
    }

    return buffer;
}

/***********************************************
 * @Nom: Connexions_send
 * @Definicio: Funció que envia un missatge al servidor
 * @Arg: char* msg: Missatge a enviar
 * @Arg: int fd: File descriptor del servidor
 * @Ret: int: 0 si tot ha anat bé, -1 si hi ha hagut algun error
 * ***********************************************/
int Connexions_send(char *trama, int fd) {
    int bytes_enviats = send(fd, trama, 256, 0);
    if (bytes_enviats < 0) {
        printF("\nError al enviar trama completa");
        return -1;
    } else if (bytes_enviats < 256) {
        printF("Trama enviada incompleta, no s'han enviat tots els bytes\n");
        return -1;
    }
    return 0;
}



/***********************************************
 * @Nom: Connexions_connectServer
 * @Definicio: Funció que connecta amb el servidor
 * @Arg: int port: Port del servidor
 * @Arg: char *ip: IP del servidor
 * @Ret: int: Socket de conexió si tot ha anat bé, -1 si hi ha hagut algun error
 ***********************************************/
int Connexions_connectServer(int port, char *ip) {

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0); // 0 porque es TCP
    if (socket_fd < 0) {
        perror("Error al crear el socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(ip);

    // Intentar conectar al servidor Gotham
    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error al conectarse al servidor");
        close(socket_fd);  // Cerrar el socket si hay un error en connect()
        return -1;
    }

    return socket_fd; // Retornar el socket de conexión
}