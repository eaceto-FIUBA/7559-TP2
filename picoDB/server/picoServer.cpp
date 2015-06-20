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

#define MAX_CLIENTS          10
#define MAX_PENDING_CONNECTIONS 5
#define SERVER_PORT          5000

bool enviarBienvenidaAlCliente(int new_client_socket);

void asignarClienteEnPool(int *clients_socket, int new_socket);

void cerrarConexionConCliente(int *clients_socket_pool, int a_client_socket_fd, int i);

int main(int argc, char *argv[]) {
    ServerSocket server(SERVER_PORT, MAX_PENDING_CONNECTIONS);
    server.iniciarServicio();

    std::cout << "picoServer: running on " << server.hostname() << ":" << std::to_string(SERVER_PORT) << std::endl;

    // file descriptors
    fd_set readfds;
    int server_socket_fd = server.socketFD();

    // clientes
    int clients_socket_pool[MAX_CLIENTS];
    int a_client_socket_fd, max_sd;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients_socket_pool[i] = 0;
    }

    bool server_running = true;

    while (server_running) {

        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(server_socket_fd, &readfds);
        max_sd = server_socket_fd;

        //add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            //socket descriptor
            a_client_socket_fd = clients_socket_pool[i];

            //if valid socket descriptor then add to read list
            if (a_client_socket_fd > 0)
                FD_SET(a_client_socket_fd, &readfds);

            //highest file descriptor number, need it for the select function
            if (a_client_socket_fd > max_sd)
                max_sd = a_client_socket_fd;
        }

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // Si hay actividad en el socket del servidor, entonces debe ser una conexión
        if (FD_ISSET(server_socket_fd, &readfds)) {
            int new_client_socket = server.aceptarNuevoCliente();

            enviarBienvenidaAlCliente(new_client_socket);

            //add new socket to array of sockets
            asignarClienteEnPool(clients_socket_pool, new_client_socket);
        }

        // verificamos otra operación de IO en otros sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            a_client_socket_fd = clients_socket_pool[i];

            int read_count;
            char read_buffer[1024];

            // solo actua si hay actividad en el socket
            if (FD_ISSET(a_client_socket_fd, &readfds)) {
                // Verificamos si lo que sucedió fue un evento de cierre de conexión (read_count = 0)
                if ((read_count = read(a_client_socket_fd, read_buffer, 1024)) == 0) {
                    cerrarConexionConCliente(clients_socket_pool, a_client_socket_fd, i);
                }
                // Procesar el mensaje entrante
                else {
                    // echo test
                    read_buffer[read_count] = '\0';
                    send(a_client_socket_fd, read_buffer, strlen(read_buffer), 0);
                }
            }
        }
    }

    server.terminarServidor();

    return 0;
}

void cerrarConexionConCliente(int *clients_socket_pool, int a_client_socket_fd, int i) {
    // log de desconexión
    sockaddr_in addr;
    int addrlen = sizeof(addr);
    getpeername(a_client_socket_fd, (struct sockaddr *) &addr, (socklen_t *) &addrlen);
    std::cout << "connection closed " << inet_ntoa(addr.sin_addr) << ":" << std::to_string(ntohs(addr.sin_port)) <<
    std::endl;

    // desconexón
    close(a_client_socket_fd);
    clients_socket_pool[i] = 0;
}

void asignarClienteEnPool(int *clients_socket, int new_socket) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        //if position is empty
        if (clients_socket[i] == 0) {
            clients_socket[i] = new_socket;
            break;
        }
    }
}

// enviar el prompt al cliente como simbolo de welcome
bool enviarBienvenidaAlCliente(int new_client_socket) {
    std::string welcome(">");

    if (send(new_client_socket, welcome.c_str(), welcome.length(), 0) != welcome.length()) {
        return false;
    }
    return true;
}