#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread> // Agregar esta línea para usar std::thread

#define PORT 7777
#define FILAS 6
#define COLUMNAS 7

void jugarPartida(int client_socket) {
    char tablero[FILAS][COLUMNAS] = {{' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    bool game_over = false;

    // Lógica del juego Cuatro en Línea
    while (!game_over) {
        // Aquí va la lógica del juego
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
