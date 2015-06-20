#include "ServerSocket.h"
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <iostream>

ServerSocket :: ServerSocket ( const unsigned int port ) {
	this->server_port = port;
}

ServerSocket :: ~ServerSocket () {
}

std::string ServerSocket :: hostname() {
	char hostname[1024] = {0};
	//memset(&hostname[0], 0, sizeof(hostname));
	gethostname(hostname, 1024);
	std::string nombre(hostname);
	return nombre;
}

int ServerSocket :: socketFD() {
	return this->server_socket;
}

struct sockaddr_in ServerSocket :: address () {
	return this->server_address;
}

void ServerSocket :: iniciarServicio () {
	struct sockaddr_in cli_addr;

	// creamos el socket
	if( (server_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		std::string mensaje = std::string("Error on socket()") + std::string(strerror(errno));
		throw mensaje;
	}

	// setear server para aceptar multiples conexiones
	int opt = 1;
	if( setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
	{
		std::string mensaje = std::string("Error on setsockopt()") + std::string(strerror(errno));
		throw mensaje;
	}

	// inicializamos el socket del server
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons( server_port );

	// realizamos el bind al puerto del servidor
	if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))<0)
	{
		std::string mensaje = std::string("Error on bind()") + std::string(strerror(errno));
		throw mensaje;
	}

	// realizamos listen especificando un maximo de conexiones pendientes
	if (listen(server_socket, MAX_NUMBER_OF_PENDING_CONNECTIONS) < 0) {
		std::string mensaje = std::string("Error on listen()") + std::string(strerror(errno));
		throw mensaje;
	}
}


int ServerSocket :: enviar ( const void* buffer,const unsigned int buffSize ) const {
	/*
	int cantBytes = write ( this->nuevoFdSocket,buffer,buffSize );
	return cantBytes;
	*/
	return 0;
}

int ServerSocket :: recibir ( void* buffer,const unsigned int buffSize ) const {
	/*
	int cantBytes = read ( this->nuevoFdSocket,buffer,buffSize );
	return cantBytes;
	*/
	return 0;
}

void ServerSocket :: terminarServidor () const {
	close ( this->server_socket);
	//close ( this->fdSocket );
}
