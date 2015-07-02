#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <iterator>     // std::ostream_iterator

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include "ServerSocket.h"

#include "../shared/inc/SignalHandler.h"
#include "../shared/inc/SIGINT_Handler.h"
#include "../shared/inc/person.h"

#define MAX_CLIENTS          10
#define MAX_PENDING_CONNECTIONS 5
#define SERVER_PORT          5000

/** Bio
 * http://www.lowtek.com/sockets/select.html
 * http://www.jasonernst.com/2011/03/22/tutorial-sockets-3-ways-to-listen/
 */

bool enviarMensajeAlCliente(ClientConnection client, std::string message);
bool enviarPromptAlCliente(ClientConnection client);
bool enviarAdiosAlCliente(ClientConnection client);
bool enviarBienvenidaAlCliente(ClientConnection client);

bool esSelectSobreTabla(std::string command);
bool esInsertSobreTabla(std::string command);
bool comandoBienTerminado(std::string command);
bool esMensajeDeAdios(std::string message);
std::string obtenerTablaDeConsulta(std::string op,std::string consulta);
std::vector<std::string> obtenerArgumentosDeInsert(std::string consulta, std::string tabla);
void insertSobreTabla(ClientConnection client,Person &person,std::string tabla, std::vector<std::string> args);
void enviarSelectSobreTabla(ClientConnection &client, Person &person, std::string tabla);

void asignarClienteEnPool(ClientConnection *clients_pool, ClientConnection client);
void cerrarConexionConCliente(ClientConnection *clients_pool, ClientConnection client, int i);
void inicializarPoolDeClientes(ClientConnection *clients_pool);

void loggearMensajeRecibido(ClientConnection client, std::string message);


inline std::string trim(const std::string &s);
inline bool empieza_con(std::string const &command, std::string const & prefix);
inline bool termina_con(std::string const & value, std::string const & ending);

