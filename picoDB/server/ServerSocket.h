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

#include "../shared/inc/Socket.h"


class ServerSocket : public Socket {

	private:
		int nuevoFdSocket;
		int CONEXIONES_PENDIENTES;

	public:
		ServerSocket ( const unsigned int port , const unsigned int conexiones_pendientes);
		~ServerSocket ();

		sockaddr_in abrirConexion ();

		int enviar ( const void* buffer,const unsigned int buffSize ) const;
		int recibir ( void* buffer,const unsigned int buffSize ) const;

		void cerrarConexion () const;
};

#endif /* SERVERSOCKET_H_ */
