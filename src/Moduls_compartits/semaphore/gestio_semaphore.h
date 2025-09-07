#ifndef GESTIO_SEMAPHORE_H_
#define GESTIO_SEMAPHORE_H_

#include "semaphore.h"
#include <string.h> //Include de la llibreria string per a les funcions de strings
#include <stdlib.h> //Include de la llibreria stdlib per a les funcions de memoria
#include <stdio.h> //Include de la llibreria stdio per a les funcions d'entrada/sortida
#include <errno.h> //Include de la llibreria errno per a la gestio d'errors



void GestioSempahore_initSemaphore(int initial_value);
void GestioSempahore_destroySemaphore();
void GestioSempahore_semaphoreWait();
void GestioSempahore_semaphoreSignal();

#endif /* GESTIO_SEMAPHORE_H_ */