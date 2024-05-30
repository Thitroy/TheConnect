#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Agregar esta línea para usar inet_pton

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <dirección IP del servidor> <puerto>" << std::endl;
        return 1;
    }
    const char* server_ip = argv[1];
    int port = std::stoi(argv[2]);

    int client_socket;
    struct sockaddr_in serv_addr;

    // Crear socket del cliente
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error al crear el socket" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convertir dirección IP de texto a binario
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Dirección IP inválida" << std::endl;
        close(client_socket);
        return -1;
    }

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error al conectar al servidor" << std::endl;
        close(client_socket);
        return -1;
    }

    // Enviar mensaje de confirmación al servidor
    const char* mensaje_confirmacion = "Listo para jugar";
    if (send(client_socket, mensaje_confirmacion, strlen(mensaje_confirmacion), 0) < 0) {
        std::cerr << "Error al enviar mensaje de confirmación al servidor" << std::endl;
        close(client_socket);
        return -1;
    }

    // Obtener coordenadas del usuario
    int fila, columna;
    std::cout << "Introduce la fila y columna del movimiento (ejemplo: 2 3): ";
    if (!(std::cin >> fila >> columna)) {
        std::cerr << "Entrada inválida" << std::endl;
        close(client_socket);
        return -1;
    }

    // Enviar coordenadas al servidor
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%d %d", fila, columna);
    if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
        std::cerr << "Error al enviar coordenadas al servidor" << std::endl;
        close(client_socket);
        return -1;
    }

    // Recibir respuesta del servidor
    char respuesta[256];
    int bytes_recibidos = recv(client_socket, respuesta, sizeof(respuesta) - 1, 0);
    if (bytes_recibidos < 0) {
        std::cerr << "Error al recibir datos del servidor" << std::endl;
    } else {
        respuesta[bytes_recibidos] = '\0';
        std::cout << "Respuesta del servidor: " << respuesta << std::endl;
    }

    // Cerrar el socket
    close(client_socket);

    return 0;
}
