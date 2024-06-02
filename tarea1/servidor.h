#ifndef SERVIDOR_H // Verifica si no se ha definido anteriormente
#define SERVIDOR_H // Define la clase Servidor

#include <string> // Para el manejo de strings
#include <netinet/in.h> // Manejo de direcciones de red y sockets en Unix

//Definimos las dimensiones para el tablero
#define FILAS 6
#define COLUMNAS 7

class Servidor {
public:
    // Constructor y destructor
    Servidor(int port);
    ~Servidor();

    // Función para iniciar el servidor
    bool iniciar();
    // Función que espera y acepta conexiones entrantes del servidor
    void esperarConexiones();
    
private:
    // Se almacena el socket del servidor
    int server_fd_;
    // Se almacena el puerto del servidor 
    int port_;
    // Estructura que contiene la dirección del servidor
    struct sockaddr_in address_;

    // Función que maneja una partida con el cliente conectado
    void jugarPartida(int client_socket, std::string client_ip, int client_port);
    // Función que recibe los movimientos del cliente y actualiza el tablero
    void recibirMovimiento(int client_socket, char tablero[FILAS][COLUMNAS], std::string client_ip, int client_port);
    // Función que ejecuta movimientos de parte del servidor y actualiza el tablero
    void hacerMovimientoServidor(int client_socket, char tablero[FILAS][COLUMNAS]);
    // Función que envía el tablero actualizado al cliente
    void enviarTablero(int client_socket, char tablero[FILAS][COLUMNAS]);
    // Función que envía el tablero actualizado al cliente con un mensaje adicional (Quién ganó)
    void enviarTableroConMensaje(int client_socket, char tablero[FILAS][COLUMNAS], const char* mensaje);
    // Función que verifica si el cliente o el servidor ganaron.
    bool verificarVictoria(char tablero[FILAS][COLUMNAS], char ficha);
    // Función que verifica si el tablero está lleno
    bool columnaLlena(char tablero[FILAS][COLUMNAS], int columna);

};

#endif // SERVIDOR_H
