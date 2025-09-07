#include "Enigma_funct/enigma_funct.h"
#include "Enigma_comms/enigma_comms.h"

int fd_fleck;
int fd_gotham;
EnigmaConfig enigma_config_file;
CuaContEnigma cont_enigma;

FleckThread* new_fleck_thread = NULL;
InfoThread* info_thread = NULL;
pthread_t thread_fleck[MAX_FLECKS];
int num_threads = 0;
pthread_t gotham_thread; 
LinkedList* linked_cua_global = NULL;

void gestio_cua_contEnigma(int estatConnexio) {
    cont_enigma.mtype = 1;
    int msgid_contEnigma = EnigmaFunct_accedeixCua(1);

    while(msgrcv(msgid_contEnigma, &cont_enigma, sizeof(CuaContEnigma) - sizeof(long), 1, IPC_NOWAIT) >= 0) {
    }

    if(estatConnexio == 0) cont_enigma.n_Enigma++;
    else cont_enigma.n_Enigma--;

    if ((msgsnd(msgid_contEnigma, &cont_enigma, sizeof(CuaContEnigma) - sizeof(long), 0) == -1) || (cont_enigma.mtype <= 0)) {
        perror("Error enviant missatge a la cua\n");
    }
}

void destroy_cues() {
    int msgid_enigma = EnigmaFunct_accedeixCua(0);   
    EnigmaFunct_eliminaCua(msgid_enigma);
    int msgid_fleck = EnigmaFunct_accedeixCua(1);
    EnigmaFunct_eliminaCua(msgid_fleck);
}

void EnigmaFunct_signalHandler(int sig) {
    switch (sig) {
        case SIGINT:
            char trama[MAX_PATH];
            memset(&trama, '\0', MAX_PATH);
            trames_createTramaLogout(trama, enigma_config_file.type);
            
            if (Connexions_send(trama, fd_gotham) < 0) {
                perror("Error al enviar logout a Gotham");
                exit(1);
            }

            sleep(1);

            GestioSempahore_semaphoreWait();
            gestio_cua_contEnigma(1);   
            if (cont_enigma.n_Enigma == 0) {
                destroy_cues();
                eliminarFitxersMediaFiles();
            }

            int msgid_infoFleck = EnigmaFunct_accedeixCua(0);
            int size = LINKEDLIST_getSize(linked_cua_global);

            LINKEDLIST_goToHead(linked_cua_global);
            while (size > 0) {
                CuaInfoFleck msgCua = LINKEDLIST_get(linked_cua_global);
                if ((msgsnd(msgid_infoFleck, &msgCua, sizeof(CuaInfoFleck) - sizeof(long), 0) == -1) || (msgCua.mtype <= 0)) {
                    perror("Error enviant missatge a la cua\n");
                }
                LINKEDLIST_next(linked_cua_global);
                size--;
            }
            GestioSempahore_semaphoreSignal();
            LINKEDLIST_destroy(linked_cua_global);
            GestioSempahore_destroySemaphore();
            
            pthread_cancel(gotham_thread);
            pthread_join(gotham_thread, NULL);  // Espera que el hilo termine limpiamente

            if (fd_fleck > 0) {
                close(fd_fleck);
            }
            if (fd_gotham > 0) {
                close(fd_gotham);
            }

            exit(0);
            break;

        case SIGUSR1:
            printF("\nEsperant que acabin les distorsions...\n");

            for (int i = 0; i < num_threads; i++) {
                pthread_join(thread_fleck[i], NULL);
            }
            printF("Distorsions finalitzades\n");
            eliminarFitxersMediaFiles();
            destroy_cues();
            close(fd_fleck);
            close(fd_gotham);
            exit(0);
            break;

        default:
            write(1, "Error al capturar el signal\n", 29);
            break;
    }
}

void init_recursos() {
    signal(SIGINT, EnigmaFunct_signalHandler);
    signal(SIGUSR1, EnigmaFunct_signalHandler);
    GestioSempahore_initSemaphore(1);

    linked_cua_global = LINKEDLIST_create();
    if (!linked_cua_global) {
        perror("Error creant linked list");
        exit(1);
    }

    int fd_cua_InfoFleck = open(KEY_INFO_FLECK, O_CREAT | O_WRONLY, 0666);
    if (fd_cua_InfoFleck == -1) {
        perror("Error creant fitxer clau InfoFleck");
        exit(1);
    }
    close(fd_cua_InfoFleck);

    int fd_cua_InfoEnigma = open(KEY_CONT_ENIGMA, O_CREAT | O_WRONLY, 0666);
    if (fd_cua_InfoEnigma == -1) {
        perror("Error creant fitxer clau ContEnigma");
        exit(1);
    }
    close(fd_cua_InfoEnigma);
}

