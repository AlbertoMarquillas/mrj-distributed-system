/***********************************************
*
* @Proposit: Fitxer principal del funcionament de Bowman.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio: 16/10/2024
*
************************************************/

// Incluir el .h asociado a las funciones de Gotham
#include "Gotham_comms/gotham_comms.h"

// Variables globals sense inicialitzar:
InfoWorker *info_worker;
InfoClient *info_client;
InfoThread *info_threads = NULL; // Puntero dinámico para los threads
int num_threads = 0;  // Número actual de threads creados

// Declaración global
GothamConfig gotham_config_file;


int fd_worker;
int fd_fleck;

pthread_t thread_worker;
pthread_t thread_fleck;

int thread_worker_created = 0;
int thread_fleck_created = 0;

int gotham_running = 1;

pid_t pid_monolit = -1;
int fdpipe[2];

int threads_joined = 0;

InfoThread** threads_array = NULL;
int threads_array_size = 0;


// Función para añadir un nuevo thread dinámicamente
void addThread(int index) {
    // Redimensionamos el array dinámico para agregar un nuevo thread
    num_threads++;
    info_threads = realloc(info_threads, num_threads * sizeof(InfoThread));

    if (info_threads == NULL) {
        perror("Error al redimensionar memoria pel thread");
        exit(1);
    }

    // Inicializamos el nuevo thread
    info_threads[num_threads - 1].thread_index = index;
    info_threads[num_threads - 1].info_worker = info_worker;
    info_threads[num_threads - 1].info_client = info_client;
    info_threads[num_threads - 1].gotham_running = &gotham_running;


    InfoThread *novaInfo = malloc(sizeof(InfoThread));
    if (novaInfo == NULL) {
        perror("Error al reservar memoria pel thread");
        exit(1);
    }   

    *novaInfo = info_threads[num_threads - 1]; // Copiamos la información del thread

    threads_array = realloc(threads_array, sizeof(InfoThread*) * (threads_array_size + 1));
    threads_array[threads_array_size] = novaInfo;
    threads_array_size++;

    if(index == 0){
        if (pthread_create(&thread_worker, NULL, GothamComms_connectWorker, (void*)novaInfo) != 0) {
            perror("Error al crear el thread worker\n");
            free(novaInfo); // Liberar la memoria del thread
            exit(1);
        }
        //pthread_detach(thread_worker);
    } else {
        if(pthread_create(&thread_fleck, NULL, GothamComms_connectFleck, (void*)novaInfo) != 0){
            perror("Error al crear el thread fleck\n");
            free(novaInfo); // Liberar la memoria del thread
            exit(1);
        } 
        //pthread_detach(thread_fleck);
        
    }
}

void Gotham_freeThreads(){
    if (!threads_joined) {
        if (thread_fleck_created && pthread_join(thread_fleck, NULL) != 0) { 
            perror("Error al hacer join del thread fleck\n");
        }
        threads_joined = 1;
    }

    // Liberamos la memoria de cada InfoThread
    for (int i = 0; i < threads_array_size; i++) {
        if (threads_array[i] != NULL) {
            free(threads_array[i]);
        }
    }

    if (info_threads != NULL) {
        free(info_threads);
        info_threads = NULL;
        num_threads = 0;
    }

    if(threads_array != NULL) {
        free(threads_array);
    }
    threads_array = NULL;
    threads_array_size = 0;
}


void Gotham_closeFileDescriptors(){
    if (fd_worker != -1) {
        shutdown(fd_worker, SHUT_RDWR);
        close(fd_worker);
        fd_worker = -1;
    }
    if (fd_fleck != -1) {
        shutdown(fd_fleck, SHUT_RDWR);
        close(fd_fleck);
        fd_fleck = -1;
    }
}

void Gotham_freeConfigFile() {
    if (gotham_config_file.worker_ip) {
        free(gotham_config_file.worker_ip);
        gotham_config_file.worker_ip = NULL;
    } 
    if (gotham_config_file.worker_port) {
        free(gotham_config_file.worker_port);
        gotham_config_file.worker_port = NULL;
    }
    if (gotham_config_file.fleck_ip) {
        free(gotham_config_file.fleck_ip);
        gotham_config_file.fleck_ip = NULL;
    }
    if (gotham_config_file.fleck_port) {
        free(gotham_config_file.fleck_port);
        gotham_config_file.fleck_port = NULL;
    }
}

