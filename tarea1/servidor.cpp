#include "servidor.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <cstdlib>
#include <ctime>

using namespace std;

Servidor::Servidor(int port) : server_fd_(-1), port_(port) {
    memset(&address_, 0, sizeof(address_));
    srand(time(0)); // Inicializar la semilla para la selección aleatoria
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
    address_.sin_port = htons(port_);

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
    cout << "Servidor iniciado. Esperando conexiones entrantes..." << endl;

    while (true) {
        int new_socket;
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        if ((new_socket = accept(server_fd_, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            cerr << "Error al aceptar la conexión" << endl;
            continue;
        }

        string client_ip = inet_ntoa(client_addr.sin_addr);
        int client_port = ntohs(client_addr.sin_port);
        cout << "Juego nuevo[" << client_ip << ":" << client_port << "]" << endl;

        thread(&Servidor::jugarPartida, this, new_socket, client_ip, client_port).detach();
    }
}

void Servidor::jugarPartida(int client_socket, string client_ip, int client_port) {
    // Reiniciar el tablero y las variables de estado al comenzar una nueva partida
    char tablero[FILAS][COLUMNAS] = { {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                      {' ', ' ', ' ', ' ', ' ', ' ', ' '} };

    bool turno_cliente = (rand() % 2 == 0); // Seleccionar aleatoriamente quién empieza

    if (turno_cliente) {
        const char* mensaje_inicio = "Inicia el cliente.\n";
        send(client_socket, mensaje_inicio, strlen(mensaje_inicio), 0);
        enviarTablero(client_socket, tablero); // Enviar el tablero vacío
    } else {
        const char* mensaje_inicio = "Inicia el servidor.\n";
        send(client_socket, mensaje_inicio, strlen(mensaje_inicio), 0);
        hacerMovimientoServidor(client_socket, tablero); // Movimiento inicial del servidor
        enviarTablero(client_socket, tablero); // Enviar el tablero después del primer movimiento del servidor
        turno_cliente = true; // Cambiar el turno al cliente después del primer movimiento del servidor
    }

    try {
        while (true) {
            if (turno_cliente) {
                cout << "Juego [" << client_ip << ":" << client_port << "]: turno del cliente." << endl;
                recibirMovimiento(client_socket, tablero, client_ip, client_port);
                if (verificarVictoria(tablero, 'C')) {
                    enviarTableroConMensaje(client_socket, tablero, "Gana el Cliente...\nFin del juego.\n");
                    close(client_socket);
                    throw std::runtime_error("Victoria del jugador");
                }
                enviarTablero(client_socket, tablero); // Enviar el tablero actualizado después del movimiento del cliente
                turno_cliente = false; // Pasar el turno al servidor
            } else {
                cout << "Juego [" << client_ip << ":" << client_port << "]: turno del servidor." << endl;
                hacerMovimientoServidor(client_socket, tablero);
                if (verificarVictoria(tablero, 'S')) {
                    enviarTableroConMensaje(client_socket, tablero, "Gana el Servidor...\nFin del juego.\n");
                    close(client_socket);
                    throw std::runtime_error("Victoria del servidor");
                }
                enviarTablero(client_socket, tablero); // Enviar el tablero actualizado después del movimiento del servidor
                turno_cliente = true; // Pasar el turno al cliente
            }

            // Comprobar empate
            bool empate = true;
            for (int col = 0; col < COLUMNAS; ++col) {
                if (tablero[0][col] == ' ') {
                    empate = false;
                    break;
                }
            }
            if (empate) {
                enviarTableroConMensaje(client_socket, tablero, "¡Empate! El tablero está lleno y nadie ha ganado.\nFin del juego.\n");
                close(client_socket);
                throw std::runtime_error("Empate en el juego");
            }
        }
    } catch (const std::runtime_error& e) {
        cerr << "Juego [" << client_ip << ":" << client_port << "] terminado: " << e.what() << endl;
        close(client_socket);
    }
}
bool Servidor::columnaLlena(char tablero[FILAS][COLUMNAS], int columna) {
    return tablero[0][columna] != ' ';
}

void Servidor::recibirMovimiento(int client_socket, char tablero[FILAS][COLUMNAS], string client_ip, int client_port) {
    if (client_socket < 0) {
        cerr << "Socket inválido al inicio de recibirMovimiento" << endl;
        throw std::runtime_error("Socket inválido");
    }

    char buffer[256];
    int columna = -1;

    while (true) {
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

        if (sscanf(buffer, "%d", &columna) != 1 || columna < 1 || columna > COLUMNAS) {
            const char* mensaje_error = "Movimiento inválido. Introduce una columna válida (1-7).\n";
            send(client_socket, mensaje_error, strlen(mensaje_error), 0);
            cerr << "Error: Coordenadas inválidas recibidas" << endl;
            continue;
        }

        columna--; // Ajustar a índice 0
        if (tablero[0][columna] != ' ') {
            string mensaje_error = "Juego [" + client_ip + ":" + to_string(client_port) + "]: La columna " + to_string(columna + 1) + " está llena. Introduce otra columna.\n";
            send(client_socket, mensaje_error.c_str(), mensaje_error.length(), 0);
            cerr << "Juego [" << client_ip << ":" << client_port << "]: Error: La columna " << columna + 1 << " está llena." << endl;
            continue;
        }
        break; // Salir del bucle cuando se recibe un movimiento válido
    }

    for (int fila = FILAS - 1; fila >= 0; --fila) {
        if (tablero[fila][columna] == ' ') {
            tablero[fila][columna] = 'C';
            break;
        }
    }

    cout << "Juego [" << client_ip << ":" << client_port << "]: cliente juega columna " << columna + 1 << "." << endl;
}

void Servidor::hacerMovimientoServidor(int client_socket, char tablero[FILAS][COLUMNAS]) {
    int columna;
    do {
        columna = rand() % COLUMNAS;
    } while (tablero[0][columna] != ' '); // Elegir una columna aleatoria que no esté llena

    for (int fila = FILAS - 1; fila >= 0; --fila) {
        if (tablero[fila][columna] == ' ') {
            tablero[fila][columna] = 'S'; // Suponemos que el servidor usa fichas 'S'
            break;
        }
    }

    cout << "Juego: servidor juega columna " << columna + 1 << "." << endl;
}

void Servidor::enviarTablero(int client_socket, char tablero[FILAS][COLUMNAS]) {
    string tablero_str = "TABLERO\n";
    for (int fila = 0; fila < FILAS; ++fila) {
        tablero_str += to_string(fila + 1) + " "; // Agregar el número de fila a la izquierda
        for (int col = 0; col < COLUMNAS; ++col) {
            tablero_str += tablero[fila][col];
            if (col < COLUMNAS - 1) {
                tablero_str += ' ';
            }
        }
        tablero_str += '\n';
    }
    tablero_str += "  -------------\n"; // Alineación del borde inferior
    tablero_str += "  1 2 3 4 5 6 7\n"; // Agregar un espacio adicional para alinear con los números de fila
    send(client_socket, tablero_str.c_str(), tablero_str.length(), 0);
}

void Servidor::enviarTableroConMensaje(int client_socket, char tablero[FILAS][COLUMNAS], const char* mensaje) {
    enviarTablero(client_socket, tablero); // Enviar el tablero primero
    send(client_socket, mensaje, strlen(mensaje), 0); // Enviar el mensaje de victoria o empate
}

bool Servidor::verificarVictoria(char tablero[FILAS][COLUMNAS], char ficha) {
    // Comprobar horizontalmente
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

    // Comprobar verticalmente
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

    // Comprobar diagonalmente (\)
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

    // Comprobar diagonalmente (/)
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