void addThread(CuaInfoFleck msg, EnigmaConfig enigma_config_file, int fd_fleck) {
    num_threads++;

    // Realloc
    FleckThread* temp_fleck = realloc(new_fleck_thread, num_threads * sizeof(FleckThread));
    InfoThread* temp_info = realloc(info_thread, num_threads * sizeof(InfoThread));

    if (!temp_fleck || !temp_info) {
        perror("Error al reallocar memòria");
        exit(1);
    }

    new_fleck_thread = temp_fleck;
    info_thread = temp_info;

    // Inicialización del nou thread
    new_fleck_thread[num_threads - 1].fd = fd_fleck;
    new_fleck_thread[num_threads - 1].enigma_config_file = enigma_config_file;
    new_fleck_thread[num_threads - 1].fleck_id = msg.fleck_id;

    info_thread[num_threads - 1].linked_cua = linked_cua_global;

    //Copia de FleckThread
    FleckThread *fleck_copy = malloc(sizeof(FleckThread));
    if (!fleck_copy) {
        perror("Error al reservar memòria per a fleck_copy");
        exit(1);
    }
    memcpy(fleck_copy, &new_fleck_thread[num_threads - 1], sizeof(FleckThread));

    // Copia de InfoThread
    InfoThread *thread_data = malloc(sizeof(InfoThread));
    if (!thread_data) {
        perror("Error al reservar memòria per al thread");
        free(fleck_copy); // liberar si falla
        exit(1);
    }

    thread_data->info_fleck = fleck_copy;
    thread_data->linked_cua = linked_cua_global;
    thread_data->folderName = enigma_config_file.folder_name;
    // Crear el hilo con su copia independiente
    if (pthread_create(&thread_fleck[num_threads - 1], NULL, EnigmaComms_threadFleck, (void *)thread_data) != 0) {
        perror("Error al crear thread Fleck");
        free(fleck_copy);
        free(thread_data);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    init_recursos();

    if (argc != 2) {
        write(2, ERROR_ARGS, strlen(ERROR_ARGS));
        exit(1);
    }
    signal(SIGPIPE, SIG_IGN);
    if (EnigmaFunct_readConfigEnigma(argv[1], &enigma_config_file) == -1) {
        write(2, ERROR_FILE, strlen(ERROR_FILE));
        exit(1);
    }

    fd_gotham = EnigmaFunct_ConnectToGotham(enigma_config_file, linked_cua_global);
    if (fd_gotham < 0) {
        perror("Error connectant amb Gotham");
        exit(1);
    }

    gestio_cua_contEnigma(0);

    GothamThread *info_gotham_thread = malloc(sizeof(GothamThread));
    info_gotham_thread->fd = fd_gotham;
    if (pthread_create(&gotham_thread, NULL, EnigmaComms_heartbeatThreadGotham, (void *)info_gotham_thread) != 0) {
        perror("Error creant thread de heartbeat");
        exit(1);
    }

    if (EnigmaFunct_esperaWorkerPrincipal(fd_gotham) == -1) {
        perror("Error esperant worker principal");
        exit(1);
    }

    int fd = Connexions_openServer(atoi(enigma_config_file.fleck_port), enigma_config_file.fleck_ip);
    if (fd < 0) {
        perror("Error obrint servidor per Fleck");
        exit(1);
    }

    int msgid_infoFleck = EnigmaFunct_accedeixCua(0);

    while (1) {
        CuaInfoFleck msg;
        memset(&msg, 0, sizeof(CuaInfoFleck));
        sleep(1);

        fd_fleck = Connexions_acceptar(fd);

        if (msgrcv(msgid_infoFleck, &msg, sizeof(CuaInfoFleck) - sizeof(long), 1, IPC_NOWAIT) < 0) {
            if (errno == ENOMSG) {
                msg.estatDistort = 0;
                msg.n_bytes = 0;
                msg.mtype = 1;
                msg.fleck_id = num_threads;
            } else {
                perror("Error llegint de la cua");
                exit(1);
            }
        }

        GestioSempahore_semaphoreWait();
        LINKEDLIST_goToHead(linked_cua_global);
        while (!LINKEDLIST_isAtEnd(*linked_cua_global)) {
            LINKEDLIST_next(linked_cua_global);
        }
        LINKEDLIST_add(linked_cua_global, msg);
        GestioSempahore_semaphoreSignal();

        addThread(msg, enigma_config_file, fd_fleck);
    }

    return 0;
}
