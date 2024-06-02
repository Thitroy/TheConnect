#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>

class Cliente {
public:
    Cliente(const std::string& server_ip, int port);
    ~Cliente();

    bool conectar();
    bool enviarMensaje(const std::string& mensaje);
    bool recibirMensaje(std::string& respuesta);
    void jugar();

private:
    std::string server_ip_;
    int port_;
    int client_socket_;

    void limpiarPantalla(); // Declarar la función limpiarPantalla
    void mostrarTablero();
};

#endif // CLIENTE_H
