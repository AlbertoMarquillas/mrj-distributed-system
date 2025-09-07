# Makefile - Gestió del client Fleck amb i sense Valgrind

CC = gcc
CFLAGS = -g -Wall -Werror -lm
VALGRIND = valgrind --dsymutil=yes --track-origins=yes --leak-check=full --track-fds=yes --show-reachable=yes -s

SRC = \
    fleck.c \
    Fleck_comms/fleck_comms.c \
    Fleck_comms/fleck_comms_functs.c \
    Fleck_funct/fleck_funct.c \
    Linkedlist/linkedlist_fleck.c \
    ../Moduls_compartits/connexions/connexions.c \
    ../Moduls_compartits/utils/utils.c \
    ../Moduls_compartits/trames/trames.c \
    ../Moduls_compartits/semaphore/semaphore.c \
    ../Moduls_compartits/semaphore/gestio_semaphore.c

STATIC_OBJ = ../Moduls_compartits/compression/so_compression.o

BIN = Fleck

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(SRC) $(STATIC_OBJ) $(CFLAGS) -o $(BIN)

# ---------- Execució normal ----------
run_puigpedros: $(BIN)
	./$(BIN) Fichers_de_configuracio/config_fleck_puigpedros.dat

run_montserrat: $(BIN)
	./$(BIN) Fichers_de_configuracio/config_fleck_montserrat.dat

run_matagalls: $(BIN)
	./$(BIN) Fichers_de_configuracio/config_fleck_matagalls.dat

# ---------- Execució amb Valgrind ----------
val_puigpedros: $(BIN)
	$(VALGRIND) ./$(BIN) Fichers_de_configuracio/config_fleck_puigpedros.dat

val_montserrat: $(BIN)
	$(VALGRIND) ./$(BIN) Fichers_de_configuracio/config_fleck_montserrat.dat

val_matagalls: $(BIN)
	$(VALGRIND) ./$(BIN) Fichers_de_configuracio/config_fleck_matagalls.dat

# ---------- Execució en segon pla ----------
run_puigpedros_bg:
	nohup ./$(BIN) Fichers_de_configuracio/config_fleck_puigpedros.dat > fleck_puigpedros.out 2>&1 &

run_montserrat_bg:
	nohup ./$(BIN) Fichers_de_configuracio/config_fleck_montserrat.dat > fleck_montserrat.out 2>&1 &

run_matagalls_bg:
	nohup ./$(BIN) Fichers_de_configuracio/config_fleck_matagalls.dat > fleck_matagalls.out 2>&1 &

# ---------- Neteja ----------
clean:
	rm -f $(BIN) *.o fleck_*.out
