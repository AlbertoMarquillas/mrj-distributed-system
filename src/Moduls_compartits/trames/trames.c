#include "trames.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>



/*
* @Nom: trames_calcularChecksum
* @Definicio: Calcular el checksum de la trama
* @Arg: char *trama: Trama de la que es vol calcular el checksum
* @Ret: uint16_t: Valor del checksum   
*/
uint16_t trames_calcularChecksum(char *trama) {
    uint16_t checksum = 0;
    checksum = (unsigned char)trama[0];
    checksum = checksum + (unsigned char)trama[1];
    checksum = checksum + (unsigned char)trama[2];
    for(int i = 2; i < 256 - 6; i++){
        checksum = checksum + trama[i];
    }

    for(int i = (256 - 4); i < 256; i++){
        checksum = checksum + (unsigned char)trama[i];
    }

    checksum = checksum % 65536;
    return checksum;
}


/*
* @Nom: trames_verificarChecksum
* @Definicio: Calcula el checksum de la trama rebuda i verifica si és correcte
* @Arg: char *trama: Trama a verificar
* @Ret: int: 0 si el checksum és correcte, -1 si no ho és   
*/
int trames_verificarChecksum(char *trama) {
    uint16_t checksum;
    memcpy(&checksum, trama + 256 - 6, sizeof(checksum));
    uint16_t nuevoChecksum = trames_calcularChecksum(trama);
    if(checksum != nuevoChecksum){
        return -1;
    } else {
        return 0;
    }
}


/*
* @Nom: trames_contruitFormatTrama
* @Definicio: Constueix la trama a partir de les variables necessàries
* @Argg: char *data: Dades a enviar
* @Arg: uint8_t type: Tipus de trama
* @Arg: short data_length: Longitud de les dades
* @Arg: uint32_t timestamp: Temps en que s'ha creat la trama
* @Ret: char*: Trama construida
*/
void trames_contruitFormatTrama(char *trama, char *data, uint8_t type, uint16_t data_length, uint32_t timestamp) {
    
    uint16_t checksum;
    int i = 0;

    memset(trama, '\0', 256);  // Inicializa la trama

    // Copiar type
    memcpy(trama + i, &type, sizeof(type));
    i += sizeof(type);

    // Copiar data_length en big-endian
    uint16_t data_length_be = htons(data_length);
    memcpy(trama + i, &data_length_be, sizeof(data_length_be));
    i += sizeof(data_length_be);

    // Copiar data
    if (data != NULL) {
        memcpy(trama + i, data, data_length);
        i += 2;
    }

    // Copiar timestamp y checksum (al final)
    memcpy(trama + 256 - sizeof(timestamp), &timestamp, sizeof(timestamp));
    checksum = trames_calcularChecksum(trama);
    memcpy(trama + 256 - sizeof(timestamp) - sizeof(checksum), &checksum, sizeof(checksum));
}



/*
* @Nom: trames_createTramaConnection
* @Definicio: Funció que crea una trama de connexió
* @Arg: char *data_info: Tipus de worker o nom d'usuari fleck
* @Arg: char *ip: IP del servidor
* @Arg: char *port: Port del servidor
* @Ret: char*: Trama de connexió a enviar
*/
void trames_createTramaConnection(char *trama, char *data_info, char *ip, char *port) {

    uint8_t type;
    uint16_t data_length;
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);

    //Asignar type en funció de si és worker o fleck
    if (strcmp(data_info, MEDIA) == 0 || strcmp(data_info, TEXT) == 0) {
        type = 0x02;
    } else {
        type = 0x01;
    }

    //Asegurar que termina en '\0'
    data_info[strlen(data_info)] = '\0';
    ip[strlen(ip)] = '\0';
    port[strlen(port)] = '\0';

    // Asegúrate de asignar memoria suficiente para los datos
    char *data = (char *)malloc(strlen(data_info) + strlen(ip) + strlen(port) + 3); // +3 para 2 '&' y '\0'
    memset(data, '\0', strlen(data_info) + strlen(ip) + strlen(port) + 3);

    if (data == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }

    asprintf(&data, "%s&%s&%s", data_info, ip, port);
    data[strlen(data)] = '\0';

    // Calcula la longitud de los datos
    data_length = (uint16_t)strlen(data);    
    trames_contruitFormatTrama(trama, data, type, data_length, timestamp);
}




