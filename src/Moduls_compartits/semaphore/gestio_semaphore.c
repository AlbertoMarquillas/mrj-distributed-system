#include "gestio_semaphore.h"


// Semáforo declarado como estático
static semaphore sem;

// Inicializa el semáforo
void GestioSempahore_initSemaphore(int initial_value) {
    if (SEM_constructor(&sem) < 0) {
        perror("Error al crear el semáforo");
        exit(1);
    }
    if (SEM_init(&sem, initial_value) < 0) {
        perror("Error al inicializar el semáforo");
        SEM_destructor(&sem);
        exit(1);
    }
}

// Destruye el semáforo
void GestioSempahore_destroySemaphore() {
    if (SEM_destructor(&sem) < 0) {
        perror("Error al destruir el semáforo");
    }
}

// Operaciones de wait y signal
void GestioSempahore_semaphoreWait() {
    if (SEM_wait(&sem) < 0) {
        perror("Error en SEM_wait");
    }
}

void GestioSempahore_semaphoreSignal() {
    if (SEM_signal(&sem) < 0) {
        perror("Error en SEM_signal");
    }
}
