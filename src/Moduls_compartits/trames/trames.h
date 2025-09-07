/***********************************************
*
* @Proposit: Fitxer .h de funcions auxiliars
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 11/10/2024
* @Data ultima modificacio:
*
************************************************/
//Degine _GNU_SOURCE
#define _GNU_SOURCE

//Create class UTILS
#ifndef TRAMES_
#define TRAMES_

#define MEDIA "Media"
#define TEXT "Text"

#define CONNEXIO_FLECK_GOTHAM 0x01
#define CONNEXIO_WORKER_GOTHAM 0x02
#define CONNEXIO_FLECK_WORKER 0x03
#define DISTORT_GOTHAM_FLECK 0x10
#define DISTORT_RESUME_GOTHAM_FLECK 0x11
#define DISTORT_WORKER_FLECK 0x04 //Trama ja distorsionada
#define DISTORT_DADES_FLECK 0x05 //Trama amb dades distorsionades
#define DISTORT_MD5SUM_CHECK 0x06
#define DISCONNECT 0x07
#define NEW_WORKER 0x08
#define WRONG_DATA 0x09
#define HEARTBEAT 0x12

//Includes
#include <string.h> //Include de la llibreria string per a les funcions de strings
#include <stdlib.h> //Include de la llibreria stdlib per a les funcions de memoria
#include <stdio.h> //Include de la llibreria stdio per a les funcions d'entrada/sortida
#include <errno.h> //Include de la llibreria errno per a la gestio d'errors
#include <unistd.h> //Include de la llibreria unistd per a la gestio de descriptores de fitxers
#include <arpa/inet.h> //Include de la llibreria arpa/inet per a la gestio de la xarxa
#include <sys/socket.h> //Include de la llibreria sys/socket per a la gestio de sockets
#include <netinet/in.h> //Include de la llibreria netinet/in per a la gestio de la xarxa
#include <pthread.h> //Include de la llibreria pthread per a la gestio de fils
#include <fcntl.h> //Include de la llibreria fcntl
#include <time.h>
#include <stdint.h>
/*
* @Nom: trames_calcularChecksum
* @Definicio: Calcular el checksum de la trama
* @Arg: char *trama: Trama de la que es vol calcular el checksum
* @Ret: uint16_t: Valor del checksum   
*/
uint16_t trames_calcularChecksum(char *trama);




/*
* @Nom: trames_verificarChecksum
* @Definicio: Calcula el checksum de la trama rebuda i verifica si és correcte
* @Arg: char *trama: Trama a verificar
* @Ret: int: 0 si el checksum és correcte, -1 si no ho és   
*/
int trames_verificarChecksum(char *trama);

/*
* @Nom: trames_contruitFormatTrama
* @Definicio: Constueix la trama a partir de les variables necessàries
* @Argg: char *data: Dades a enviar
* @Arg: uint8_t type: Tipus de trama
* @Arg: short data_length: Longitud de les dades
* @Arg: uint32_t timestamp: Temps en que s'ha creat la trama
* @Ret: char*: Trama construida
*/
void trames_contruitFormatTrama(char *trama, char *data, uint8_t type, uint16_t data_length, uint32_t timestamp);


/*
* @Nom: trames_createTramaConnection
* @Definicio: Funció que crea una trama de connexió
* @Arg: char *data_info: Tipus de worker o nom d'usuari fleck
* @Arg: char *ip: IP del servidor
* @Arg: char *port: Port del servidor
* @Ret: char*: Trama de connexió a enviar
*/
void trames_createTramaConnection(char *trama, char *data_info, char *ip, char *port);


/*
* @Nom: trames_verificarTramaConnection
* @Definicio: Funció que verifica si la trama de connexió s'ha rebut correctament
* @Arg: char *msj: Trama a verificar
* @Ret: char*: Trama de resposta
*/
void trames_verificarTramaConnection(char *trama, char *msj, uint8_t type);


/*
* @Nom: trames_createTramaOK
* @Definicio: Funció que crea una trama de resposta OK
* @Arg: uint8_t type: Tipus de trama
* @Ret: char*: Trama de resposta OK
*/
void trames_createTramaOK(char *trama, uint8_t type, char *data);


/*
* @Nom: trames_createTramaKO
* @Definicio: Funció que crea una trama de resposta KO
* @Arg: uint8_t type: Tipus de trama    
* @Arg: char *data: Dades a enviar
* @Ret: char*: Trama de resposta KO
*/
void trames_createTramaKO(char *trama, uint8_t type, char *data);


