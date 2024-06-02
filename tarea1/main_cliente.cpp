#include "cliente.h" // Incluir la definición de la clase Cliente
#include <iostream>  // Para manejo de entrada y salida estándar
#include <string>    // Para manejo de strings

using namespace std;

int main(int argc, char *argv[]) {
    // Verificar que se hayan proporcionado la dirección IP del servidor y el puerto
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <dirección IP del servidor> <puerto>" << endl;
        return 1; // Terminar el programa si los argumentos no son correctos
    }

    // Obtener la dirección IP y el puerto del servidor de los argumentos de la línea de comandos
    string server_ip = argv[1];
    int port = stoi(argv[2]);

    // Crear una instancia del cliente y conectar al servidor
    Cliente cliente(server_ip, port);
    cliente.jugar(); // Iniciar el flujo de juego del cliente

    return 0; // Finalizar el programa
}