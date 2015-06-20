#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include "ServerSocket.h"

#include "../shared/inc/SignalHandler.h"
#include "../shared/inc/SIGINT_Handler.h"

#define MAX_CLIENTS          10
#define MAX_PENDING_CONNECTIONS 5
#define SERVER_PORT          5000

/** Bio
 * http://www.lowtek.com/sockets/select.html
 * http://www.jasonernst.com/2011/03/22/tutorial-sockets-3-ways-to-listen/
 */

bool enviarMensajeAlCliente(ClientConnection client, std::string message);
bool enviarPromptAlCliente(ClientConnection client);
bool enviarAdiosAlCliente(ClientConnection client);
bool enviarBienvenidaAlCliente(ClientConnection client);

bool esMensajeDeAdios(std::string message);

void asignarClienteEnPool(ClientConnection *clients_pool, ClientConnection client);
void cerrarConexionConCliente(ClientConnection *clients_pool, ClientConnection client, int i);
void inicializarPoolDeClientes(ClientConnection *clients_pool);

void loggearMensajeRecibido(ClientConnection client, std::string message);

int main(int argc, char *argv[]) {
    ServerSocket server(SERVER_PORT, MAX_PENDING_CONNECTIONS);
    server.iniciarServicio();

    std::cout << "picoServer: running on " << server.hostname() << ":" << std::to_string(SERVER_PORT) << std::endl;

    // file descriptors
    fd_set readfds;
    int server_socket_fd = server.socketFD();

    // clientes
    ClientConnection clients_pool[MAX_CLIENTS];
    //ClientConnection a_client_connection;
    int max_sd;

    // inicializar el pool de clientes
    inicializarPoolDeClientes(clients_pool);

    // Registramos signal handler de SIGINT para manejar interrupcion del Server
    SIGINT_Handler sigint_handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    while (sigint_handler.getGracefulQuit() == 0) {

        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(server_socket_fd, &readfds);
        max_sd = server_socket_fd;

        //add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            //socket descriptor
            ClientConnection a_client_connection = clients_pool[i];

            //if valid socket descriptor then add to read list
            if (a_client_connection.socket_fd > 0)
                FD_SET(a_client_connection.socket_fd, &readfds);

            //highest file descriptor number, need it for the select function
            if (a_client_connection.socket_fd > max_sd)
                max_sd = a_client_connection.socket_fd;
        }

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // Si hay actividad en el socket del servidor, entonces debe ser una conexión
        if (FD_ISSET(server_socket_fd, &readfds)) {
            ClientConnection new_client = server.aceptarNuevoCliente();
            if (new_client.socket_fd != -1) {
                enviarBienvenidaAlCliente(new_client);
                enviarPromptAlCliente(new_client);

                //add new socket to array of sockets
                asignarClienteEnPool(clients_pool, new_client);
            }
        }

        // verificamos otra operación de IO en otros sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            ClientConnection a_client_connection = clients_pool[i];

            // solo actua si hay actividad en el socket
            if (FD_ISSET(a_client_connection.socket_fd, &readfds)) {
                char read_buffer[1024];

                // leemos del buffer
                int read_count = read(a_client_connection.socket_fd, read_buffer, 1024);

                // Verificamos si lo que sucedió fue un evento de cierre de conexión (read_count = 0)
                if (read_count == 0) {
                    cerrarConexionConCliente(clients_pool, a_client_connection, i);
                }
                // Procesar el mensaje entrante
                else {
                    // echo test
                    read_buffer[read_count] = '\0';
                    std::string rx_cmd(read_buffer, read_count);

                    loggearMensajeRecibido(a_client_connection,rx_cmd);

                    if (esMensajeDeAdios(rx_cmd)) {
                        cerrarConexionConCliente(clients_pool, a_client_connection, i);
                        continue;
                    }

                    enviarPromptAlCliente(a_client_connection);

                    //send(a_client_socket_fd, read_buffer, strlen(read_buffer), 0);
                }
            }
        }
    }

    std::cout << "exiting server..." << std::endl;

    server.terminarServidor();

    return 0;
}

void inicializarPoolDeClientes(ClientConnection *clients_pool) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients_pool[i].socket_fd = 0;
    }
}

void cerrarConexionConCliente(ClientConnection *clients_pool, ClientConnection client, int i) {

    enviarAdiosAlCliente(client);

    // log de desconexión
    sockaddr_in addr = client.addr;
    int addrlen = sizeof(addr);
//    getpeername(client.socket_fd, (struct sockaddr *) &addr, (socklen_t *) &addrlen);
    std::cout << "connection closed\t" << inet_ntoa(addr.sin_addr) << ":" << std::to_string(ntohs(addr.sin_port)) <<
    std::endl;

    // desconexón
    close(client.socket_fd);
    clients_pool[i].socket_fd = 0;
}

void asignarClienteEnPool(ClientConnection *clients_pool, ClientConnection client) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        // Si no hay un cliente en la posición del pool
        if (clients_pool[i].socket_fd == 0) {
            clients_pool[i].socket_fd = client.socket_fd;
            clients_pool[i].addr = client.addr;
            break;
        }
    }
}

// enviar el prompt al cliente como simbolo de welcome
bool enviarPromptAlCliente(ClientConnection client) {
    return enviarMensajeAlCliente(client,"picoServer>");
}


// enviar bienvenida
bool enviarBienvenidaAlCliente(ClientConnection client) {
    return enviarMensajeAlCliente(client,"Welcome!\n");
}

bool enviarAdiosAlCliente(ClientConnection client) {
    return enviarMensajeAlCliente(client,"Goodbye!\n");
}

// enviar el prompt al cliente como simbolo de welcome
bool enviarMensajeAlCliente(ClientConnection client, std::string message) {
    if (send(client.socket_fd, message.c_str(), message.length(), 0) != message.length()) {
        return false;
    }
    return true;
}

void loggearMensajeRecibido(ClientConnection client, std::string message) {
    if (message.length() > 0) {
        std::cout << "new message from\t" << inet_ntoa(client.addr.sin_addr) << ":" <<
                                                                               std::to_string(ntohs(client.addr.sin_port)) << "\t\t"
        << "("<< std::to_string(message.length()) <<")\t"
        << message << std::endl;
    }
}

bool esMensajeDeAdios(std::string message) {
    std::string prefix = "exit();";
    if (std::mismatch(prefix.begin(), prefix.end(), message.begin()).first == prefix.end()) {
        return true;
    }
    return false;
}