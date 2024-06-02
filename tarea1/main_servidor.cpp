#include "servidor.h" // Incluir la definición de la clase Servidor
#include <iostream>   // Para manejo de entrada y salida estándar
#include <string>     // Para manejo de strings

using namespace std;

int main(int argc, char *argv[]) {
    // Verificar que se haya proporcionado el puerto
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <puerto>" << endl;
        return 1; // Terminar el programa si los argumentos no son correctos
    }

    // Obtener el puerto del servidor de los argumentos de la línea de comandos
    int port = stoi(argv[1]);

    // Crear una instancia del servidor y configurar el servidor
    Servidor servidor(port);
    if (!servidor.iniciar()) {
        cerr << "Error al iniciar el servidor" << endl;
        return -1; // Terminar el programa si hay un error al iniciar el servidor
    }

    // Esperar y aceptar conexiones entrantes
    servidor.esperarConexiones();

    return 0; // Finalizar el programa
}