/*
* @Nom: trames_createPeticioDistort
* @Definició: Crea la trama de petició de distorsió de fleck a gotham
* @Arg: char *mediaType: Pot ser media o text
* @Arg: char *fileName: Nom del fitxer a distorsionar
* @Ret: char *trama: Trama a enviar
*/
void trames_createPeticioDistort(uint8_t type, char *trama, char *mediaType, char *fileName);

void trames_createEnviarFitxer(char *trama, char *data);

/*
* @Nom: trames_createRespostaPeticioDistort
* @Definició: Crea la trama de resposta de petició de distorsió (gotham -> fleck)
* @Arg: char *ip: IP d'on s'ha de connectar 
* @Arg: char *port: port 'on s'haurà de connectar
* @Arg: int ok: boolea que indica si hi ha un worker disponible per connectar-se
* @Ret: char *trama: Trama a enviar
*/
void trames_createRespostaPeticioDistort(uint8_t type, char *trama, char *ip, char*port, int ok);
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
void trames_createEnviarDistort(char *trama, char *userName, char *fileName, char* fileSize, char* MD5SUM, char *factor);


/*
* @Nom: trames_createDistortFile
* @Definició: Crea la trama de petició de distorsió que el worker enviara a fleck
* @Arg: char *fileSize: Tamany del fitxer
* @Arg: char *MD5SUM: MD5SUM del fitxer
* @Ret: char *trama: Trama a enviar
*/
void trames_createDistortFile(char *trama, char *fileSize, char *MD5SUM);


/*
* @Nom: trames_createFinalFitxer
* @Definició: Crear la trama de resposta un cop tot el fitxer distorsionat s'ha enviat
* @Arg: char *MDenviat: MD5SUM del fitxer enviat
* @Arg: char *MDcalculat: MD5SUM del fitxer calculat
* @Ret: Trama a enviar amb el resultat de la comprovació
*/
void trames_createFinalFitxer(char *trama, int error);


/*
* @Nom: trames_TramaErronia
* @Definició: Crear la trama que cal enviar quan la trama rebuda és errònia (p.ex error de checksum)
* @Ret: Trama a enviar
*/
void trames_TramaErronia(char *trama);

/*
* @Nom: trames_createTramaLogout
* @Definició: Crear la trama de logout de Fleck → Gotham, Fleck → Worker o Worker → Gotham
* @Arg: char *data: data a enviar (por ser userName de Fleck o mediaType)
* @Ret: Trama a enviar
*/
void trames_createTramaLogout(char *trama, char *data);

/*
* @Nom: trames_createNouWorker
* @Definició: Crear la trama de nou worker per assignar un nou worker principal
* @Ret: Trama a enviar
*/
void trames_createWorkerPrincipal(char *trama);


/*
* @Nom: trames_createHeartBeat
* @Definició: Crear la trama de HeartBeat   
* @Ret: Trama a enviar  
*/
void trames_createHeartBeat(char *trama);


/*
* @Nom: trames_obteTramaType
* @Definició: Obtenir el type de la trama (byte 0)
* @Arg: char *trama: Trama a analitzar
* @Ret: uint8_t: Type de la trama
*/
uint8_t trames_obteTramaType(char *trama);


/*
* @Nom: trames_obteData
* @Definició: Obtenir les dades que hi ha a la trama
* @Arg: char *trama: Trama a analitzar
* @Ret: char *: Dades de la trama (data)
*/
char *trames_obteData(char *trama);

/*
* @Nom: trames_obteType
* @Definició: Obtenir el type que hi ha a la data (Media, Text o UserName)
* @Arg: char *trama: Trama a analitzar
* @Ret: char *: Info del type de la data
*/
void trames_obteTypeWorker(char *trama, char **typeWorker);

/*
* @Nom: trames_obteIp
* @Definició: Obtenir la IP de la trama que es troba a la data
* @Arg: char *trama: Trama a analitzar
* @Ret: char *: IP
*/
char *trames_obteIp(char *trama);

/*
* @Nom: trames_obtePort
* @Definició: Obtenir el port de la trama que es troba a la data
* @Arg: char *trama: Trama a analitzar
* @Ret: char *: Port
*/
char *trames_obtePort(char *trama);

void trames_obteTimeStamp(char *trama, char *timeStamp);
char *trames_obteUserName(char *trama);
char *trames_obteFileName(char *trama);
#endif