int main(int argc, char *argv[]) {
    ServerSocket server(SERVER_PORT, MAX_PENDING_CONNECTIONS);
    server.iniciarServicio();

    std::cout << "picoServer: running on " << server.hostname() << ":" << std::to_string(SERVER_PORT) << std::endl;

    // file descriptors
    fd_set readfds;
    int server_socket_fd = server.socketFD();

    // clientes
    ClientConnection clients_pool[MAX_CLIENTS];
    //ClientConnection a_client_connection;
    int max_sd;

    // inicializar el pool de clientes
    inicializarPoolDeClientes(clients_pool);

    // Registramos signal handler de SIGINT para manejar interrupcion del Server
    SIGINT_Handler sigint_handler;
    SignalHandler::getInstance()->registrarHandler(SIGINT, &sigint_handler);

    std::vector<std::string> tables;
    tables.push_back("person");

    while (sigint_handler.getGracefulQuit() == 0) {

        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(server_socket_fd, &readfds);
        max_sd = server_socket_fd;

        //add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            //socket descriptor
            ClientConnection a_client_connection = clients_pool[i];

            //if valid socket descriptor then add to read list
            if (a_client_connection.socket_fd > 0)
                FD_SET(a_client_connection.socket_fd, &readfds);

            //highest file descriptor number, need it for the select function
            if (a_client_connection.socket_fd > max_sd)
                max_sd = a_client_connection.socket_fd;
        }

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // Si hay actividad en el socket del servidor, entonces debe ser una conexión
        if (FD_ISSET(server_socket_fd, &readfds)) {
            ClientConnection new_client = server.aceptarNuevoCliente();

            if (new_client.socket_fd != -1) {
                enviarBienvenidaAlCliente(new_client);
                enviarPromptAlCliente(new_client);
                //add new socket to array of sockets
                asignarClienteEnPool(clients_pool, new_client);
            }
        }

        // verificamos otra operación de IO en otros sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            ClientConnection a_client_connection = clients_pool[i];

            // solo actua si hay actividad en el socket
            if (FD_ISSET(a_client_connection.socket_fd, &readfds)) {
                char read_buffer[1024];

                // leemos del buffer
                int read_count = read(a_client_connection.socket_fd, read_buffer, 1024);

                // Verificamos si lo que sucedió fue un evento de cierre de conexión (read_count = 0)
                if ((read_count == 0) || (read_count == -1)) {
                    cerrarConexionConCliente(clients_pool, a_client_connection, i);
                }
                // Procesar el mensaje entrante
                else {
                    // echo test
                    read_buffer[read_count] = '\0';
                    std::string rx_cmd(read_buffer, read_count);

                    // normalización
                    transform(rx_cmd.begin(), rx_cmd.end(), rx_cmd.begin(), ::tolower);
                    rx_cmd = trim(rx_cmd);

                    loggearMensajeRecibido(a_client_connection,rx_cmd);

                    if (comandoBienTerminado(rx_cmd) == false) {
                        enviarMensajeAlCliente(a_client_connection,std::string("Error: Query must end with ';'.\n"));
                    }
                    else if (esMensajeDeAdios(rx_cmd)) {
                        cerrarConexionConCliente(clients_pool, a_client_connection, i);
                        continue;
                    }
                    else if (esSelectSobreTabla(rx_cmd)) {
                        bool found = false;
                        std::string tabla = obtenerTablaDeConsulta("select * from",rx_cmd);

                        std::vector<std::string>::const_iterator it = tables.begin();
                        std::string::size_type s;

                        while(it != tables.end()) {
                            s = tabla.find(*it, 0);
                            if( s != std::string::npos ) {
                                Person person("person");
                                enviarSelectSobreTabla(a_client_connection,person,tabla);
                                found = true;
                                break;
                            }
                            ++i;
                        }

                        if (found == false) {
                            enviarMensajeAlCliente(a_client_connection, std::string("Error: Invalid table name: "+ tabla +"\n"));
                        }
                    }
                    else if (esInsertSobreTabla(rx_cmd)) {
                        bool found = false;
                        std::string tabla = obtenerTablaDeConsulta("insert into",rx_cmd);
                        std::vector<std::string> args = obtenerArgumentosDeInsert(rx_cmd,tabla);

                        std::vector<std::string>::const_iterator it = tables.begin();
                        std::string::size_type s;

                        while(it != tables.end()) {
                            s = tabla.find(*it, 0);
                            if( s != std::string::npos ) {
                                Person person("person");
                                insertSobreTabla(a_client_connection,person,tabla, args);
                                found = true;
                                break;
                            }
                            ++i;
                        }

                        if (found == false) {
                            enviarMensajeAlCliente(a_client_connection, std::string("Error: Invalid table name: "+ tabla +"\n"));
                        }
                    }
                    else {
                        enviarMensajeAlCliente(a_client_connection,std::string("Error: Invalid command or sintax: "+ rx_cmd+ "\n"));
                    }
                    enviarPromptAlCliente(a_client_connection);
                }
            }
        }
    }

    std::cout << "exiting server..." << std::endl;

    server.terminarServidor();

    return 0;
}

void inicializarPoolDeClientes(ClientConnection *clients_pool) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients_pool[i].socket_fd = 0;
    }
}

void cerrarConexionConCliente(ClientConnection *clients_pool, ClientConnection client, int i) {

    enviarAdiosAlCliente(client);

    // log de desconexión
    sockaddr_in addr = client.addr;
    int addrlen = sizeof(addr);
//    getpeername(client.socket_fd, (struct sockaddr *) &addr, (socklen_t *) &addrlen);
    std::cout << "connection closed\t" << inet_ntoa(addr.sin_addr) << ":" << std::to_string(ntohs(addr.sin_port)) <<
    std::endl;

    // desconexón
    close(client.socket_fd);
    clients_pool[i].socket_fd = 0;
}

void asignarClienteEnPool(ClientConnection *clients_pool, ClientConnection client) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        // Si no hay un cliente en la posición del pool
        if (clients_pool[i].socket_fd == 0) {
            clients_pool[i].socket_fd = client.socket_fd;
            clients_pool[i].addr = client.addr;
            break;
        }
    }
}

// enviar el prompt al cliente
bool enviarPromptAlCliente(ClientConnection client) {
    return enviarMensajeAlCliente(client,"picoServer>");
}


// enviar bienvenida
bool enviarBienvenidaAlCliente(ClientConnection client) {
    return enviarMensajeAlCliente(client,"Welcome to picoDB!\n");
}

bool enviarAdiosAlCliente(ClientConnection client) {
    return enviarMensajeAlCliente(client,"Goodbye!\n");
}

// enviar el prompt al cliente como simbolo de welcome
bool enviarMensajeAlCliente(ClientConnection client, std::string message) {
    if (send(client.socket_fd, message.c_str(), message.length(), 0) != message.length()) {
        return false;
    }
    return true;
}

