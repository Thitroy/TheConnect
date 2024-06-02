#ifndef CLIENTE_H // Verifica si no se ha definido anteriormente
#define CLIENTE_H // Define la clase Cliente

#include <string> // Para el manejo de strings

class Cliente {
public:
    // Constructor y destructor
    Cliente(const std::string& server_ip, int port);
    ~Cliente();

    // Función para conectar con el servidor
    bool conectar();
    // Función para enviar un mensaje al servidor
    bool enviarMensaje(const std::string& mensaje);
    // Función para recibir un mensaje del servidor
    bool recibirMensaje(std::string& respuesta);
    // Función principal para manejar el flujo de juego del cliente
    void jugar();

private:
    // Dirección IP del servidor
    std::string server_ip_;
    // Puerto del servidor
    int port_;
    // Descriptor del socket del cliente
    int client_socket_;

    // Función para limpiar la pantalla del terminal
    void limpiarPantalla();
    // Función para mostrar el tablero actual
    void mostrarTablero();
};

#endif // CLIENTE_H