/*
* @Nom: trames_verificarTramaConnection
* @Definicio: Funció que verifica si la trama de connexió s'ha rebut correctament
* @Arg: char *msj: Trama a verificar
* @Ret: char*: Trama de resposta
*/
void trames_verificarTramaConnection(char *trama, char *msj, uint8_t type) {
    char *data = NULL;
    
    data = trames_obteData(msj);
    memcpy(msj, &type, sizeof(type));
    memset(trama, '\0', 256);
    if(trames_verificarChecksum(msj) == -1){
        trames_createTramaKO(trama, type, "CON_KO"); //Checksum incorrecte
    } else {
        trames_createTramaOK(trama, type, NULL);   //Checksum correcte
    }

    if(data != NULL){
        free(data);
    }
}


/*
* @Nom: trames_createTramaOK
* @Definicio: Funció que crea una trama de resposta OK
* @Arg: uint8_t type: Tipus de trama
* @Ret: char*: Trama de resposta OK
*/
void trames_createTramaOK(char *trama, uint8_t type, char *data) {
    uint16_t data_length;
    if(data == NULL){
        data_length = (uint16_t) 0;
    } else {
        data_length = (uint16_t)strlen(data);
    }
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);
    trames_contruitFormatTrama(trama, data, type, data_length, timestamp);
}


/*
* @Nom: trames_createTramaKO
* @Definicio: Funció que crea una trama de resposta KO
* @Arg: uint8_t type: Tipus de trama    
* @Arg: char *data: Dades a enviar
* @Ret: char*: Trama de resposta KO
*/
void trames_createTramaKO(char *trama, uint8_t type, char *data) {
    uint32_t timestamp = (uint32_t)time(NULL);
    uint16_t data_length = (uint16_t)strlen(data); //Longitud de les dades (CON_KO)
    trames_contruitFormatTrama(trama, data, type, data_length, timestamp);
}

/*
* @Nom: trames_peticioDistort
* @Definició: Crea la trama de petició de distorsió de fleck a gotham
* @Arg: char *mediaType: Pot ser media o text
* @Arg: char *fileName: Nom del fitxer a distorsionar
* @Ret: char *trama: Trama a enviar
*/
void trames_createPeticioDistort(uint8_t type, char *trama, char *mediaType, char *fileName){
    uint16_t data_length;
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);
    
    //Crear la cadena data amb la info i calcular data_length
    char *data = NULL;
    asprintf(&data, "%s&%s", mediaType, fileName); 
    data_length = (uint16_t)strlen(data); 
    memset(trama, '\0', 256);

    trames_contruitFormatTrama(trama, data, type, data_length, timestamp); //Construir la trama
}


/*
* @Nom: trames_createRespostaPeticioDistort
* @Definició: Crea la trama de resposta de petició de distorsió (gotham -> fleck)
* @Arg: char *ip: IP d'on s'ha de connectar 
* @Arg: char *port: port 'on s'haurà de connectar
* @Arg: int ok: boolea que indica si hi ha un worker disponible per connectar-se
* @Ret: char *trama: Trama a enviar
*/
void trames_createRespostaPeticioDistort(uint8_t type, char *trama, char *ip, char *port, int ok){
    uint16_t data_length;
    char *data = NULL;
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);

    if(ok == 1){
        //Asegurar que termina en '\0'
        ip[strlen(ip)] = '\0';
        port[strlen(port)] = '\0';
        asprintf(&data, "%s&%s", ip, port);
        data[strlen(data)] = '\0';
        data_length = (uint16_t)strlen(data);
        trames_contruitFormatTrama(trama, data, type, data_length, timestamp);
    } else {
        trames_createTramaKO(trama, type, "DISTORT_KO");
    }
}


