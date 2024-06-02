#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>

using namespace std;

class Cliente {
public:
    Cliente(const string& server_ip, int port);
    ~Cliente(); // Destructor

    bool conectar();
    bool enviarMensaje(const string& mensaje);
    bool recibirMensaje(string& respuesta);
    void jugar();

private:
    string server_ip_;
    int port_;
    int client_socket_;
};

#endif // CLIENTE_H
