/*
 ============================================================================
 Name        : Esonero1-Client.c
 Author      : Bilanzuoli Filippo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include "protocol.h"

// Error handler
void errorhandler(const char *errorMessage) {
    perror(errorMessage);
    exit(EXIT_FAILURE);
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        errorhandler("WSAStartup failed");
    }

    // Create client socket
    int c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c_socket == INVALID_SOCKET) {
        errorhandler("Socket creation failed");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PROTO_PORT);

    // Connect to the server
    if (connect(c_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        errorhandler("Connection failed");
    }

    printf("Connected to the server.\n");

    char request[100];
    char password[100];

    while (1) {
        printf("Enter the type of password (n: numeric, a: alphabetic, m: mixed, s: secure) or 'q' to quit: ");
        fgets(request, sizeof(request), stdin);

        if (request[0] == 'q') {
            send(c_socket, "q", 1, 0);
            break;
        }

        printf("Enter the desired password length: ");
        int length;
        scanf("%d", &length);
        getchar(); // Clear newline character from the buffer

        // Send request to server
        snprintf(password, sizeof(password), "%s %d", request, length);
        send(c_socket, password, strlen(password) + 1, 0);

        // Receive generated password
        int bytes_received = recv(c_socket, password, sizeof(password), 0);
        if (bytes_received > 0) {
            printf("Generated password: %s\n", password);
        }

        printf("\n");
    }

    closesocket(c_socket);
    WSACleanup();
    return 0;
}
