# CDR | Tarea 1 - Juego de Conecta 4 en Red

En el siguiente repositorio se desarrolló la tarea 1 del ramo de Comunicación de Datos y Redes de ICINF UBB; una versión del Connect4, utilizando el protocolo orientado a la conexión TCP para soportar multiples conexiones de Cliente - Servidor.

# 1.- Clonar el Repositorio

Para clonar este repositorio, crea una carpeta específica para depositar el proyecto, luego abre la ubicación de la carpeta con la terminal, y ejecuta el siguiente comando:

```bash
git clone https://github.com/Thitroy/TheConnect/
cd Tarea1
```

# 2.- Compilar el Proyecto

Para compilar tanto el servidor como el cliente, usa el Makefile incluido en la carpeta Tarea1. Esto generará dos ejecutables: servidor y cliente.

```bash
cd Tarea1
make
```

# 3.- Iniciar el Servidor

Para iniciar el servidor, ejecuta el siguiente comando, especificando el puerto en el que deseas que escuche:

```bash
./servidor <puerto>
# Ejemplo:
./servidor 12345
```

# 4.- Iniciar el Cliente

Para iniciar el cliente, ejecuta el siguiente comando, proporcionando la dirección IP del servidor y el puerto en el que está ejecutándose:

**Nota: Como requisito debe conocer el ip del servidor.**

```bash
./cliente <IP_del_servidor> <puerto>
# Ejemplo:
./cliente 192.168.1.100 12345
```

# 5.- Instrucciones para Jugar

### Inicio del Juego:

- Se usa la letra C para las fichas del cliente, y la S para las fichas del servidor.
- El servidor y el cliente seleccionarán aleatoriamente quién comienza.
- De iniciar el cliente, le solicitará ingresar su columna deseada. De lo contrario, el servidor depositará primeramente una ficha.
- De un tiempo de 2 a 3 segundos entre cada ingreso de ficha, para así evitar posibles errores de pérdida de memoria entre tablero y tablero.

### Realizar Movimientos:

- El cliente introduce la columna (1-7) en la que desea colocar su ficha.
- El servidor realiza movimientos automáticamente.

### Condiciones de Victoria:

- El juego verifica después de cada movimiento si alguno de los jugadores ha ganado formando una línea de 4 fichas consecutivas de forma horizontal, vertical o diagonalmente.
- Si el tablero se llena sin que ningún jugador forme una línea de 4 fichas, el juego termina en empate.

### Errores:

- Si un jugador intenta colocar una ficha en una columna llena, se le pedirá que elija otra columna.
- El juego continúa hasta que se declare un ganador o un empate.

### Reiniciar el Juego:

- Una vez terminado el juego, se preguntará al cliente si desea jugar otra partida.

# 6.- Limpiar los Archivos Compilados

Para limpiar los archivos objeto y los ejecutables generados, utiliza el siguiente comando:

```bash
make clean
```

Integrantes:
- Fernanda Morales
- Abel Rosales