/*
* @Nom: trames_createEnviarDistort
* @Definició: Crea la trama de distorsió que fleck envia al worker
* @Arg: char *mediaType: Pot ser media o text
* @Arg: char *fileName: Nom del fitxer a distorsionar
* @Arg: char *fileSize: Tamany del fitxer
* @Arg: char *MD5SUM: MD5SUM del fitxer
* @Arg: char *factor: Factor de distorsió
* @Ret: char *trama: Trama a enviar
*/
void trames_createEnviarDistort(char *trama, char *userName, char *fileName, char* fileSize, char* MD5SUM, char *factor){
    uint8_t type = 0x03;
    uint16_t data_length;
    uint32_t timestamp = (uint32_t)time(NULL);
    //Crear la cadena data amb la info i calcular data_length
    char *data = NULL;
    asprintf(&data, "%s&%s&%s&%s&%s", userName, fileName, fileSize, MD5SUM, factor);
    data[strlen(data)] = '\0';
    data_length = (uint16_t)strlen(data);
    memset(trama, '\0', 256);
    trames_contruitFormatTrama(trama, data, type, data_length, timestamp); //Construir la trama
}


/*
* @Nom: trames_createDistortFile
* @Definició: Crea la trama de petició de distorsió que el worker enviara a fleck
* @Arg: char *fileSize: Tamany del fitxer
* @Arg: char *MD5SUM: MD5SUM del fitxer
* @Ret: char *trama: Trama a enviar
*/
void trames_createDistortFile(char *trama, char *fileSize, char *MD5SUM){
    
    uint8_t type = 0x04;
    uint16_t data_length;
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);
    //Asegurar que termina en '\0'
    fileSize[strlen(fileSize)] = '\0';
    MD5SUM[strlen(MD5SUM)] = '\0';

    //Crear la cadena data amb la info i calcular data_length
    char *data = NULL;
    asprintf(&data, "%s&%s", fileSize, MD5SUM);
    data[strlen(data)] = '\0';
    data_length = (uint16_t)strlen(data);
    trames_contruitFormatTrama(trama, data, type, data_length, timestamp); //Construir la trama
    
    free(data);
}

void trames_createEnviarFitxer(char *trama, char *data){
    uint8_t type = 0x05;
    uint16_t data_length;
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);
    data_length = (uint16_t) 247;
    trames_contruitFormatTrama(trama, data, type, data_length, timestamp); //Construir la trama
}



void trames_createFinalFitxer(char *trama, int error){
    uint8_t type = 0x06;
    if(error == 0){
        trames_createTramaOK(trama, type, "CHECK_OK");
    } else {
        trames_createTramaKO(trama, type, "CHECK_KO");
    }
}

/*
* @Nom: trames_TramaErronia
* @Definició: Crear la trama que cal enviar quan la trama rebuda és errònia (p.ex error de checksum)
* @Ret: Trama a enviar
*/
void trames_TramaErronia(char *trama){

    uint8_t type = 0x09;
    uint16_t data_length = 0;
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);
    trames_contruitFormatTrama(trama, NULL, type, data_length, timestamp); //Construir la trama
}


/*
* @Nom: trames_createTramaLogout
* @Definició: Crear la trama de logout de Fleck → Gotham, Fleck → Worker o Worker → Gotham
* @Arg: char *data: data a enviar (por ser userName de Fleck o mediaType)
* @Ret: Trama a enviar
*/
void trames_createTramaLogout(char *trama, char *data){
    uint8_t type = 0x07;
    uint16_t data_length = (uint16_t)strlen(data);
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);
    trames_contruitFormatTrama(trama, data, type, data_length, timestamp); //Construir la trama
}


/*
* @Nom: trames_createWorkerPrincipal
* @Definició: Crear la trama de nou worker per assignar un nou worker principal
* @Ret: Trama a enviar
*/
void trames_createWorkerPrincipal(char *trama){
    uint8_t type = 0x08;
    uint16_t data_length = 0;
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);
    trames_contruitFormatTrama(trama, NULL, type, data_length, timestamp); //Construir la trama
}


/*
* @Nom: trames_createHeartBeat
* @Definició: Crear la trama de HeartBeat   
* @Ret: Trama a enviar  
*/
void trames_createHeartBeat(char *trama){
    uint8_t type = 0x12;
    uint16_t data_length = 0;
    uint32_t timestamp = (uint32_t)time(NULL);
    memset(trama, '\0', 256);
    trames_contruitFormatTrama(trama, NULL, type, data_length, timestamp); //Construir la trama
}


/*
* @Nom: trames_obteTramaType
* @Definició: Obtenir el type de la trama (byte 0)
* @Arg: char *trama: Trama a analitzar
* @Ret: uint8_t: Type de la trama
*/
uint8_t trames_obteTramaType(char *trama){
    uint8_t type;
    memcpy(&type, trama, 1);

    return type;
}


