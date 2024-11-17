/*
 * protocol.h
 *
 *  Created on: 12 nov 2024
 *      Author: filip
 */

#ifndef PASSWORD_GEN_H
#define PASSWORD_GEN_H

#define PROTO_PORT 27015  // Default port
#define MAX_PASSWORD_LENGTH 32
#define MIN_PASSWORD_LENGTH 6
#define QLEN 5            // Max queue size for client connections

// Password types
#define PASSWORD_NUMERIC 'n'
#define PASSWORD_ALPHABETIC 'a'
#define PASSWORD_MIXED 'm'
#define PASSWORD_SECURE 's'

// Function prototypes for password generation
void generate_numeric(char *password, int length);
void generate_alpha(char *password, int length);
void generate_mixed(char *password, int length);
void generate_secure(char *password, int length);

#endif
