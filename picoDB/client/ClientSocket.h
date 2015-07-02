#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

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

class ClientSocket {

public:

    ClientSocket();

    ~ClientSocket();

    void conectar(const std::string &host, unsigned int port);

    /**
     * Retorna true si el Socket es valido, false en caso contrario.
     */
    bool es_valido();

    void desconectar();

    void shutdown();

    int recibirMensaje(void *stream, unsigned int size);

    int enviar(const char *stream, unsigned int size);

    int socketFD();

private:
    int fd;     // file descriptor
    bool valido; // estado del Socket
    struct sockaddr_in servidor;

};

#endif /* CLIENTSOCKET_H_ */
