#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

/**************************
Function: charToInt
Description: For use with encryption. Converts a given character to its integer equivalent.
Input: character
Output: integer
**************************/
int charToInt(char ch)
{
    static const char characters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; // Array of characters

    // Find a character match
    int i;
    for (i = 0; i < 27; i++)
    {
        if (ch == characters[i])
        {
            return i;
        }
    }

    // No character was found
    return -1;
}

/**************************
Function: intToChar
Description: For use with encryption. Converts a given integer to its character equivalent.
Input: integer
Output: character
**************************/
char intToChar(int integer)
{
    static const char characters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; // Array of characters

    // Check for a valid integer
    if (integer < 0 || integer > 26)
    {
        return '$'; // Not a valid integer, return garbage character
    }

    return characters[integer];
}

/**************************
Function: validFile
Description: Parses a given file and validates that all characters are valid per assignment specs (A-Z or space)
Input: string
Output: int
**************************/
int isValidFile(char *fileName)
{
    static const char characters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; // Array of valid characters
    FILE *file = fopen(fileName, "r");
    int c;

    if (file == NULL)
    {
        fprintf(stderr, "Error (validFile): can't open file.");
        return -1; // Error opening file
    }

    while ((c = fgetc(file)) != EOF)
    {
        // Check if the current character is a valid character
        int isValid = 0;
        int i;

        for (i = 0; i < 27; i++)
        {
            if (c == characters[i] || c == '\n')
            {
                isValid = 1; // Valid character was found
                //fprintf(stdout, "A valid character was found: %c, %d\n", c, isValid);
            }
        }

        // If character was not valid, return 0
        if (isValid == 0)
        {
            //fprintf(stderr, "Error (function: validFile) - file contains invalid characters.");
            return 0;
        }
    }

    // File contains all valid characters
    return 1;
}

/**************************
Function: readFile
Description: Copies the contents of a file into a string
Input: filename (string), string to copy into (string)
Output: N/A
Cited: https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
**************************/
void readFile(char *fileName, char *string)
{
    char *buffer = 0;
    long length;
    FILE *f = fopen(fileName, "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }

    strcpy(string, buffer);
}

/**************************
Function: encryptMsg
Description: Takes a given string and encrypts it using a key.
Input: string, key
Output: N/A
**************************/
void encryptText(char *message, char *key)
{
    int msgInt, keyInt, encryptInt;
    int msgLen = strlen(message);

    // Validate that the message is shorter than the key
    if (strlen(key) < strlen(message))
    {
        fprintf(stderr, "Error (encryptMsg): key length is shorter than message length.\n");
        exit(1);
    }

    // Encrypt the message using the key
    int i;
    for (i = 0; i < msgLen; i++)
    {
        if (message[i] == '\n')
            return; // Check for new line character

        // Perform the encryption for each character
        msgInt = charToInt(message[i]);
        keyInt = charToInt(key[i]);
        encryptInt = (msgInt + keyInt) % 27;
        message[i] = intToChar(encryptInt);
    }
    message[i] = '\0'; // Null terminator
    return;
}

/**************************
Function: decryptMsg
Description: Takes a given string and decrypts it using a key.
Input: string, key
Output: N/A
**************************/
void decryptText(char *message, char *key)
{
    int msgInt, keyInt, decryptMsg;
    int msgLen = strlen(message);

    // Validate that the message is shorter than the key
    if (strlen(key) < strlen(message))
    {
        fprintf(stderr, "Error (encryptMsg): key length is shorter than message length.\n");
        exit(1);
    }

    // Encrypt the message using the key
    int i;
    for (i = 0; i < msgLen; i++)
    {
        if (message[i] == '\n')
            return; // Check for new line character

        // Perform the encryption for each character
        msgInt = charToInt(message[i]);
        keyInt = charToInt(key[i]);
        decryptMsg = (msgInt - keyInt) % 27;

        // Ensure number is 0 or higher
        if (decryptMsg < 0)
        {
            decryptMsg += 27;
        }
        message[i] = intToChar(decryptMsg);
    }
    message[i] = '\0'; // Null terminator
    return;
}

int main(int argc, char *argv[])
{
    char *testmessage = malloc(2048);
    memset(testmessage, '\0', 2048);

    char key[72] = "ASDFIUWEFNA ASDFH IIOQPPLZASDFJASDFKJASKDFJHASDKFJHASKDFASKDJHFKSDFKSFM";

    readFile("plaintext1", testmessage);
    printf("1: %s", testmessage);

    encryptText(testmessage, key);
    printf("2: %s", testmessage);

    decryptText(testmessage, key);
    printf("3: %s", testmessage);

    return 0;
}
