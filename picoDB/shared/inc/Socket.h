#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>

class Socket {

	protected:
		int fdSocket;
		struct sockaddr_in serv_addr;

	public:
		Socket ( const unsigned int port );
		virtual ~Socket ();

		virtual sockaddr_in abrirConexion () = 0;

		virtual int enviar ( const void* buffer,const unsigned int buffSize ) const = 0;
		virtual int recibir ( void* buffer,const unsigned int buffSize ) const = 0;

		virtual void cerrarConexion () const = 0;
};

#endif /* SOCKET_H_ */
