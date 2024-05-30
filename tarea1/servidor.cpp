#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <errno.h>

#define PORT 7777
#define FILAS 6
#define COLUMNAS 7

bool verificarVictoria(char tablero[FILAS][COLUMNAS], char ficha);
void enviarTablero(int client_socket, char tablero[FILAS][COLUMNAS]);

void recibirMovimiento(int client_socket, char tablero[FILAS][COLUMNAS]) {
    if (client_socket < 0) {
        std::cerr << "Socket inválido al inicio de recibirMovimiento" << std::endl;
        return;
    }

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    int bytes_recibidos = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_recibidos < 0) {
        std::cerr << "Error al recibir las coordenadas del movimiento: " << errno << std::endl;
        if (errno == EBADF) {
            std::cerr << "Descriptor de archivo inválido (EBADF)" << std::endl;
        } else if (errno == ECONNRESET) {
            std::cerr << "Conexión restablecida por el cliente (ECONNRESET)" << std::endl;
        }
        close(client_socket);
        return;
    } else if (bytes_recibidos == 0) {
        std::cerr << "Conexión cerrada por el cliente" << std::endl;
        close(client_socket);
        return;
    }

    buffer[bytes_recibidos] = '\0';
    std::cout << "Datos recibidos: " << buffer << std::endl;

    int fila, columna;
    if (sscanf(buffer, "%d %d", &fila, &columna) != 2 || 
        fila < 0 || fila >= FILAS || 
        columna < 0 || columna >= COLUMNAS) {
        const char* mensaje_error = "Movimiento inválido. Introduce coordenadas válidas.\n";
        send(client_socket, mensaje_error, strlen(mensaje_error), 0);
        std::cerr << "Error: Coordenadas inválidas recibidas" << std::endl;
        return;
    }

    if (tablero[fila][columna] != ' ') {
        const char* mensaje_error = "La casilla seleccionada ya está ocupada. Introduce otras coordenadas.\n";
        send(client_socket, mensaje_error, strlen(mensaje_error), 0);
        std::cerr << "Error: Casilla seleccionada ya ocupada" << std::endl;
        return;
    }

    tablero[fila][columna] = 'X';

    bool victoria = verificarVictoria(tablero, 'X');
    if (victoria) {
        const char* mensaje_victoria = "¡Has ganado!\n";
        send(client_socket, mensaje_victoria, strlen(mensaje_victoria), 0);
        close(client_socket);
        return;
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
        return;
    }

    enviarTablero(client_socket, tablero);
}

void enviarTablero(int client_socket, char tablero[FILAS][COLUMNAS]) {
    std::string tablero_str;
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

bool verificarVictoria(char tablero[FILAS][COLUMNAS], char ficha) {
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

void jugarPartida(int client_socket) {
    char tablero[FILAS][COLUMNAS] = { {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '} };

    while (true) {
        recibirMovimiento(client_socket, tablero);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Error al crear el socket del servidor" << std::endl;
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Error al configurar el socket del servidor" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Error al enlazar el socket del servidor" << std::endl;
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Error al escuchar en el socket del servidor" << std::endl;
        return -1;
    }

    std::cout << "Servidor iniciado. Esperando conexiones entrantes..." << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            std::cerr << "Error al aceptar la conexión" << std::endl;
            continue;  // Continuar esperando nuevas conexiones en caso de error
        }

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        int bytes_recibidos = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_recibidos < 0) {
            std::cerr << "Error al recibir mensaje de confirmación del cliente" << std::endl;
            close(new_socket);
            continue;
        }
        buffer[bytes_recibidos] = '\0';

        if (strcmp(buffer, "Listo para jugar") != 0) {
            std::cerr << "Mensaje de confirmación incorrecto recibido del cliente" << std::endl;
            close(new_socket);
            continue;
        }

        std::thread(jugarPartida, new_socket).detach();
    }

    return 0;
}
