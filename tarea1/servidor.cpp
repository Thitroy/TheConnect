#include "servidor.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <thread>
#include <errno.h>

using namespace std;

Servidor::Servidor() : server_fd_(-1) {
    memset(&address_, 0, sizeof(address_));
}

Servidor::~Servidor() {
    if (server_fd_ != -1) {
        close(server_fd_);
    }
}

bool Servidor::iniciar() {
    int opt = 1;

    if ((server_fd_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cerr << "Error al crear el socket del servidor" << endl;
        return false;
    }

    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        cerr << "Error al configurar el socket del servidor" << endl;
        return false;
    }

    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(PORT);

    if (bind(server_fd_, (struct sockaddr *)&address_, sizeof(address_)) < 0) {
        cerr << "Error al enlazar el socket del servidor" << endl;
        return false;
    }

    if (listen(server_fd_, 3) < 0) {
        cerr << "Error al escuchar en el socket del servidor" << endl;
        return false;
    }

    return true;
}

void Servidor::esperarConexiones() {
    int addrlen = sizeof(address_);

    cout << "Servidor iniciado. Esperando conexiones entrantes..." << endl;

    while (true) {
        int new_socket;
        if ((new_socket = accept(server_fd_, (struct sockaddr *)&address_, (socklen_t *)&addrlen)) < 0) {
            cerr << "Error al aceptar la conexión" << endl;
            continue;
        }

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        int bytes_recibidos = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_recibidos < 0) {
            cerr << "Error al recibir mensaje de confirmación del cliente" << endl;
            close(new_socket);
            continue;
        }
        buffer[bytes_recibidos] = '\0';

        if (strcmp(buffer, "Listo para jugar") != 0) {
            cerr << "Mensaje de confirmación incorrecto recibido del cliente" << endl;
            close(new_socket);
            continue;
        }

        thread(&Servidor::jugarPartida, this, new_socket).detach();
    }
}

void Servidor::jugarPartida(int client_socket) {
    char tablero[FILAS][COLUMNAS] = { {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '} };

    while (true) {
        try {
            recibirMovimiento(client_socket, tablero);
        } catch (const std::runtime_error& e) {
            cerr << "Juego terminado: " << e.what() << endl;
            close(client_socket);
            break;
        }
    }
}

void Servidor::recibirMovimiento(int client_socket, char tablero[FILAS][COLUMNAS]) {
    if (client_socket < 0) {
        cerr << "Socket inválido al inicio de recibirMovimiento" << endl;
        throw std::runtime_error("Socket inválido");
    }

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    int bytes_recibidos = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_recibidos < 0) {
        cerr << "Error al recibir las coordenadas del movimiento: " << errno << endl;
        close(client_socket);
        throw std::runtime_error("Error al recibir datos");
    } else if (bytes_recibidos == 0) {
        cerr << "Conexión cerrada por el cliente" << endl;
        close(client_socket);
        throw std::runtime_error("Conexión cerrada por el cliente");
    }

    buffer[bytes_recibidos] = '\0';
    cout << "Datos recibidos: " << buffer << endl;

    int fila, columna;
    if (sscanf(buffer, "%d %d", &fila, &columna) != 2 || 
        fila < 0 || fila >= FILAS || 
        columna < 0 || columna >= COLUMNAS) {
        const char* mensaje_error = "Movimiento inválido. Introduce coordenadas válidas.\n";
        send(client_socket, mensaje_error, strlen(mensaje_error), 0);
        cerr << "Error: Coordenadas inválidas recibidas" << endl;
        return;
    }

    if (tablero[fila][columna] != ' ') {
        const char* mensaje_error = "La casilla seleccionada ya está ocupada. Introduce otras coordenadas.\n";
        send(client_socket, mensaje_error, strlen(mensaje_error), 0);
        cerr << "Error: Casilla seleccionada ya ocupada" << endl;
        return;
    }

    tablero[fila][columna] = 'X';

    if (verificarVictoria(tablero, 'X')) {
        const char* mensaje_victoria = "¡Has ganado!\n";
        send(client_socket, mensaje_victoria, strlen(mensaje_victoria), 0);
        close(client_socket);
        throw std::runtime_error("Victoria del jugador");
    }

    bool empate = true;
    for (int col = 0; col < COLUMNAS; ++col) {
        if (tablero[0][col] == ' ') {
            empate = false;
            break;
        }
    }
    if (empate) {
        const char* mensaje_empate = "¡Empate! El tablero está lleno y nadie ha ganado.\n";
        send(client_socket, mensaje_empate, strlen(mensaje_empate), 0);
        close(client_socket);
        throw std::runtime_error("Empate en el juego");
    }

    enviarTablero(client_socket, tablero);
}

void Servidor::enviarTablero(int client_socket, char tablero[FILAS][COLUMNAS]) {
    string tablero_str;
    for (int fila = 0; fila < FILAS; ++fila) {
        for (int col = 0; col < COLUMNAS; ++col) {
            tablero_str += tablero[fila][col];
            if (col < COLUMNAS - 1) {
                tablero_str += ' ';
            }
        }
        tablero_str += '\n';
    }

    send(client_socket, tablero_str.c_str(), tablero_str.length(), 0);
}

bool Servidor::verificarVictoria(char tablero[FILAS][COLUMNAS], char ficha) {
    for (int fila = 0; fila < FILAS; ++fila) {
        for (int col = 0; col <= COLUMNAS - 4; ++col) {
            if (tablero[fila][col] == ficha &&
                tablero[fila][col+1] == ficha &&
                tablero[fila][col+2] == ficha &&
                tablero[fila][col+3] == ficha) {
                return true;
            }
        }
    }

    for (int col = 0; col < COLUMNAS; ++col) {
        for (int fila = 0; fila <= FILAS - 4; ++fila) {
            if (tablero[fila][col] == ficha &&
                tablero[fila+1][col] == ficha &&
                tablero[fila+2][col] == ficha &&
                tablero[fila+3][col] == ficha) {
                return true;
            }
        }
    }

    for (int fila = 0; fila <= FILAS - 4; ++fila) {
        for (int col = 0; col <= COLUMNAS - 4; ++col) {
            if (tablero[fila][col] == ficha &&
                tablero[fila+1][col+1] == ficha &&
                tablero[fila+2][col+2] == ficha &&
                tablero[fila+3][col+3] == ficha) {
                return true;
            }
        }
    }

    for (int fila = 0; fila <= FILAS - 4; ++fila) {
        for (int col = COLUMNAS - 1; col >= 3; --col) {
            if (tablero[fila][col] == ficha &&
                tablero[fila+1][col-1] == ficha &&
                tablero[fila+2][col-2] == ficha &&
                tablero[fila+3][col-3] == ficha) {
                return true;
            }
        }
    }

    return false;
}
