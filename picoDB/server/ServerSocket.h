#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string>

struct ClientConnection {
    int socket_fd;
    sockaddr_in addr;
};

class ServerSocket {

private:

    int server_port;
    int server_socket;
    struct sockaddr_in server_address;

    int max_number_of_pending_connections;

public:
    ServerSocket(const unsigned int port, const unsigned int max_pending);

    ~ServerSocket();

    void iniciarServicio();

    int socketFD();

    struct sockaddr_in address();

    ClientConnection aceptarNuevoCliente();

    std::string hostname();

    int enviar(ClientConnection conn, const void *buffer, const unsigned int buffSize) const;

    int recibir(ClientConnection conn, void *buffer, const unsigned int buffSize) const;

    void terminarServidor() const;
};

#endif /* SERVERSOCKET_H_ */
