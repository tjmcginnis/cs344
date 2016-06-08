/*
 * Author: Tyler McGinnis
 * CS344
 * Program 4
 *
 * File: crypto.h
 */

#ifndef CRYPTO_H
#define CRYPTO_H

/*
 * Encrypt plaintest using key
 */
char* encrypt_plaintext(char* plaintext, char* key);

/*
 * Decrypt cipher using key
 */
char* decrypt_cipher(char* cipher, char* key);

#endif
