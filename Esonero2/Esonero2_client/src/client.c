/*
 ============================================================================
 Name        : Esonero2_client.c
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

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#include "protocol.h"
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

#ifdef _WIN32
    #define close_socket closesocket
#else
    #define close_socket close
#endif

void init_sockets() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(EXIT_FAILURE);
    }
#endif
}

void cleanup_sockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

void print_help() {
    printf("Password Generator Menu\n");
    printf("Commands:\n");
    printf("  h        : show menu\n");
    printf("  n LENGTH : generate numeric password (digits only)\n");
    printf("  a LENGTH : generate alphabetic password (lowercase letters)\n");
    printf("  m LENGTH : generate mixed password (lowercase letters and numbers)\n");
    printf("  s LENGTH : generate secure password (uppercase, lowercase, numbers, symbols)\n");
    printf("  u LENGTH : generate unambiguous secure password (no similar-looking characters)\n");
    printf("  q        : quit application\n");
    printf("\nLENGTH must be between 6 and 32 characters.\n");
}

int main() {
    int client_socket;
    char buffer[BUFFER_SIZE];
    char server_response[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);

    // Initialize sockets
    init_sockets();

    // Create UDP socket
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // Convert server address from string to binary format (Windows specific)
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        fprintf(stderr, "Invalid server address\n");
        cleanup_sockets();
        close_socket(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Welcome to the Password Generator Client!\n");
    print_help();

    while (1) {
        printf("\nEnter a command: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "q") == 0) {
            printf("Exiting...\n");
            break;
        } else if (strcmp(buffer, "h") == 0) {
            print_help();
            continue;
        }

        // Extract command and length
        char command;
        int length;
        if (sscanf(buffer, "%c %d", &command, &length) == 2) {
            if (length < 6 || length > 32) {
                printf("Error: Length must be between 6 and 32 characters.\n");
                continue;
            }
        } else {
            printf("Error: Invalid command format. Use 'h' for menu.\n");
            continue;
        }

        // Send request to server
        sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, server_addr_len);

        // Receive response from server
        int n = recvfrom(client_socket, server_response, sizeof(server_response), 0, (struct sockaddr*)&server_addr, &server_addr_len);
        if (n < 0) {
            perror("Error receiving data");
            continue;
        }

        server_response[n] = '\0';
        printf("Password received: %s\n", server_response);
    }

    // Cleanup and close
    close_socket(client_socket);
    cleanup_sockets();
    return 0;
}
