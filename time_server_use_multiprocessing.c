#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            break;
        }

        char* command = strtok(buffer, "\n");

        if (strcmp(command, "GET_TIME") == 0) {
            time_t now = time(NULL);
            char* current_time = ctime(&now);
            send(client_socket, current_time, strlen(current_time), 0);
        } else if (strncmp(command, "GET_TIME ", 9) == 0) {
            char* format = strtok(command + 9, "\n");

            if (strcmp(format, "dd/mm/yyyy") == 0) {
                time_t now = time(NULL);
                struct tm* local_time = localtime(&now);
                char formatted_time[20];
                strftime(formatted_time, sizeof(formatted_time), "%d/%m/%Y", local_time);
                send(client_socket, formatted_time, strlen(formatted_time), 0);
            } else if (strcmp(format, "dd/mm/yy") == 0) {
                time_t now = time(NULL);
                struct tm* local_time = localtime(&now);
                char formatted_time[20];
                strftime(formatted_time, sizeof(formatted_time), "%d/%m/%y", local_time);
                send(client_socket, formatted_time, strlen(formatted_time), 0);
            } else if (strcmp(format, "mm/dd/yyyy") == 0) {
                time_t now = time(NULL);
                struct tm* local_time = localtime(&now);
                char formatted_time[20];
                strftime(formatted_time, sizeof(formatted_time), "%m/%d/%Y", local_time);
                send(client_socket, formatted_time, strlen(formatted_time), 0);
            } else if (strcmp(format, "mm/dd/yy") == 0) {
                time_t now = time(NULL);
                struct tm* local_time = localtime(&now);
                char formatted_time[20];
                strftime(formatted_time, sizeof(formatted_time), "%m/%d/%y", local_time);
                send(client_socket, formatted_time, strlen(formatted_time), 0);
            } else {
                char* error_message = "Invalid format\n";
                send(client_socket, error_message, strlen(error_message), 0);
            }
        } else {
            char* error_message = "Invalid command\n";
            send(client_socket, error_message, strlen(error_message), 0);
        }
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length;
    pid_t child_pid;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(9000);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    listen(server_socket, MAX_CLIENTS);
    printf("Server is listening on port 9000\n");

    while (1) {
        client_address_length = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_length);
        if (client_socket < 0) {
            perror("Failed to accept connection");
            exit(1);
        }

        child_pid = fork();
        if (child_pid < 0) {
            perror("Failed to create child process");
            exit(1);
        }

        if (child_pid == 0) {
            // Child process
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        } else {
            // Parent process
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
} 