# Makefile para compilar y ejecutar el servidor y el cliente

# Compilador y flags
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Objetivos
TARGETS = servidor cliente

# Archivos fuente
SERVER_SRC = servidor.cpp main_servidor.cpp
CLIENT_SRC = cliente.cpp main_cliente.cpp

# Archivos objeto
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)

all: $(TARGETS)

# Reglas para compilar el servidor
servidor: $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o servidor $(SERVER_OBJ) -lpthread

# Reglas para compilar el cliente
cliente: $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o cliente $(CLIENT_OBJ)

# Reglas para compilar archivos .cpp en archivos .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar los archivos objeto y los ejecutables
clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(TARGETS)

.PHONY: all clean