void Gotham_closeMonolit(){
    if (pid_monolit != -1) {
        kill(pid_monolit, SIGTERM);
    }

    if (fdpipe[1] > 0) {
        close(fdpipe[1]);
        fdpipe[1] = -1;
    }
}

void Gotham_freeMemory(){
    Gotham_closeFileDescriptors();
    GothamComms_freeMem();
    Gotham_freeThreads();
    Gotham_freeConfigFile();
    Gotham_closeMonolit();

    free(info_worker);
    free(info_client);
    if (fd_fleck != -1) {
        close(fd_fleck);
        fd_fleck = -1;
    }
    if (fd_worker != -1) {
        close(fd_worker);
        fd_worker = -1;
    }
    if (fdpipe[0] != -1) {
        close(fdpipe[0]);
        fdpipe[0] = -1;
    }
    if (fdpipe[1] != -1) {
        close(fdpipe[1]);
        fdpipe[1] = -1;
    }    
}

void Gotham_signalHandler(int signum) {
    if (signum == SIGINT) {
        printF("\nGotham ha sigut desconnectat\n");
        printF("Tancant el sistema...\n");
        gotham_running = 0;  // Indicar a los threads que deben terminar
        sleep(1);  // Dar tiempo a los threads para salir
        Gotham_freeMemory();
        exit(0);  // Terminación exitosa
    }
}

void Gotham_handleProcess() {
    
    info_worker = malloc(sizeof(InfoWorker));
    info_client = malloc(sizeof(InfoClient));
    
    if (!info_worker || !info_client) {
        write(1, "Error al reservar memoria\n", strlen("Error al reservar memoria\n"));
        exit(1);
    }
    
    char *worker_port = gotham_config_file.worker_port;
    char *worker_ip = gotham_config_file.worker_ip;

    char *fleck_port = gotham_config_file.fleck_port;
    char *fleck_ip = gotham_config_file.fleck_ip;

    // Convertimos y verificamos los valores del puerto e IP de Harley/Enigma
    if (worker_port == NULL || worker_ip == NULL) {
        perror("Error: Configuración de Harley/Enigma inválida.\n");
        exit(1);
    }

    fd_worker = Connexions_openServer(atoi(worker_port), worker_ip);

    // Convertimos y verificamos los valores del puerto e IP de Fleck
    if (fleck_port == NULL || fleck_ip == NULL) {
        perror("Error: Configuración de Fleck inválida.\n");
        exit(1);
    }
    fd_fleck = Connexions_openServer(atoi(fleck_port), fleck_ip);

    // WORKER
    info_worker->fd = fd_worker;
    info_worker->ip = gotham_config_file.worker_ip;
    info_worker->port = gotham_config_file.worker_port;
    info_worker->type = NULL;
    info_worker->number_of_server = 0;

    // FLECK
    info_client->fd = fd_fleck;
    info_client->ip = gotham_config_file.fleck_ip;
    info_client->port = gotham_config_file.fleck_port;
    info_client->type = NULL;

    // Añadimos los threads
    addThread(0);  // Creas un thread para Worker
    addThread(1);  // Creas otro thread para Fleck
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        perror("ERROR_ARGS");
        exit(1);
    }

    signal(SIGINT, Gotham_signalHandler);
    signal(SIGPIPE, SIG_IGN);
    int error = GothamFunct_readConfigGotham(argv[1], &gotham_config_file);
    if (error == -1) {
        perror("ERROR_FILE");
        exit(1);
    }

    if(pipe(fdpipe) == -1) { // Create the pipe (used to communicate with Monolit)
        printF("Error: pipe failed\n");
        Gotham_freeMemory();
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        printF("Error: Fork failed\n");
        exit(1);
    } else if (pid == 0) { // Child process
        char fd_str[10];
        sprintf(fd_str, "%d", fdpipe[0]);

        char *args[] = {"../Arkham/arkham.exe", fd_str, NULL};
        execv(args[0], args);

        printF("Error: execv failed\n");
        Gotham_freeMemory();
        exit(1);
    } else { // Parent process
        pid_monolit = pid;  // Save the PID of the child process
        GothamCommsFuncts_setFdArkam(fdpipe[1]); // Setter per poder utilitzar la variable fdpipe[1]
        Gotham_handleProcess();
        while (gotham_running) {
            sleep(1); // Esperem 1 segon per iteració
        }
    }     

    return 0;
}