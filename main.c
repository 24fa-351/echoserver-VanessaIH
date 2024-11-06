#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int server, client;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[BUFFER_SIZE];

    if (argc != 3 ||  strcmp(argv[1], "-p") != 0) {
        fprintf(stderr, "Usage %s -p <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[2]);
    if (port <= 1024 || port > 65535) {
        fprintf(stderr, "Port must be between 1024 and 65535\n");
        exit(EXIT_FAILURE);
    }
    //check
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error in socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    //checks
    if (bind(server, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error in bind");
        close(server);
        exit(EXIT_FAILURE);
    }
    if (listen(server, 1) == -1) {
        perror("Error in listen");
        close(server);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", port);
    if ((client = accept(server, (struct sockaddr *)&client_address, &client_address_len)) == -1) {
        perror("Error in accept");
        close(server);
        exit(EXIT_FAILURE);
    }
    printf("Client is connect.\n");
    //run for each line until ordered to kill
    ssize_t reading_bytes;
    while ((reading_bytes = recv(client, buffer, sizeof(buffer)-1,0)) > 0) {
        buffer[reading_bytes] = '\0';
        send(client, buffer, reading_bytes, 0);
        if (strchr(buffer, '\n')) {
            break;
        }
    }    if (reading_bytes == 0) {
        printf("Client has been disconnected\n");
    } else if (reading_bytes < 0) {
        perror("Error in recv");
    }
    close(client);
    close(server);
    return 0;
}