/*
* @Nom: trames_obteData
* @Definició: Obtenir les dades que hi ha a la trama
* @Arg: char *trama: Trama a analitzar
* @Ret: char *: Dades de la trama (data)
*/
char *trames_obteData(char *trama){
    if (trama == NULL) {
        perror("Trama és NULL a trames_obteData\n");
        return NULL;
    }
    

    uint8_t type;
    uint16_t data_length;
    int cont = 0;

    memcpy(&type, trama + cont, sizeof(type));
    cont += sizeof(type);
    memcpy(&data_length, trama + cont, sizeof(data_length));
    data_length = ntohs(data_length);

    cont += sizeof(data_length);

    char *data = (char *)malloc(data_length + 1);
    if (!data) {
        perror("Error al asignar memoria para 'data'");
        return NULL;
    }

    memcpy(data, trama + cont, data_length);
    data[data_length] = '\0'; // Finaliza la cadena

    return data;
}



/*
* @Nom: trames_obteType
* @Definició: Obtenir el type que hi ha a la data (Media, Text o UserName)
* @Arg: char *trama: Trama a analitzar
* @Ret: char *: Info del type de la data
*/
void trames_obteTypeWorker(char *trama, char **typeWorker) { // Nota el uso de char**
    

    char *data = trames_obteData(trama);
    if (data == NULL) {
        return;
    }

    char *token = strtok(data, "&");
    if (token != NULL) {
        *typeWorker = strdup(token); // Asigna memoria con strdup
    }

    free(data);
}



void trames_obteTimeStamp(char *trama, char *timeStamp) {
    if (trama == NULL || timeStamp == NULL) return;

    // Extraer los últimos 4 bytes (timestamp)
    uint32_t raw_time;
    memcpy(&raw_time, trama + 252, sizeof(uint32_t));

    time_t t = (time_t)raw_time;
    struct tm* tm_info = localtime(&t);
    if (!tm_info) return;

    // Solo formateamos dentro del buffer recibido
    strftime(timeStamp, 20, "%Y-%m-%d %H:%M:%S", tm_info);
}





/*
* @Nom: trames_obtePort
* @Definició: Obtenir el port de la trama que es troba a la data
* @Arg: char *trama: Trama a analitzar
* @Ret: char *: Port
*/
char *trames_obtePort(char *trama) {
    // Obtener los datos de la trama
    

    char *data = trames_obteData(trama);
    if (data == NULL) {
        return NULL; // Si no hay datos, devolver NULL
    }

    // Extraer los tokens de la trama
    strtok(data, "&");
    strtok(NULL, "&");
    char *token3 = strtok(NULL, "&");

    // Duplicar el tercer token (port) si existe
    char *port = NULL;
    if (token3 != NULL) {
        port = strdup(token3);
    }

    // Liberar la memoria asignada a 'data'
    if(data != NULL){
        free(data);
    }

    // Devolver el puerto duplicado
    return port;
}

/*
* @Nom: trames_obteIp
* @Definició: Obtenir la IP de la trama que es troba a la data
* @Arg: char *trama: Trama a analitzar
* @Ret: char *: IP
*/
char *trames_obteIp(char *trama) {

    // Obtener los datos de la trama
    char *data = trames_obteData(trama);
    if (data == NULL) {
        return NULL; // Si no hay datos, devolver NULL
    }

    // Extraer los tokens de la trama
    strtok(data, "&");
    char *token2 = strtok(NULL, "&");

    // Duplicar el segundo token (IP) si existe
    char *ip = NULL;
    if (token2 != NULL) {
        ip = strdup(token2);
    }

    // Liberar la memoria asignada a 'data'
    if(data != NULL){
        free(data);
    }

    // Devolver la IP duplicada
    return ip;
}



char *trames_obteUserName(char *trama) {
    char *data = trames_obteData(trama);
    if (data == NULL) {
        return NULL;
    }
    return strtok(data, "&");
}


char *trames_obteFileName(char *trama){
    char *data = trames_obteData(trama);
    if (data == NULL) {
        return NULL;
    }

     // Extraer los tokens de la trama
    strtok(data, "&");
    return strtok(NULL, "&");
}