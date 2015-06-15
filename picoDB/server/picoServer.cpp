#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "../shared/inc/person.h"
#include "ServerSocket.h"

#define MAX_REQUEST_BUFFSIZE 1024
#define SERVER_PORT          5000

using namespace std;

int main() {
    std::cout << "running picoDB Server" << endl;

    bool serverRunning = true;

    try {
        ServerSocket socket ( SERVER_PORT );

        while (serverRunning == true) {
            std::cout << "picoDB Server: waiting for clients" << std::endl;
            std::cout << "picoDB Server: close connection sending 'exit();'" << std::endl << std::endl;

            socket.abrirConexion();

            pid_t childPid = fork();

            if (childPid == 0) {

                std::string request;

                char buffer[MAX_REQUEST_BUFFSIZE];

                do {
                    int bytesRecibidos = socket.recibir ( static_cast<void*>(buffer),MAX_REQUEST_BUFFSIZE );
                    request = buffer;
                    request.resize(bytesRecibidos);
                    std::cout << "picoDB Server: Request received: " << request << std::endl;

                    std::cout << "picoDB Server: sending Response . . ." << std::endl;
                    socket.enviar ( static_cast<const void*>(request.c_str()),request.size() );

                } while ( request != std::string("exit();") );

                std::cout << "picoDB Server: closing connection" << std::endl;
                socket.cerrarConexion ();

                // remove resources

                // kill process
                exit(0);
            }
            else {
                socket.cerrarConexion ();
            }
        }
    } catch (std::string& error) {
        std::cerr << "Exception: " << error << std::endl;
    }

    exit(0);
}