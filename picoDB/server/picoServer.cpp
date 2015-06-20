#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include "ServerSocket.h"

#define MAX_CLIENTS          10
#define MAX_PENDING_CONNECTIONS 5
#define SERVER_PORT          5000

int main(int argc , char *argv[])
{
    /*
    int opt = TRUE;
    int server_socket , addrlen , new_socket , client_socket[MAX_CLIENTS] , activity, i , valread , sd;
    int max_sd;

    char buffer[1025];  //data buffer of 1K



    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        client_socket[i] = 0;
    }

    //create a master socket
    if( (server_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( SERVER_PORT );

    //bind the socket to localhost port 5000
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", SERVER_PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(server_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");
    */

    ServerSocket server(SERVER_PORT, MAX_PENDING_CONNECTIONS);
    server.iniciarServicio();

    std::cout << "picoServer: running on " << server.hostname() << ":" << std::to_string(SERVER_PORT) << std::endl;

    // file descriptors
    fd_set readfds;
    int master_socket = server.socketFD();

    // clientes
    int clients_socket[MAX_CLIENTS];
    int sd, max_sd;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients_socket[i] = 0;
    }

    bool server_running = true;

    struct sockaddr_in address = server.address();
    int addrlen = sizeof(address);

    while(server_running)  {

        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for (int i = 0 ; i < MAX_CLIENTS ; i++)
        {
            //socket descriptor
            sd = clients_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            int new_socket = -1;

            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&(addrlen)))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            //send new connection greeting message
            std::string welcome("welcome to picoServer\n");
            if( send(new_socket, welcome.c_str(), welcome.length(), 0) != welcome.length() )
            {
                perror("send");
            }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                //if position is empty
                if( clients_socket[i] == 0 )
                {
                    clients_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);

                    break;
                }
            }
        }

        //else its some IO operation on some other socket :)
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = clients_socket[i];

            int valread;
            char buffer[1024];

            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    clients_socket[i] = 0;
                }

                    //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end of the data read
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }

    server.terminarServidor();

    return 0;
}

/*
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "../shared/inc/person.h"
#include "ServerSocket.h"
#include <unistd.h>

#define MAX_REQUEST_BUFFSIZE    1024
#define SERVER_PORT             5000
#define SERVER_MAX_WORKERS      4
#define SERVER_MAX_PENDING_CONNECTIONS  10

using namespace std;

int main() {
    std::cout << "Running picoDB Server" << endl;

    string hostname;

    {
        char name[256];
        int r = gethostname(name, 256);
        hostname = std::string(name);
    }

    std::cout << "Hostname: " << hostname << endl << endl;


    bool serverRunning = true;

    try {
        ServerSocket socket ( SERVER_PORT, SERVER_MAX_PENDING_CONNECTIONS );

        while (serverRunning == true) {  // TODO implement signal handler for quit

            std::cout << "picoDB Server: waiting for clients" << std::endl;
            std::cout << "picoDB Server: close connection sending 'exit();'" << std::endl << std::endl;

            struct sockaddr_in their_addr  = socket.abrirConexion();

            pid_t childPid = fork();

            std::cout << "picoDB Server: got a connection from "<< inet_ntoa(their_addr.sin_addr)  <<":" << htons(their_addr.sin_port) << endl;

            if (childPid == 0) {

                std::string request;

                char buffer[MAX_REQUEST_BUFFSIZE];

                do {
                    int bytesRecibidos = socket.recibir ( static_cast<void*>(buffer),MAX_REQUEST_BUFFSIZE );
                    if (bytesRecibidos > 0) {
                        request = buffer;
                        request.resize(bytesRecibidos);
                        std::cout << "picoDB Server: Request received: " << request << std::endl;

                        std::cout << "picoDB Server: sending Response . . ." << std::endl;
                        socket.enviar(static_cast<const void *>(request.c_str()), request.size());
                    }
                } while ( request != std::string("exit();") );

                std::cout << "picoDB Server: closing connection from "<< inet_ntoa(their_addr.sin_addr)  <<":" << htons(their_addr.sin_port) << endl;
                socket.cerrarConexion ();

                // remove resources

                // kill process
                exit(0);
            }
            else {
                socket.cerrarConexion ();
            }
        }
    } catch (std::string& error) {
        std::cerr << "Exception: " << error << std::endl;
    }

    exit(0);
}
 */