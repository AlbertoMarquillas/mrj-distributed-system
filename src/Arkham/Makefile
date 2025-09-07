# Makefile per a Arkham

CC = gcc
CFLAGS = -Wall -Werror -lm -lpthread
VALGRIND = valgrind --dsymutil=yes --track-origins=yes --leak-check=full --track-fds=yes --show-reachable=yes -s

SRC = \
    arkham.c \
    ../Moduls_compartits/utils/utils.c \
    ../Moduls_compartits/trames/trames.c \
    ../Moduls_compartits/semaphore/semaphore.c \
    ../Moduls_compartits/semaphore/gestio_semaphore.c \
    ../Moduls_compartits/connexions/connexions.c

BIN = arkham.exe

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(SRC) -o $(BIN) $(CFLAGS)

# ---------- Execució ----------
# Per defecte el paràmetre d'entrada és 0 (fd pipe)
run: $(BIN)
	./$(BIN) 0

val: $(BIN)
	$(VALGRIND) ./$(BIN) 0

# ---------- Execució en segon pla ----------
run_bg: $(BIN)
	nohup ./$(BIN) 0 > arkham.out 2>&1 &

# ---------- Neteja ----------
clean:
	rm -f $(BIN) *.o arkham.out