void loggearMensajeRecibido(ClientConnection client, std::string message) {
    if (message.length() > 0) {
        std::cout << "new message from\t" << inet_ntoa(client.addr.sin_addr) << ":" <<
                                                                               std::to_string(ntohs(client.addr.sin_port)) << "\t\t"
        << "("<< std::to_string(message.length()) <<")\t"
        << message << std::endl;
    }
}

bool esMensajeDeAdios(std::string message) {
    std::string prefix = "exit();";
    if (std::mismatch(prefix.begin(), prefix.end(), message.begin()).first == prefix.end()) {
        return true;
    }
    return false;
}

bool comandoBienTerminado(std::string command) {
    return termina_con(command,std::string(";"));
}

bool esSelectSobreTabla(std::string command) {
    return empieza_con(command,std::string("select * from"));
}

bool esInsertSobreTabla(std::string command) {
    return empieza_con(command,std::string("insert into"));
}

inline bool empieza_con(std::string const &command, std::string const & prefix) {
    if (std::mismatch(prefix.begin(), prefix.end(), command.begin()).first == prefix.end()) {
        return true;
    }
    return false;
}

inline std::string trim(const std::string &s)
{
    auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
    auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
    return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

inline bool termina_con(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void enviarSelectSobreTabla(ClientConnection &client, Person &person, std::string tabla) {
	std::string select_all = person.getPersons();

	enviarMensajeAlCliente(client, select_all);

    // TODO. Implementar la lectura del archivo y la escritura a través de socket
    std::cout << "select * "<< tabla <<"\t" << inet_ntoa(client.addr.sin_addr) << ":" <<
    std::to_string(ntohs(client.addr.sin_port)) << "\t\t" <<  std::endl;
}

std::string obtenerTablaDeConsulta(std::string op, std::string query) {
    std::string::size_type i = query.find(op);

    if (i != std::string::npos)
        query.erase(i, op.length());

    std::string semicolon(";");
    i = query.find(semicolon);
    if (i != std::string::npos)
        query.erase(i, semicolon.length());

    std::string::size_type first = query.find("(");
    std::string::size_type last = query.find_last_of(")");
    if (first != std::string::npos && last != std::string::npos) {
        std::string args = query.substr (first,last-first+1);

        i = query.find(args);
        if (i != std::string::npos)
            query.erase(i, args.length());
    }
    query = trim(query);

    return query;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::vector<std::string> obtenerArgumentosDeInsert(std::string query, std::string table) {
    unsigned first = query.find("(");
    unsigned last = query.find_last_of(")");
    std::string args = query.substr (first +1,last-first-1);

    std::string whitechar("\"");
    std::string::size_type i = args.find(whitechar);
    while (i != std::string::npos) {
        args.erase(i, whitechar.length());
        i = args.find(whitechar);
    }
    args = trim(args);

    std::vector<std::string> argsvector = split(args,',');

    return argsvector;
}

void insertSobreTabla(ClientConnection client, Person &person,std::string tabla, std::vector<std::string> args) {

    std::ostringstream argsstr;

    if(args.size()==3){
    	std::string nombre = args[0];
    	std::string direccion = args[1];
    	std::string telefono = args[2];

        bool personAdded = false;
        std::string error = "";
        try {
            personAdded = person.addPerson(nombre,direccion,telefono);
        } catch (std::string exc) {
            personAdded = false;
            std::cerr << "exception on insert: " << exc << std::endl;
            error = std::string("ERROR: " + exc +  "\n");
        }


        // Convert all but the last element to avoid a trailing ","
        std::copy(args.begin(), args.end()-1, std::ostream_iterator<std::string>(argsstr, ","));

        // Now add the last element with no delimiter
        argsstr << args.back();

        std::cout << "insert into "<< tabla <<" args("<< std::to_string(args.size()) << "): " << argsstr.str() <<"\t" << inet_ntoa(client.addr.sin_addr) << ":" <<
        std::to_string(ntohs(client.addr.sin_port)) << "\t\t" <<  std::endl;

        enviarMensajeAlCliente(client,personAdded ? "1 row affected\n" : error);
    }
    else {
        // informar sobre error en la sentencia
        enviarMensajeAlCliente(client,"ERROR: invalid argument count\n");
    }
}
