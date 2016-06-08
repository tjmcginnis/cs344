/*
 * Author: Tyler McGinnis
 * CS344
 * Program 4
 *
 * File: crypto.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

char* VALID_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

int get_numerical_value(char letter);
char get_character_value(int number);
int safe_modulus(int numerator, int denominator);

/*
 * Encrypt plaintext using key
 */
char* encrypt_plaintext(char* plaintext, char* key)
{
    int i;
    int tmp;
    int length;
    char* cipher;

    length = strlen(plaintext);

    cipher = malloc(sizeof(char*) * length + 1);
    memset(cipher, 0, length + 1);

    /* build cipher using mod 27 */
    for (i = 0; i < length; i++) {
        tmp = get_numerical_value(plaintext[i]) + get_numerical_value(key[i]);
        cipher[i] = get_character_value(safe_modulus(tmp, 27));
    }

    return cipher;
}

/*
 * Decrypt cipher using key
 */
char* decrypt_cipher(char* cipher, char* key)
{
    int i;
    int tmp;
    int length;
    char* plaintext;

    length = strlen(cipher);

    plaintext = malloc(sizeof(char*) * length + 1);
    memset(plaintext, 0, length + 1);

    /* Built plaintext using mod 27 */
    for (i = 0; i < length; i++) {
        tmp = get_numerical_value(cipher[i]) - get_numerical_value(key[i]);
        plaintext[i] = get_character_value(safe_modulus(tmp, 27));
    }

    return plaintext;
}

/*
 * Get int value corresponding to a letter
 * http://stackoverflow.com/questions/3217629/in-c-how-do-i-find-the-index-of-a-character-within-a-string
 */
int get_numerical_value(char letter)
{
   char *val;
   val = strchr(VALID_CHARS, letter);
   return (int)(val - VALID_CHARS);
}

/*
 * Get the char that corresponds to a number
 */
char get_character_value(int number)
{
    return VALID_CHARS[number];
}

/*
 * Perform modulus on positive and negative numerators
 */
int safe_modulus(int numerator, int denominator)
{
    if (numerator < 0) {
        numerator = numerator * -1;
        return denominator - numerator;
    }
    return numerator % denominator;
}
