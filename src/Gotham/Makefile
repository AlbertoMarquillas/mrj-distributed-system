# Compilador y banderas
CC = gcc
CFLAGS = -Wall -Werror -g

# Archivos fuente y destino
SRC = gotham.c \
      Gotham_funct/gotham_funct.c \
      Gotham_comms/gotham_comms.c \
      Gotham_comms/gotham_comms_functs.c \
      Linkedlist/linkedlist_gotham.c \
      ../Moduls_compartits/connexions/connexions.c \
      ../Moduls_compartits/utils/utils.c \
      ../Moduls_compartits/trames/trames.c

TARGET = Gotham

# Archivo de configuración por defecto
CONFIG = Fichers_de_configuracio/config_gotham_matagalls.dat

# Regla por defecto
all: $(TARGET)

# Compilación
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Ejecutar el programa
run: $(TARGET)
	./$(TARGET) $(CONFIG)

# Ejecutar con valgrind
valgrind: $(TARGET)
	valgrind --dsymutil=yes --track-origins=yes --leak-check=full --track-fds=yes --show-reachable=yes -s ./$(TARGET) $(CONFIG)

# Limpiar archivos generados
clean:
	rm -f $(TARGET)
