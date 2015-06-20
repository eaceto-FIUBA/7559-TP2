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

class ServerSocket {

	private:


		int server_port;
		int server_socket;
		struct sockaddr_in server_address;

		static const int MAX_NUMBER_OF_PENDING_CONNECTIONS = 5;

	public:
		ServerSocket ( const unsigned int port );
		~ServerSocket ();

		void iniciarServicio ();
		int socketFD ();
		struct sockaddr_in address ();

		std::string hostname();



		int enviar ( const void* buffer,const unsigned int buffSize ) const;
		int recibir ( void* buffer,const unsigned int buffSize ) const;

		void terminarServidor () const;
};

#endif /* SERVERSOCKET_H_ */
