#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "ClientSocket.h"
#include "../shared/src/SocketException.h"
#include "../shared/inc/SignalHandler.h"
#include "../shared/inc/SIGINT_Handler.h"


using namespace std;

static const unsigned int SIZE_RECIBIR = 10;
static const char* goodbye = "Goodbye!";

int enviarMensaje(ClientSocket &cliente, std::string message) {

    int cantBytes = send(cliente.getFD(), message.c_str(), message.length(), 0);
    return cantBytes;
}

int recibirMensaje(ClientSocket &cliente, void *buffer, const unsigned int buffSize) {
    int cantBytes = read ( cliente.getFD(),buffer,buffSize );
    return cantBytes;
}

int main(int argc, char *argv[]) {

    std::cout << "picoDB Client" << std::endl;

    ClientSocket cliente;
    try{
    	cliente.conectar("127.0.0.1", 5000);
    }catch(SocketException &e){
    		std::cout << e.getMensaje() << std::endl;
    }

    char welcome_msj[1024];
    int read_count = recibirMensaje(cliente, welcome_msj, 1024 );
    welcome_msj[read_count] = '\0';
    std::cout << welcome_msj;

    // Registramos signal handler de SIGINT para manejar interrupcion del Server
    SIGINT_Handler sigint_handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    std::string mensaje;
    while (sigint_handler.getGracefulQuit() == 0) {

    	// leeo lina del prompt
    	std::getline (std::cin, mensaje);

    	// envío consulta al servidor
    	if( enviarMensaje(cliente, mensaje) != mensaje.length()){
    		std::cout << "Error: no se envió correctamente el mensaje" << std::endl;
    	}

    	// obtengo respuesta del server
        char read_buffer[1024];
        int read_count = recibirMensaje(cliente,  static_cast<void*>(read_buffer), sizeof(read_buffer)*sizeof(char) );

        if(read_count != -1){
        	read_buffer[read_count] = '\0';
			std::cout << read_buffer;
			if(strncmp(read_buffer,goodbye,sizeof(goodbye)*sizeof(char))==0){
				break;
			}
        }

    }

    cliente.desconectar();
    std::cout << "picoDB Client disconnected" << std::endl;
    return 0;
}
