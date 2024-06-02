#include "servidor.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <puerto>" << endl;
        return 1;
    }

    int port = stoi(argv[1]);
    Servidor servidor(port);
    if (!servidor.iniciar()) {
        cerr << "Error al iniciar el servidor" << endl;
        return -1;
    }
    servidor.esperarConexiones();

    return 0;
}
