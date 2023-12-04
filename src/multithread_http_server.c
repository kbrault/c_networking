#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 8888
#define MAX_CONNECTIONS 10

// Function to handle client requests in a separate thread.
// Parameters:
//   - arg: Pointer to a void argument, expected to be cast as an 
//          integer pointer containing the client socket descriptor.
// Returns:
//   - NULL: Since it's a thread function, it returns NULL after handling 
//           the client request.
void *handle_client(void *arg) {
    // Cast the argument to an integer pointer to retrieve the client 
    // socket descriptor
    int client_socket = *((int *)arg);

    // Free the memory allocated for the argument
    free(arg); 

    // Variables for handling date and time
    time_t now;
    struct tm *local_time;
    char time_str[100];

    // Get the current time
    now = time(NULL);
    local_time = localtime(&now);

    // Format time as string
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time); 

    // Print the timestamp and thread ID for the handled client request
    printf("[%s] Handling client request in thread %ld\n", time_str, 
            pthread_self());

    // HTTP response to be sent to the client
    char response[] = "HTTP/1.1 200 OK\n"
                      "Content-Type: text/html\n\n"
                      "<h1>Hello, world !</h1>";
    
    // Send the HTTP response to the client
    write(client_socket, response, strlen(response));
    close(client_socket); // Close the client socket

    return NULL;
}

int main() {
    int server_socket; // Declare server_socket variable
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create a socket for communication
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int option = 1;
    // Set socket options to enable address reuse
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, 
        sizeof(option))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address settings
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the server socket to a specific address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, 
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %d\n", PORT);

    // Continuously accept and handle incoming client connections
    while (1) {
        int *client_socket = malloc(sizeof(int));
        if (client_socket == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        // Accept incoming client connection
        *client_socket = accept(server_socket, (struct sockaddr *)&client_addr, 
                                &addr_len);
        if (*client_socket < 0) {
            perror("Accept failed");
            free(client_socket);
            exit(EXIT_FAILURE);
        }

        // Thread identifier
        pthread_t tid;

        // Create a new thread to handle the client request.
        // Parameters:
        //   - &tid: Pointer to a pthread_t variable where the thread 
        //           identifier will be stored.
        //   - NULL: Optional thread attributes. Here, it's set to NULL for 
        //           default attributes.
        //   - handle_client: Function pointer to the function that will be 
        //                    executed by the thread.
        //   - (void *)client_socket: Pointer to void type, casting the
        //                            client_socket to pass it as an argument.
        // Return Value:
        //   - Returns 0 on successful thread creation.
        //   - Returns a non-zero value if thread creation fails.
        if (pthread_create(&tid, NULL, handle_client, 
                          (void *)client_socket) != 0) {
            perror("Thread creation failed");
            free(client_socket);
            exit(EXIT_FAILURE); 
        }
        pthread_detach(tid); // Detach the thread for automatic cleanup
    }

    return 0;
}
