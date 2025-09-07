# Makefile - Gestión de Harley con y sin Valgrind

CC = gcc
CFLAGS = -g -Wall -Werror -lpthread -lm
VALGRIND = valgrind --dsymutil=yes --track-origins=yes --leak-check=full --track-fds=yes --show-reachable=yes -s

SRC = \
    harley.c \
    Harley_funct/harley_funct.c \
    Harley_comms/harley_comms.c \
    Harley_comms/harley_comms_functs.c \
    Linkedlist/linkedlist_harley.c \
    ../Moduls_compartits/connexions/connexions.c \
    ../Moduls_compartits/utils/utils.c \
    ../Moduls_compartits/trames/trames.c \
    ../Moduls_compartits/semaphore/semaphore.c \
    ../Moduls_compartits/semaphore/gestio_semaphore.c

STATIC_OBJ = ../Moduls_compartits/compression/so_compression.o

BIN = Harley

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(SRC) $(STATIC_OBJ) $(CFLAGS) -o $(BIN)

# ---------- Ejecución SIN Valgrind ----------
run1: $(BIN)
	./$(BIN) Fichers_de_configuracio/config_harley_montserrat.dat

run2: $(BIN)
	./$(BIN) Fichers_de_configuracio/config_harley_montserrat_2.dat

# ---------- Ejecución CON Valgrind ----------
val1: $(BIN)
	$(VALGRIND) ./$(BIN) Fichers_de_configuracio/config_harley_montserrat.dat

val2: $(BIN)
	$(VALGRIND) ./$(BIN) Fichers_de_configuracio/config_harley_montserrat_2.dat

# ---------- Ejecución en segundo plano ----------
run1_bg:
	nohup ./$(BIN) Fichers_de_configuracio/config_harley_montserrat.dat > harley1.out 2>&1 &

run2_bg:
	nohup ./$(BIN) Fichers_de_configuracio/config_harley_montserrat_2.dat > harley2.out 2>&1 &

# ---------- Limpieza ----------
clean:
	rm -f $(BIN) *.o harley*.out

