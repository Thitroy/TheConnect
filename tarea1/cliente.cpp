#include "cliente.h" // Incluye la definición de la clase Cliente
#include <iostream> // Incluye la biblioteca de entrada y salida estándar
#include <cstring> // Incluye funciones para manipulación de cadenas de caracteres
#include <unistd.h> // Incluye funciones para operaciones con archivos y directorios (por ejemplo, close)
#include <sys/socket.h> // Incluye definiciones de la biblioteca de sockets
#include <netinet/in.h> // Incluye estructuras de datos para manejar direcciones de red
#include <arpa/inet.h> // Incluye funciones para manipular direcciones IP (por ejemplo, inet_pton)

using namespace std;

// Constructor
// Inicializa la IP del servidor, el puerto y el socket del cliente
Cliente::Cliente(const string& server_ip, int port)
    : server_ip_(server_ip), port_(port), client_socket_(-1) {
}

// Destructor
// Cierra el socket del cliente si está abierto
Cliente::~Cliente() {
    if (client_socket_ != -1) {
        close(client_socket_);
    }
}

// Función para conectar el cliente al servidor
bool Cliente::conectar() {
    // Crear el socket
    if ((client_socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error al crear el socket" << endl;
        return false;
    }

    // Configurar la dirección del servidor
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Inicializar la estructura con ceros
    serv_addr.sin_family = AF_INET; // Familia de direcciones (IPv4)
    serv_addr.sin_port = htons(port_); // Convertir el número de puerto a formato de red

    // Convertir la dirección IP del servidor de texto a binario
    if (inet_pton(AF_INET, server_ip_.c_str(), &serv_addr.sin_addr) <= 0) {
        cerr << "Dirección IP inválida" << endl;
        return false;
    }

    // Conectar al servidor
    if (connect(client_socket_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Error al conectar al servidor" << endl;
        return false;
    }

    return true; // Conexión exitosa
}

// Función para enviar un mensaje al servidor
bool Cliente::enviarMensaje(const string& mensaje) {
    cout << "Enviando mensaje: " << mensaje << endl;
    if (send(client_socket_, mensaje.c_str(), mensaje.length(), 0) < 0) {
        cerr << "Error al enviar mensaje al servidor" << endl;
        return false;
    }
    return true;
}

// Función para recibir un mensaje del servidor
bool Cliente::recibirMensaje(string& respuesta) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer)); // Limpiar el buffer antes de recibir datos
    int bytes_recibidos = recv(client_socket_, buffer, sizeof(buffer) - 1, 0);
    if (bytes_recibidos < 0) {
        cerr << "Error al recibir datos del servidor" << endl;
        return false;
    }
    buffer[bytes_recibidos] = '\0';
    respuesta = string(buffer);
    cout << "Mensaje recibido: " << respuesta << endl;
    return true;
}

// Función para limpiar la pantalla de la terminal
void Cliente::limpiarPantalla() {
    // Secuencias de escape ANSI para limpiar la terminal
    cout << "\033[2J\033[1;1H";
}

void Cliente::jugar() {
    while (true) {
        if (!conectar()) {
            return;
        }

        string respuesta;
        if (recibirMensaje(respuesta)) {
            cout << respuesta << endl;
        }

        bool juego_terminado = false; // Variable para verificar si el juego ha terminado

        while (!juego_terminado) {
            limpiarPantalla();  // Limpiar la terminal antes de mostrar el tablero
            cout << respuesta << endl;  // Mostrar el tablero y cualquier mensaje recibido

            if (respuesta.find("Fin del juego") != string::npos) {
                juego_terminado = true; // Marcar que el juego ha terminado
                break;
            }

            if (respuesta.find("Inicia el servidor") != string::npos) {
                if (!recibirMensaje(respuesta)) {
                    break;
                }
                limpiarPantalla();  // Limpiar la terminal antes de mostrar el tablero actualizado
                cout << respuesta << endl;  // Mostrar el tablero y cualquier mensaje recibido

                if (respuesta.find("Fin del juego") != string::npos) {
                    juego_terminado = true; // Marcar que el juego ha terminado
                    break;
                }
                continue; // Saltar la entrada del cliente porque es turno del servidor
            }

            int columna;
            bool columna_llena = false;
            string mensaje_error = "";
            while (true) {
                if (columna_llena) {
                    // Limpiar el mensaje de error anterior
                    cout << "\033[F\33[2K\r"; // Mover hacia arriba y limpiar la línea
                }
                cout << "Introduce la columna (1-7): ";
                cin >> columna;

                if (!enviarMensaje(to_string(columna))) {
                    return;
                }

                if (!recibirMensaje(respuesta)) {
                    return;
                }

                if (respuesta.find("La columna") == string::npos) {
                    break;
                }

                // Mostrar el mensaje de error
                mensaje_error = respuesta;
                cout << mensaje_error << endl;
                columna_llena = true;
            }

            limpiarPantalla();  // Limpiar la terminal antes de mostrar el tablero actualizado
            cout << respuesta << endl;  // Mostrar el tablero y cualquier mensaje recibido

            if (respuesta.find("Fin del juego") != string::npos) {
                juego_terminado = true; // Marcar que el juego ha terminado
                break;
            }

            // Esperar y recibir el tablero actualizado después del movimiento del servidor
            if (!recibirMensaje(respuesta)) {
                break;
            }
            limpiarPantalla();  // Limpiar la terminal antes de mostrar el tablero actualizado
            cout << respuesta << endl;  // Mostrar el tablero y cualquier mensaje recibido

            if (respuesta.find("Fin del juego") != string::npos) {
                juego_terminado = true; // Marcar que el juego ha terminado
                break;
            }
        }

        close(client_socket_);  // Cerrar el socket después de terminar el juego

        // Preguntar al usuario si desea jugar otra partida
        char jugar_otra;
        cout << "¿Desea jugar otra partida? (s/n): ";
        cin >> jugar_otra;
        if (jugar_otra != 's' && jugar_otra != 'S') {
            break;
        }
    }
}

// Función para mostrar el tablero recibido del servidor
void Cliente::mostrarTablero() {
    string respuesta;
    if (recibirMensaje(respuesta)) {
        cout << respuesta << endl;
    }
}