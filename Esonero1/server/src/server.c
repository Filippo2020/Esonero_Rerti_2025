/*
 ============================================================================
 Name        : Esonero1_Server.c
 Author      : Bilanzuoli Filippo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>   // Include Windows socket library
#include "protocol.h"   // Assuming this header defines PROTO_PORT, etc.

// Error handler
void errorhandler(const char *errorMessage) {
    perror(errorMessage);
    exit(EXIT_FAILURE);
}

// Generate numeric password
void generate_numeric(char *password, int length) {
    const char charset[] = "0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % 10];
    }
    password[length] = '\0';
}

// Generate alphabetic password
void generate_alpha(char *password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % 26];
    }
    password[length] = '\0';
}

// Generate mixed password (letters and numbers)
void generate_mixed(char *password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % 36];
    }
    password[length] = '\0';
}

// Generate secure password (letters, numbers, and symbols)
void generate_secure(char *password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % 72];
    }
    password[length] = '\0';
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        errorhandler("WSAStartup failed");
    }

    // Create server socket
    int s_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_socket == INVALID_SOCKET) {
        errorhandler("Socket creation failed");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PROTO_PORT);

    // Bind socket
    if (bind(s_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        errorhandler("Bind failed");
    }

    // Listen for connections
    if (listen(s_socket, QLEN) == SOCKET_ERROR) {
        errorhandler("Listen failed");
    }

    printf("Waiting for client connections...\n");

    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);  // Use 'int' instead of 'socklen_t'
        int c_socket = accept(s_socket, (struct sockaddr *)&client_addr, &client_len);
        if (c_socket == INVALID_SOCKET) {
            perror("Accept failed");
            continue;
        }

        printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        char request[100];
        int bytes_received = recv(c_socket, request, sizeof(request), 0);
        if (bytes_received <= 0) {
            closesocket(c_socket);  // Windows version of close
            continue;
        }

        // If client sends 'q', close the connection
        if (request[0] == 'q') {
            closesocket(c_socket);
            break;
        }

        // Parse the request: e.g. "n 8"
        char type;
        int length;
        sscanf(request, "%c %d", &type, &length);

        if (length < MIN_PASSWORD_LENGTH || length > MAX_PASSWORD_LENGTH) {
            char *error_msg = "Password length must be between 6 and 32 characters.\n";
            send(c_socket, error_msg, strlen(error_msg), 0);
            closesocket(c_socket);
            continue;
        }

        char password[MAX_PASSWORD_LENGTH + 1];

        // Generate password based on type
        switch (type) {
            case 'n':  // Numeric password
                generate_numeric(password, length);
                break;
            case 'a':  // Alphabetic password
                generate_alpha(password, length);
                break;
            case 'm':  // Mixed password
                generate_mixed(password, length);
                break;
            case 's':  // Secure password
                generate_secure(password, length);
                break;
            default:
                strcpy(password, "Invalid request");
                break;
        }

        // Send the generated password to the client
        send(c_socket, password, strlen(password) + 1, 0);
        printf("Generated password: %s\n", password);

        closesocket(c_socket);
    }

    closesocket(s_socket);  // Close the server socket
    WSACleanup();  // Clean up Winsock
    return 0;
}

