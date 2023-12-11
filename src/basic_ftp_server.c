#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 2121 // FTP port number

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char pwd_response[4096];
    const char *welcome_message = "220 Welcome to Simple FTP Server\n";

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Send welcome message
    send(new_socket, welcome_message, strlen(welcome_message), 0);

    // Handle FTP-like commands
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        read(new_socket, buffer, sizeof(buffer));

        printf("Received: %s", buffer);

        //USER
        if (strncmp(buffer, "USER", 4) == 0) {
            const char *user_ok = "331 User name okay, need password.\n";
            send(new_socket, user_ok, strlen(user_ok), 0);
        }

        // PASS
        else if (strncmp(buffer, "PASS", 4) == 0) {
            const char *pass_ok = "230 User logged in.\n";
            send(new_socket, pass_ok, strlen(pass_ok), 0);
        }

        // PWD
        else if (strncmp(buffer, "PWD", 3) == 0) {
            char current_directory[4096]; // Change the buffer size as needed
            if (getcwd(current_directory, sizeof(current_directory)) != NULL) {
                // Get current working directory and send it to the client
                snprintf(pwd_response, sizeof(pwd_response), "257 \"%s\" is the current directory.\n", current_directory);
                send(new_socket, pwd_response, strlen(pwd_response), 0);
            } else {
            const char *pwd_error = "550 Failed to get the current directory.\n";
            send(new_socket, pwd_error, strlen(pwd_error), 0);
            }
        }

        // CWD
        else if (strncmp(buffer, "CWD", 3) == 0) {
            // Extract the directory path from the command
            char directory[1024];
            sscanf(buffer, "CWD %s", directory);

            // Attempt to change the directory
            if (chdir(directory) == 0) {
                const char *cwd_ok = "250 Directory successfully changed.\n";
                send(new_socket, cwd_ok, strlen(cwd_ok), 0);
            } else {
                const char *cwd_error = "550 Failed to change directory.\n";
                send(new_socket, cwd_error, strlen(cwd_error), 0);
            }
        }

        // PUT
        /* doesn't work ...
        else if (strncmp(buffer, "PUT", 3) == 0) {
            char filename[1024];
            sscanf(buffer, "PUT %s", filename);

            FILE *file = fopen(filename, "wb");
            if (file != NULL) {
                const char *ready_to_receive = "150 Ready to receive file.\n";
                send(new_socket, ready_to_receive, strlen(ready_to_receive), 0);

                // Receive and write the file data
                memset(buffer, 0, sizeof(buffer));
                int bytes_received;
                while ((bytes_received = recv(new_socket, buffer, sizeof(buffer), 0)) > 0) {
                    fwrite(buffer, 1, bytes_received, file);
                }

                fclose(file);

                const char *file_received = "226 File received successfully.\n";
                send(new_socket, file_received, strlen(file_received), 0);
            } else {
                const char *file_error = "550 Failed to create/open file.\n";
                send(new_socket, file_error, strlen(file_error), 0);
            }
        }
        */

        // QUIT
         else if (strncmp(buffer, "QUIT", 4) == 0) {
            const char *quit_ok = "221 Goodbye.\n";
            send(new_socket, quit_ok, strlen(quit_ok), 0);
            break;
        } 

        // Unknown command
        else {
            const char *unknown_cmd = "500 Unknown command.\n";
            send(new_socket, unknown_cmd, strlen(unknown_cmd), 0);
        }
    }

    // Close socket
    close(new_socket);
    close(server_fd);

    return 0;
}
