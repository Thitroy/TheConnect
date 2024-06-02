#include "cliente.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

Cliente::Cliente(const string& server_ip, int port)
    : server_ip_(server_ip), port_(port), client_socket_(-1) {
}

Cliente::~Cliente() {
    if (client_socket_ != -1) {
        close(client_socket_);
    }
}

bool Cliente::conectar() {
    if ((client_socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error al crear el socket" << endl;
        return false;
    }

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, server_ip_.c_str(), &serv_addr.sin_addr) <= 0) {
        cerr << "Dirección IP inválida" << endl;
        return false;
    }

    if (connect(client_socket_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Error al conectar al servidor" << endl;
        return false;
    }

    return true;
}

bool Cliente::enviarMensaje(const string& mensaje) {
    if (send(client_socket_, mensaje.c_str(), mensaje.length(), 0) < 0) {
        cerr << "Error al enviar mensaje al servidor" << endl;
        return false;
    }
    return true;
}

bool Cliente::recibirMensaje(string& respuesta) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer)); // Limpiar el buffer antes de recibir datos
    int bytes_recibidos = recv(client_socket_, buffer, sizeof(buffer) - 1, 0);  // Cambiado a client_socket_
    if (bytes_recibidos < 0) {
        cerr << "Error al recibir datos del servidor" << endl;
        return false;
    }
    buffer[bytes_recibidos] = '\0';
    respuesta = string(buffer);
    return true;
}

void Cliente::limpiarPantalla() {
    // Secuencias de escape ANSI para limpiar la terminal
    cout << "\033[2J\033[1;1H";
}

void Cliente::jugar() {
    if (!conectar()) {
        return;
    }

    string respuesta;
    if (recibirMensaje(respuesta)) {
        cout << respuesta << endl;
    }

    bool juego_terminado = false; // Variable para verificar si el juego ha terminado

    while (!juego_terminado) {
        limpiarPantalla();  // Limpiar la terminal antes de mostrar el tablero
        cout << respuesta << endl;  // Mostrar el tablero y cualquier mensaje recibido

        if (respuesta.find("Fin del juego") != string::npos) {
            juego_terminado = true; // Marcar que el juego ha terminado
            break;
        }

        if (respuesta.find("Inicia el servidor") != string::npos) {
            if (!recibirMensaje(respuesta)) {
                break;
            }
            limpiarPantalla();  // Limpiar la terminal antes de mostrar el tablero actualizado
            cout << respuesta << endl;  // Mostrar el tablero y cualquier mensaje recibido

            if (respuesta.find("Fin del juego") != string::npos) {
                juego_terminado = true; // Marcar que el juego ha terminado
                break;
            }
            continue; // Saltar la entrada del cliente porque es turno del servidor
        }

        int columna;
        bool columna_llena = false;
        string mensaje_error = "";
        while (true) {
            if (columna_llena) {
                // Limpiar el mensaje de error anterior
                cout << "\033[F\33[2K\r"; // Mover hacia arriba y limpiar la línea
            }
            cout << "Introduce la columna (1-7): ";
            cin >> columna;

            if (!enviarMensaje(to_string(columna))) {
                return;
            }

            if (!recibirMensaje(respuesta)) {
                return;
            }

            if (respuesta.find("La columna") == string::npos) {
                break;
            }

            // Mostrar el mensaje de error
            mensaje_error = respuesta;
            cout << mensaje_error << endl;
            columna_llena = true;
        }

        limpiarPantalla();  // Limpiar la terminal antes de mostrar el tablero actualizado
        cout << respuesta << endl;  // Mostrar el tablero y cualquier mensaje recibido

        if (respuesta.find("Fin del juego") != string::npos) {
            juego_terminado = true; // Marcar que el juego ha terminado
            break;
        }

        // Esperar y recibir el tablero actualizado después del movimiento del servidor
        if (!recibirMensaje(respuesta)) {
            break;
        }
        limpiarPantalla();  // Limpiar la terminal antes de mostrar el tablero actualizado
        cout << respuesta << endl;  // Mostrar el tablero y cualquier mensaje recibido

        if (respuesta.find("Fin del juego") != string::npos) {
            juego_terminado = true; // Marcar que el juego ha terminado
            break;
        }
    }

    close(client_socket_);  // Cerrar el socket después de terminar el juego
}


void Cliente::mostrarTablero() {
    string respuesta;
    if (recibirMensaje(respuesta)) {
        cout << respuesta << endl;
    }
}
