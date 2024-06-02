#include "cliente.h"
#include "servidor.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <modo> [<dirección IP del servidor> <puerto>]" << endl;
        cerr << "Modos: " << endl;
        cerr << "  servidor - Iniciar el servidor" << endl;
        cerr << "  cliente <dirección IP del servidor> <puerto> - Iniciar el cliente" << endl;
        return 1;
    }

    string modo = argv[1];

    if (modo == "servidor") {
        Servidor servidor;
        if (!servidor.iniciar()) {
            cerr << "Error al iniciar el servidor" << endl;
            return -1;
        }
        servidor.esperarConexiones();
    } else if (modo == "cliente") {
        if (argc != 4) {
            cerr << "Uso: " << argv[0] << " cliente <dirección IP del servidor> <puerto>" << endl;
            return 1;
        }
        string server_ip = argv[2];
        int port = stoi(argv[3]);

        Cliente cliente(server_ip, port);
        cliente.jugar();
    } else {
        cerr << "Modo desconocido: " << modo << endl;
        cerr << "Modos válidos son: servidor, cliente" << endl;
        return 1;
    }

    return 0;
}
