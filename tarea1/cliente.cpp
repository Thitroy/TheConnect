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
        return -1;
    }

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error al conectar al servidor" << std::endl;
        return -1;
    }

    // Lógica del cliente aquí
    // Enviar mensaje al servidor
const char* message = "Hola servidor!";
if (send(client_socket, message, strlen(message), 0) < 0) {
    std::cerr << "Error al enviar mensaje al servidor" << std::endl;
    return -1;
}

    return 0;
}
