#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread> // Agregar esta línea para usar std::thread

#define PORT 7777
#define FILAS 6
#define COLUMNAS 7

// Declaración de funciones
bool verificarVictoria(char tablero[FILAS][COLUMNAS], char ficha);
void enviarTablero(int client_socket, char tablero[FILAS][COLUMNAS]);



void recibirMovimiento(int client_socket, char tablero[FILAS][COLUMNAS]) {
    // Recibir coordenadas del movimiento del cliente
    char buffer[256];
    int bytes_recibidos = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_recibidos <= 0) {
        std::cerr << "Error al recibir las coordenadas del movimiento" << std::endl;
        close(client_socket);
        return;
    }

    // Interpretar las coordenadas recibidas
    int fila, columna;
    if (sscanf(buffer, "%d %d", &fila, &columna) != 2 || 
        fila < 0 || fila >= FILAS || 
        columna < 0 || columna >= COLUMNAS) {
        // Enviar mensaje de error al cliente
        const char* mensaje_error = "Movimiento inválido. Introduce coordenadas válidas.\n";
        send(client_socket, mensaje_error, strlen(mensaje_error), 0);
        return;
    }

    // Verificar si la casilla está vacía
    if (tablero[fila][columna] != ' ') {
        // Enviar mensaje de error al cliente
        const char* mensaje_error = "La casilla seleccionada ya está ocupada. Introduce otras coordenadas.\n";
        send(client_socket, mensaje_error, strlen(mensaje_error), 0);
        return;
    }

    // Actualizar el tablero con el movimiento del cliente
    tablero[fila][columna] = 'X'; // Suponiendo que 'X' representa al jugador

    // Verificar si se ha cumplido una condición de victoria
    bool victoria = verificarVictoria(tablero, 'X'); // Suponiendo que 'X' representa al jugador
    if (victoria) {
        // Enviar mensaje de victoria al cliente
        const char* mensaje_victoria = "¡Has ganado!\n";
        send(client_socket, mensaje_victoria, strlen(mensaje_victoria), 0);
        // Cerrar conexión con el cliente
        close(client_socket);
        return;
    }

    // Verificar si hay un empate
    bool empate = true;
    for (int col = 0; col < COLUMNAS; ++col) {
        if (tablero[0][col] == ' ') {
            empate = false;
            break;
        }
    }
    if (empate) {
        // Enviar mensaje de empate al cliente
        const char* mensaje_empate = "¡Empate! El tablero está lleno y nadie ha ganado.\n";
        send(client_socket, mensaje_empate, strlen(mensaje_empate), 0);
        // Cerrar conexión con el cliente
        close(client_socket);
        return;
    }

    // Si no se ha cumplido una condición de victoria ni hay empate, enviar estado actualizado del tablero al cliente
    enviarTablero(client_socket, tablero);
 // envia el estado actualizado del tablero al cliente para que pueda ver el tablero después de su movimiento
}
void enviarTablero(int client_socket, char tablero[FILAS][COLUMNAS]) {
    // Concatenar el tablero en una cadena de caracteres
    std::string tablero_str;
    for (int fila = 0; fila < FILAS; ++fila) {
        for (int col = 0; col < COLUMNAS; ++col) {
            tablero_str += tablero[fila][col];
            if (col < COLUMNAS - 1) {
                tablero_str += ' '; // Separador entre columnas
            }
        }
        tablero_str += '\n'; // Salto de línea entre filas
    }

    // Enviar el tablero al cliente
    send(client_socket, tablero_str.c_str(), tablero_str.length(), 0);
}


// Verificar si se ha cumplido una condición de victoria
// Devuelve true si hay cuatro fichas consecutivas en alguna dirección
bool verificarVictoria(char tablero[FILAS][COLUMNAS], char ficha) {
    // Verificar horizontalmente
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

    // Verificar verticalmente
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

    // Verificar diagonalmente (de izquierda a derecha)
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

    // Verificar diagonalmente (de derecha a izquierda)
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

    return false; // No hay cuatro fichas consecutivas en ninguna dirección
}

void jugarPartida(int client_socket) {
    char tablero[FILAS][COLUMNAS] = {{' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    bool game_over = false;
    bool turno_jugador = true; // Variable para alternar entre turnos

    // Lógica del juego Cuatro en Línea
    while (!game_over) {
        // Mensaje para indicar de quién es el turno
        if (turno_jugador) {
            const char* mensaje_turno = "Es tu turno. Elige una columna.\n";
            send(client_socket, mensaje_turno, strlen(mensaje_turno), 0);
        } else {
            const char* mensaje_turno = "Espera tu turno. El oponente está moviendo.\n";
            send(client_socket, mensaje_turno, strlen(mensaje_turno), 0);
        }

        // Lógica para recibir movimientos del cliente y actualizar el tablero
        // Supongamos que el cliente envía un número de fial y columna como movimiento
        int columna; // Aquí debes recibir el movimiento del cliente (puedes usar la función recv)
        recibirMovimiento(client_socket, tablero);

        // Cambiar el turno para el próximo movimiento
        turno_jugador = !turno_jugador;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Crear socket del servidor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Error al crear el socket" << std::endl;
        return -1;
    }

    // Configurar opciones del socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Error al configurar el socket" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Asignar dirección al socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Error al asignar la dirección al socket" << std::endl;
        return -1;
    }

    // Esperar por conexiones entrantes
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Error al esperar por conexiones entrantes" << std::endl;
        return -1;
    }
    std::cout << "Servidor iniciado. Esperando conexiones entrantes..." << std::endl;

    while (true) {
        // Aceptar nueva conexión
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            std::cerr << "Error al aceptar la conexión" << std::endl;
            return -1;
        }

        // Crear un hilo para cada partida
        std::thread(jugarPartida, new_socket).detach();
    }

    return 0;
}
