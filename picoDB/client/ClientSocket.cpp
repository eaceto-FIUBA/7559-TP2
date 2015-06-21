
#include "ClientSocket.h"
#include "../shared/src/SocketException.h"

ClientSocket::ClientSocket() {

	fd = 0;
	valido = false;

}

ClientSocket::~ClientSocket() {

}

bool ClientSocket::es_valido(){

	return this->valido;
}

int ClientSocket::getFD(){
	return this->fd;
}

void ClientSocket::conectar(const std::string& host, unsigned int port) {

	if(!es_valido()){
		fd=::socket(AF_INET,SOCK_STREAM,0);

		struct hostent *he;

		if (!(he= gethostbyname(host.c_str()))) {
			valido =false;
			::close(fd);
			throw SocketException("Error Socket- connect() - gethostbyname().");
		}

		servidor.sin_family = AF_INET;
		//pone la direccion
		servidor.sin_addr = *((struct in_addr *)he->h_addr);
		//pone el puerto
		servidor.sin_port = htons(port);
		//completa con 0
		bzero(&(servidor.sin_zero),8);

		//se conecta y recibe el error
		int error =::connect (this->fd, (struct sockaddr *)&servidor,sizeof(struct sockaddr));
		//si la variable error en -1 lanzo una excepcion
		if( error == -1){
			valido = false;
			std::string strError = "Error Socket -connect(): ";
			strError += strerror(errno);
			throw SocketException(strError.c_str());
		}
	}
	else{
		valido = false;
		throw SocketException("Error Socket -connect(): Socket invalido");
	}

}

void ClientSocket::desconectar(){

	if(es_valido()){

		::close(this->fd);
		this->fd = -1;
		this->valido=false;
	}

}

void ClientSocket::shutdown(){

	if(es_valido()){

		int error=::shutdown(fd,SHUT_RDWR);
		//si la variable error es -1 lanzo una excepcion
		if(error==-1){
			throw SocketException("Error Socket -cancelar_socket() -valor recibido(-1)");
		}
	}
	else{

		throw SocketException("Error Socket -cancelar_socket(): Socket invalido");
	}
}
