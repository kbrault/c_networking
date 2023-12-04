#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <arpa/inet.h>

#define PORT 8888
#define MAX_CONNECTIONS 10

/*
     +----------------------------------+
     |             socket()             |
     | Create a communication endpoint  |
     |        (socket descriptor)       |
     +----------------------------------+
                      |
                      v
     +----------------------------------+
     |              bind()              |
     |   Associate socket with a        |
     | specific address and port number |
     +----------------------------------+
                      |
                      v
     +----------------------------------+
     |             listen()             |
     |     Mark socket as passive,      |
     |    ready to accept connections   |
     +----------------------------------+
                      |
                      v
     +----------------------------------+
     |             accept()             |
     |     Accept incoming connection   |
     |     and create new socket for    |
     |        bidirectional data        |
     |         exchange with client     |
     +----------------------------------+
                      |
                      v
     +----------------------------------+
     |            write()               |
     |     Send data over the socket    |
     +----------------------------------+
                      |
                      v
     +----------------------------------+
     |            close()               |
     |     Close the socket             |
     +----------------------------------+
*/

// Handle the incoming HTTP request from the client by sending a simple 
// HTTP response.
// Parameters:
//   - client_socket: the socket descriptor for the client connection.
void handle_request(int client_socket) {
    // Define an HTTP response message
    char response[] = "HTTP/1.1 200 OK\n"
                      "Content-Type: text/html\n\n"
                      "<h1>Hello, world !</h1>";

    // Send an HTTP response to the client socket.
    // Parameters:
    //   - client_socket: the socket descriptor for the client connection.
    //   - response: a pointer to the buffer containing the HTTP response message.
    //   - strlen(response): the length of the response message to send.
    write(client_socket, response, strlen(response));

    // Close the client socket after handling the HTTP request.
    // Parameter:
    //   - client_socket: the socket descriptor for the client connection.
    close(client_socket);
}

int main(){
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create a socket for communication using IPv4 and TCP (stream-oriented).
    // Parameters:
    //   - AF_INET: the address family for IPv4 communication.
    //   - SOCK_STREAM: the socket type for stream-oriented communication.
    //   - 0: the protocol value for the specified socket type; 0 indicates 
    //        that the system should choose the protocol.
    // Return Value:
    //   - Returns a socket descriptor on success, which represents the 
    //     endpoint of communication.
    //   - Returns -1 if an error occurs during socket creation.
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow reuse of the server_socket address and port.
    // Parameters:
    //   - server_socket: the socket descriptor for the server's endpoint.
    //   - SOL_SOCKET: the level at which the option is defined, indicating 
    //                 the socket layer.
    //   - SO_REUSEADDR: the socket option to enable reuse of the server 
    //                   address and port.
    //   - &option: a pointer to the value indicating whether the option is 
    //              enabled or disabled.
    //   - sizeof(option): the size of the 'option' value in bytes.
    // Return Value:
    //   - Returns 0 on success.
    //   - Returns -1 if an error occurs during setting socket options.
    int option = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, 
                   sizeof(option))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;

    // --> bind to all interfaces
    server_addr.sin_addr.s_addr = INADDR_ANY; 

    // "host to network short"
    //      --> primarily used to convert a 16-bit quantity from host 
    //          byte order to network byte order
    server_addr.sin_port = htons(PORT); // little-endian

    // Bind the server_socket to a specific address and port.
    // Parameters:
    //   - server_socket: the socket descriptor for the server's endpoint.
    //   - (struct sockaddr *)&server_addr: a pointer to a sockaddr 
    //                                      structure containing the server's 
    //                                      address and port.
    //   - sizeof(server_addr): the size of the server_addr structure in bytes.
    // Return Value:
    //   - Returns 0 on success.
    //   - Returns -1 if an error occurs during the binding process.
    if (bind(server_socket, (struct sockaddr *)&server_addr, 
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Set the server_socket to listen for incoming connections.
    // Parameters:
    //   - server_socket: the socket descriptor for the server's endpoint.
    //   - MAX_CONNECTIONS: the maximum number of pending connections that 
    //                      can be queued.
    // Return Value:
    //   - Returns 0 on success.
    //   - Returns -1 if an error occurs during the listening process.
    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d\n", PORT);

    // Continuously listen for incoming client connections
    while (1) {
        // Accept an incoming connection request from a client using the 
        // 'accept' system call.
        // Parameters:
        //   - server_socket: the socket descriptor for the server's endpoint.
        //   - (struct sockaddr *)&client_addr: a pointer to a sockaddr 
        //                                      structure that will store the 
        //                                      client's address.
        //   - &addr_len: a pointer to the size of the client_addr structure.
        // Return Value:
        //   - Returns a new socket descriptor for the established connection 
        //     if successful.
        //   - Returns -1 if an error occurs during the connection acceptance 
        //     process.
        if ((client_socket = accept(server_socket, 
                                    (struct sockaddr *)&client_addr, 
                                    &addr_len)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Upon successful connection, handle the client request
        handle_request(client_socket);
    }

    return 0;
}