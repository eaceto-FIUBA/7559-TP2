#include "ServerSocket.h"


ServerSocket :: ServerSocket ( const unsigned int port ) : Socket ( port ) {
	this->nuevoFdSocket = -1;
}

ServerSocket :: ~ServerSocket () {
}

void ServerSocket :: abrirConexion () {
	struct sockaddr_in cli_addr;

	// el servidor aceptara conexiones de cualquier cliente
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;

	int bindOk = bind ( this->fdSocket,
						(struct sockaddr *)&(this->serv_addr),
						sizeof(this->serv_addr) );
	if ( bindOk < 0 ) {
		std::string mensaje = std::string("Error en bind(): ") + std::string(strerror(errno));
		throw mensaje;
	}

	int listenOk = listen ( this->fdSocket,CONEXIONES_PENDIENTES );
	if ( listenOk < 0 ) {
		std::string mensaje = std::string("Error en listen(): ") + std::string(strerror(errno));
		throw mensaje;
	}

	int longCliente = sizeof ( cli_addr );

	this->nuevoFdSocket = accept ( this->fdSocket,
									(struct sockaddr *)&cli_addr,
									(socklen_t *)&longCliente );

	if ( this->nuevoFdSocket < 0 ) {
		std::string mensaje = std::string("Error en accept(): ") + std::string(strerror(errno));
		throw mensaje;
	}
}

int ServerSocket :: enviar ( const void* buffer,const unsigned int buffSize ) const {
	int cantBytes = write ( this->nuevoFdSocket,buffer,buffSize );
	return cantBytes;
}

int ServerSocket :: recibir ( void* buffer,const unsigned int buffSize ) const {
	int cantBytes = read ( this->nuevoFdSocket,buffer,buffSize );
	return cantBytes;
}

void ServerSocket :: cerrarConexion () const {
	close ( this->nuevoFdSocket );
	close ( this->fdSocket );
}
