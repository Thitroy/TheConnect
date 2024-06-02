#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <string>
#include <netinet/in.h>

#define FILAS 6
#define COLUMNAS 7

class Servidor {
public:
    Servidor(int port);
    ~Servidor();

    bool iniciar();
    void esperarConexiones();
    
private:
    int server_fd_;
    int port_;
    struct sockaddr_in address_;

    void jugarPartida(int client_socket, std::string client_ip, int client_port);
    void recibirMovimiento(int client_socket, char tablero[FILAS][COLUMNAS], std::string client_ip, int client_port);
    void hacerMovimientoServidor(int client_socket, char tablero[FILAS][COLUMNAS]);
    void enviarTablero(int client_socket, char tablero[FILAS][COLUMNAS]);
    void enviarTableroConMensaje(int client_socket, char tablero[FILAS][COLUMNAS], const char* mensaje);
    bool verificarVictoria(char tablero[FILAS][COLUMNAS], char ficha);
    bool columnaLlena(char tablero[FILAS][COLUMNAS], int columna);

};

#endif // SERVIDOR_H
