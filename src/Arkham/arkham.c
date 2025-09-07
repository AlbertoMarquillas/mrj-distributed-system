#include "arkham.h"

int fdpipe_arkham;

void Arkham_writeLog(char* text) {
    int fd = open("./log.txt", O_RDWR | O_CREAT, 0777);

    if (fd <= 0) {
        printF("Failed to open log.txt");
        return;
    }

    char* log;
    char* newFileContent = calloc(1, 1);  // Inicialitzem buida
    int found = 0;

    while (1) {

        log = Utils_readFileLimit(fd, '-');
        if (!log || *log == '\0') break;

        char* count = Utils_readFileLimit(fd, '\n');
        if (!count) break;

        if (strcmp(log, text) == 0) {
            int newCount = atoi(count) + 1;
            free(count);
            count = malloc(sizeof(char) * ((int)log10(newCount) + 2));
            sprintf(count, "%d", newCount);
            found = 1;
        }

        size_t newSize = strlen(newFileContent) + strlen(log) + strlen(count) + 3;
        char* temp = realloc(newFileContent, newSize);
        if (temp != NULL) {
            newFileContent = temp;
            strcat(newFileContent, log);
            strcat(newFileContent, "-");
            strcat(newFileContent, count);
            strcat(newFileContent, "\n");
        }

        free(log);
        free(count);
    }

    if (!found) {
        size_t extra = strlen(text) + 4;
        newFileContent = realloc(newFileContent, strlen(newFileContent) + extra);
        strcat(newFileContent, text);
        strcat(newFileContent, "-1\n");
    }

    lseek(fd, 0, SEEK_SET);
    write(fd, newFileContent, strlen(newFileContent));
    free(newFileContent);

    close(fd);
}

void handle_signal(int sig) {
    (void)sig;
    close(fdpipe_arkham);
    exit(0);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        const char* msg = "Error: Wrong number of arguments\n";
        write(1, msg, strlen(msg));
        exit(1);
    }
    fdpipe_arkham = atoi(argv[1]);

    signal(SIGTERM, handle_signal);

    semaphore semaphore_arkham;
    SEM_constructor_with_name(&semaphore_arkham, ftok("stats.txt", 1));
    SEM_init(&semaphore_arkham, 1);

    while (1) {
        char* text = NULL;    
        text = Utils_readFileLimit(fdpipe_arkham, '\0');

        if (text != NULL) {
            SEM_wait(&semaphore_arkham);
            Arkham_writeLog(text);
            SEM_signal(&semaphore_arkham);
            free(text);
        }
    }    

    return 0;
}
