#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <string>
#include <netinet/in.h>

#define PORT 7777
#define FILAS 6
#define COLUMNAS 7

class Servidor {
public:
    Servidor();
    ~Servidor();

    bool iniciar();
    void esperarConexiones();
    
private:
    int server_fd_;
    struct sockaddr_in address_;

    void jugarPartida(int client_socket);
    void recibirMovimiento(int client_socket, char tablero[FILAS][COLUMNAS]);
    void enviarTablero(int client_socket, char tablero[FILAS][COLUMNAS]);
    bool verificarVictoria(char tablero[FILAS][COLUMNAS], char ficha);
};

#endif // SERVIDOR_H