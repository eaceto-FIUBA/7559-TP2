#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "../shared/inc/person.h"
#include "ServerSocket.h"
#include <unistd.h>

#define MAX_REQUEST_BUFFSIZE    1024
#define SERVER_PORT             5000
#define SERVER_MAX_WORKERS      4
#define SERVER_MAX_PENDING_CONNECTIONS  10

using namespace std;

int main() {
    std::cout << "Running picoDB Server" << endl;

    string hostname;

    {
        char name[256];
        int r = gethostname(name, 256);
        hostname = std::string(name);
    }

    std::cout << "Hostname: " << hostname << endl << endl;


    bool serverRunning = true;

    try {
        ServerSocket socket ( SERVER_PORT, SERVER_MAX_PENDING_CONNECTIONS );

        while (serverRunning == true) {  // TODO implement signal handler for quit

            std::cout << "picoDB Server: waiting for clients" << std::endl;
            std::cout << "picoDB Server: close connection sending 'exit();'" << std::endl << std::endl;

            struct sockaddr_in their_addr  = socket.abrirConexion();

            pid_t childPid = fork();

            std::cout << "picoDB Server: got a connection from "<< inet_ntoa(their_addr.sin_addr)  <<":" << htons(their_addr.sin_port) << endl;

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

                std::cout << "picoDB Server: closing connection from "<< inet_ntoa(their_addr.sin_addr)  <<":" << htons(their_addr.sin_port) << endl;
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