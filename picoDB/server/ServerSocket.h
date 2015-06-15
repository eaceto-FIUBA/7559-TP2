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

#include "Socket.h"


class ServerSocket : public Socket {

	private:
		int nuevoFdSocket;
		static const int CONEXIONES_PENDIENTES = 20;

	public:
		ServerSocket ( const unsigned int port );
		~ServerSocket ();

		void abrirConexion ();

		int enviar ( const void* buffer,const unsigned int buffSize ) const;
		int recibir ( void* buffer,const unsigned int buffSize ) const;

		void cerrarConexion () const;
};

#endif /* SERVERSOCKET_H_ */
