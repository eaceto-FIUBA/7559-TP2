#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <getopt.h>
#include <iterator>
#include <algorithm>

#include "ClientSocket.h"
#include "../shared/src/SocketException.h"
#include "../shared/inc/SignalHandler.h"
#include "../shared/inc/SIGINT_Handler.h"

#define kCLINoAargument         0
#define kCLIRequiredArgument    1
#define kCLIOptionalArgument    2

using namespace std;

#define DEFAULT_SERVER_PORT          5000
int     serverPort = DEFAULT_SERVER_PORT;
std::string serverHost;

static const char *goodbye = "Goodbye!";

int enviarMensaje(ClientSocket &cliente, std::string message) {

    int cantBytes = send(cliente.socketFD(), message.c_str(), message.length(), 0);
    return cantBytes;
}

int recibirMensaje(ClientSocket &cliente, void *buffer, const unsigned int buffSize) {
    int cantBytes = read(cliente.socketFD(), buffer, buffSize);
    return cantBytes;
}

bool is_prompt(std::string read_buffer) {
    if (read_buffer.find("picoServer>") != std::string::npos) {
        return true;
    } else {
        return false;
    }
}

void printVersion() {
    cout << "picoDB Client v1.0" << endl;
    cout << "[87761]\tOpromolla, Giovanni\t\tgiopromolla@gmail.com " << endl;
    cout << "[84316]\tAceto, Ezequiel Leonardo\t\tezequiel.aceto@gmail.com  " << endl;
}

void printHelp() {
    cout << endl;
    cout << "Usage: picoClient --host <host> --port <port>"  << endl;
    cout << "Usage: picoClient --t <host> -p <port>"  << endl;
    cout << endl;
    cout << "\t <port> is optional. Port 5000 is default port."  << endl;
    cout << "picoClient --help\t\t\tdisplay help" <<endl;
    cout << "picoClient --version\t\t\tdisplay version" <<endl;
    cout << endl;
}


int setupCLI(int argc, char **argv) {
    const struct option longopts[] =
            {
                    {"version",         kCLINoAargument,        NULL, 'v'},
                    {"help",            kCLINoAargument,        NULL, 'h'},
                    {"port",            kCLIOptionalArgument,  NULL, 'p'},
                    {"host",            kCLIRequiredArgument,  NULL, 't'},
                    {0,0,0,0},
            };

    int index;
    int iarg=0;

    //turn off getopt error message
    opterr=1;

    serverHost = string("127.0.0.1");

    while(iarg != -1)
    {
        iarg = getopt_long(argc, argv, "vhp:t:", longopts, &index);

        switch (iarg) {
            case 'h':
                printHelp();
                return -1;

            case 'v':
                printVersion();
                return -1;

            case 'p':
                if (optarg) {
                    serverPort = atol(optarg);
                }
                if (serverPort < 0) {
                    printHelp();
                    return -1;
                }
                break;

            case 't':
                if (optarg) {
                    serverHost = string(optarg);
                }
                break;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {

    if (setupCLI(argc, argv)) {
        return ( 0 );
    }

    std::cout << "picoDB Client v1.0" << std::endl;

    ClientSocket cliente;

    int max_sd;

    try {
        cliente.conectar(serverHost, serverPort);
    } catch (SocketException &e) {
        std::cout << "Exception " << e.getMensaje() << std::endl;
        return (0);
    }

    // file descriptors
    fd_set readfds;
    int client_socket_fd = cliente.socketFD();

    // Registramos signal handler de SIGINT para manejar interrupcion del Server
    SIGINT_Handler sigint_handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    std::string mensaje;
    while (sigint_handler.getGracefulQuit() == 0) {

        //clear the socket set
        FD_ZERO(&readfds);

        FD_SET(client_socket_fd, &readfds);
        max_sd = client_socket_fd;

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        // Si hay actividad en el socket del cliente, entonces debe ser un mensaje del servidor
        if (FD_ISSET(client_socket_fd, &readfds)) {

            char read_buffer[1024];

            int read_count = recibirMensaje(cliente, read_buffer, 1024);
            read_buffer[read_count] = '\0';

            std::string respuesta(read_buffer);
            std::cout << respuesta;

            if (is_prompt(respuesta)) {

                // leeo lina del prompt
                std::getline(std::cin, mensaje);

                // envío consulta al servidor
                if (enviarMensaje(cliente, mensaje) != mensaje.length()) {
                    std::cout << "Error: no se envió correctamente el mensaje" << std::endl;
                }

            } else if (strncmp(read_buffer, goodbye, sizeof(goodbye) * sizeof(char)) == 0) {
                break;
            }
        }

    }

    cliente.desconectar();
    std::cout << "picoDB Client disconnected" << std::endl;
    return 0;
}
