# Makefile - Gestión de Enigma con y sin Valgrind

CC = gcc
CFLAGS = -g -Wall -Werror -lpthread -lm
VALGRIND = valgrind --dsymutil=yes --track-origins=yes --leak-check=full --track-fds=yes --show-reachable=yes -s

SRC = \
    enigma.c \
    Enigma_funct/enigma_funct.c \
    Enigma_comms/enigma_comms.c \
    Enigma_comms/enigma_comms_functs.c \
    Linkedlist/linkedlist_enigma.c \
    ../Moduls_compartits/connexions/connexions.c \
    ../Moduls_compartits/utils/utils.c \
    ../Moduls_compartits/trames/trames.c \
    ../Moduls_compartits/semaphore/semaphore.c \
    ../Moduls_compartits/semaphore/gestio_semaphore.c

STATIC_OBJ = ../Moduls_compartits/compression/so_compression.o

BIN = Enigma

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(SRC) $(STATIC_OBJ) $(CFLAGS) -o $(BIN)

# ---------- Ejecución SIN Valgrind ----------
run1: $(BIN)
	./$(BIN) Fichers_de_configuracio/config_enigma_puigpedros.dat

run2: $(BIN)
	./$(BIN) Fichers_de_configuracio/config_enigma_puigpedros_2.dat

# ---------- Ejecución CON Valgrind ----------
val1: $(BIN)
	$(VALGRIND) ./$(BIN) Fichers_de_configuracio/config_enigma_puigpedros.dat

val2: $(BIN)
	$(VALGRIND) ./$(BIN) Fichers_de_configuracio/config_enigma_puigpedros_2.dat

# ---------- Ejecución en segundo plano ----------
run1_bg:
	nohup ./$(BIN) Fichers_de_configuracio/config_enigma_puigpedros.dat > enigma1.out 2>&1 &

run2_bg:
	nohup ./$(BIN) Fichers_de_configuracio/config_enigma_puigpedros_2.dat > enigma2.out 2>&1 &

# ---------- Limpieza ----------
clean:
	rm -f $(BIN) *.o enigma*.out

