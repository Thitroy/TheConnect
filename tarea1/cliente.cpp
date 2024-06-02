#include "Cliente.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

Cliente::Cliente(const string& server_ip, int port)
    : server_ip_(server_ip), port_(port), client_socket_(-1) {}

Cliente::~Cliente() {
    // Destructor para cerrar el socket si está abierto
    if (client_socket_ != -1) {
        close(client_socket_);
    }
}

bool Cliente::conectar() {
    struct sockaddr_in serv_addr;

    if ((client_socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error al crear el socket" << endl;
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, server_ip_.c_str(), &serv_addr.sin_addr) <= 0) {
        cerr << "Dirección IP inválida" << endl;
        close(client_socket_);
        return false;
    }

    if (connect(client_socket_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Error al conectar al servidor" << endl;
        close(client_socket_);
        return false;
    }

    return true;
}

bool Cliente::enviarMensaje(const string& mensaje) {
    if (send(client_socket_, mensaje.c_str(), mensaje.size(), 0) < 0) {
        cerr << "Error al enviar mensaje al servidor" << endl;
        return false;
    }
    return true;
}

bool Cliente::recibirMensaje(string& respuesta) {
    char buffer[256];
    int bytes_recibidos = recv(client_socket_, buffer, sizeof(buffer) - 1, 0);
    if (bytes_recibidos < 0) {
        cerr << "Error al recibir datos del servidor" << endl;
        return false;
    } else {
        buffer[bytes_recibidos] = '\0';
        respuesta = string(buffer);
        return true;
    }
}

void Cliente::jugar() {
    if (!conectar()) {
        return;
    }

    if (!enviarMensaje("Listo para jugar")) {
        return;
    }

    int fila, columna;
    cout << "Introduce la fila y columna del movimiento (ejemplo: 2 3): ";
    if (!(cin >> fila >> columna)) {
        cerr << "Entrada inválida" << endl;
        return;
    }

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%d %d", fila, columna);
    if (!enviarMensaje(buffer)) {
        return;
    }

    string respuesta;
    if (recibirMensaje(respuesta)) {
        cout << "Respuesta del servidor: " << respuesta << endl;
    }
}
