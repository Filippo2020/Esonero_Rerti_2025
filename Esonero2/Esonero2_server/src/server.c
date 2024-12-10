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
#include <time.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#include "protocol.h"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
#define MAX_PASSWORD_LENGTH 32
#define MIN_PASSWORD_LENGTH 6

#ifdef _WIN32
    #define close_socket closesocket
#else
    #define close_socket close
#endif

// Initialize sockets (specific for Windows)
void init_sockets() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(EXIT_FAILURE);
    }
#endif
}

// Cleanup sockets
void cleanup_sockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

// Generate numeric passwords
void generate_numeric(char *password, int length) {
    const char *digits = "0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = digits[rand() % 10];
    }
    password[length] = '\0';  // Null-terminate the password
}

// Generate alphabetic passwords
void generate_alpha(char *password, int length) {
    const char *letters = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; i++) {
        password[i] = letters[rand() % 26];
    }
    password[length] = '\0';  // Null-terminate the password
}

// Generate mixed passwords
void generate_mixed(char *password, int length) {
    const char *chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % 36];
    }
    password[length] = '\0';  // Null-terminate the password
}

// Generate secure passwords
void generate_secure(char *password, int length) {
    const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+=-";
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % 72];
    }
    password[length] = '\0';  // Null-terminate the password
}

// Generate unambiguous passwords
void generate_unambiguous(char *password, int length) {
    const char *chars = "abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % 62];
    }
    password[length] = '\0';  // Null-terminate the password
}

// Handle client requests
void handle_request(char *request, char *response) {
    char command;
    int length;

    // Parse the request (e.g., "n 8")
    sscanf(request, "%c %d", &command, &length);

    // Check if the length is valid
    if (length < MIN_PASSWORD_LENGTH || length > MAX_PASSWORD_LENGTH) {
        snprintf(response, BUFFER_SIZE, "Error: Password length must be between %d and %d characters.", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);
        return;
    }

    // Generate the password based on the command
    switch (command) {
        case 'n':
            generate_numeric(response, length);
            break;
        case 'a':
            generate_alpha(response, length);
            break;
        case 'm':
            generate_mixed(response, length);
            break;
        case 's':
            generate_secure(response, length);
            break;
        case 'u':
            generate_unambiguous(response, length);
            break;
        default:
            snprintf(response, BUFFER_SIZE, "Error: Invalid command.");
            break;
    }
}

int main() {
    int server_socket;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Initialize sockets
    init_sockets();

    // Create UDP socket
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close_socket(server_socket);
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", SERVER_PORT);

    while (1) {
        // Receive request from client
        int n = recvfrom(server_socket, buffer, BUFFER_SIZE, 0,
                         (struct sockaddr *)&client_addr, &client_addr_len);
        if (n < 0) {
            perror("Error receiving data");
            continue;
        }

        buffer[n] = '\0';  // Null-terminate the received string
        printf("New request from %s:%d: %s\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        // Handle the request and generate the password
        handle_request(buffer, response);

        // Send the generated password back to the client
        sendto(server_socket, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, client_addr_len);
    }

    close_socket(server_socket);
    cleanup_sockets();
    return 0;
}

