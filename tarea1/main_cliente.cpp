#include "cliente.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <dirección IP del servidor> <puerto>" << endl;
        return 1;
    }

    string server_ip = argv[1];
    int port = stoi(argv[2]);

    Cliente cliente(server_ip, port);
    cliente.jugar();

    return 0;
}
