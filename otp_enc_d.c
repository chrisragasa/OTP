/*******************************
Christopher Ragasa
OSU - CS 344, Program 4 (OTP)
11/30/18
Program Description: 

OTP is a system that will encypt and decrypt information using a one-time pad-like system.
This program will run in the background and will listen on a particular port/socket assigned
when it is first run. The function of this program is to perform the encrpytion; to convert
a plaintext message to a ciphertext using a given key.
********************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define h_addr h_addr_list[0]
#define SIZE 75000

/* Functions */
void error(const char *msg, int exitVal);
void readFile(char *fileName, char *string);
void encryptText(char *message, char *key);
int charToInt(char ch);
char intToChar(int integer);
void appendToken(char *buffer);

int main(int argc, char *argv[])
{
    int portNumber, textLength, keyLength, socketFD, newsocketFD, bytesSent, charsText, charsKey, pid, r, checkSockSending = 1, checkSockRecieving = 0;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    char stringText[SIZE];
    char stringKey[SIZE];
    char cipherArray[SIZE];
    char readBuffer[512];
    socklen_t cliLength;

    memset(stringText, '\0', SIZE);  // Fill arrays with null terminators and clear garbage
    memset(stringKey, '\0', SIZE);   // Fill arrays with null terminators and clear garbage
    memset(cipherArray, '\0', SIZE); // Fill arrays with null terminators and clear garbage
    memset(readBuffer, '\0', 512);   // Fill arrays with null terminators and clear garbage

    /* Check for the correct number of arguments */
    if (argc < 2)
        error("ERROR: Incorrect number of arguments.\nSYNTAX: otp_enc_d port", 1);

    /* Port and Socket Setup */
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    portNumber = atoi(argv[1]);                 // Get the port number
    if (socketFD < 0)
    { // Check for socket creation error
        error("ERROR: server couldn't open the socket", 1);
    }
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    serverAddress.sin_family = AF_INET;                          // Create a network-capable socket
    serverAddress.sin_addr.s_addr = INADDR_ANY;                  // Any address is allowed for connection to this process
    serverAddress.sin_port = htons(portNumber);                  // Store the port number
    if (bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    { // Check for bind error
        error("ERROR: server couldn't bind", 0);
    }
    listen(socketFD, 5);               // 5 concurrent connections
    cliLength = sizeof(clientAddress); // Set the length of the client address

    /* Listening Loop */
    while (1)
    {
        newsocketFD = accept(socketFD, (struct sockaddr *)&clientAddress, &cliLength); // Creates a new connected socket, returns it's new file descriptor
        if (newsocketFD < 0)
        { // Error if the socket couldn't be created
            error("ERROR: server couldn't set up socket on accept", 1);
        }
        pid = fork(); // Create child
        if (pid < 0)
        {
            error("ERROR: fork error", 1);
        }
        else if (pid == 0)
        { // I am the child
            /* Port authorization */
            recv(newsocketFD, &checkSockRecieving, sizeof(checkSockRecieving), 0);
            if (checkSockRecieving == 0)
            {
                send(newsocketFD, &checkSockRecieving, sizeof(checkSockRecieving), 0);
            }
            else
            {
                send(newsocketFD, &checkSockSending, sizeof(checkSockSending), 0);
                exit(1);
            }
            close(socketFD);

            /* Recieve the files */
            charsText = recv(newsocketFD, stringText, SIZE - 1, 0); //read the text data from otp_enc
            if (charsText < 0)
            {
                error("ERROR: server can't read plaintext from the socket", 1);
            }
            while (strstr(stringText, "@@") == NULL)
            {
                memset(readBuffer, '\0', sizeof(readBuffer));                 // Clear the buffer
                r = recv(newsocketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Get the next chunk
                strcat(stringText, readBuffer);                               // Add chunk to what we have so far
                if (r == -1)
                {
                    printf("r == -1\n");
                    break;
                } // Check for errors
                if (r == 0)
                {
                    printf("r == 0\n");
                    break;
                }
            }

            charsKey = recv(newsocketFD, stringKey, SIZE - 1, 0); // read buffer data from otp_enc
            if (charsKey < 0)
            {
                error("ERROR: server can't read key from the socket", 1);
            }
            // Receive until a the terminating token is found
            while (strstr(stringKey, "@@") == NULL)
            {
                memset(readBuffer, '\0', sizeof(readBuffer));                 // Clear the buffer
                r = recv(newsocketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Get the next chunk
                strcat(stringKey, readBuffer);                                // Add chunk to what we have so far
                if (r == -1)
                {
                    printf("r == -1\n");
                    break;
                } // Check for errors
                if (r == 0)
                {
                    printf("r == 0\n");
                    break;
                }
            }

            /* Encryption */
            strcpy(cipherArray, stringText);     // Copy plaintext into cipherArray
            encryptText(cipherArray, stringKey); // Perform the encryption

            /* Send encrypted version back to client */
            bytesSent = 0; // Keep track of the bytes sent
            charsText = send(newsocketFD, cipherArray, SIZE - 1, 0);
            bytesSent = bytesSent + charsText; // Keep track of the bytes sent
            if (charsText < 0)
            {
                error("ERROR: server can't send encryption to socket", 1);
            }
            // While the total amount of bytes sent does not equal the size of the message
            while (bytesSent < SIZE - 1)
            {
                charsText = send(newsocketFD, &cipherArray[bytesSent], SIZE - (bytesSent - 1), 0); // Send the bytes that haven't been sent yet
                bytesSent = bytesSent + charsText;                                                 // Keep track of the bytes sent
            }
            close(newsocketFD); // Close the socket
            close(socketFD);    // Close the socket
            exit(0);            // Child dies
        }
        else
        {
            close(newsocketFD); // Parent closes the new socket
        }
    }

    return 0;
}

/**************************
Function: error
Description: A utility function to display error messages with exit values
Input: error message (string), exit value (int)
Output: N/A
**************************/
void error(const char *msg, int exitVal)
{
    fprintf(stderr, "%s\n", msg);
    exit(exitVal);
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
Function: encryptText
Description: Takes a given string and encrypts it using a key.
Input: string, key
Output: N/A
**************************/
void encryptText(char *message, char *key)
{
    int msgInt, keyInt, encryptInt;
    int msgLen = strlen(message);
    int keyLen = strlen(key);

    // Validate that the message is shorter than the key
    if (keyLen < msgLen)
    {
        fprintf(stderr, "Error (encryptMsg): key length is shorter than message length.\n");
        exit(1);
    }

    // Encrypt the message using the key
    int i;
    for (i = 0; i < msgLen; i++)
    {
        if (message[i] == '\n' || message[i] == '@')
            return; // Check for new line character or terminating token

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
Function: appendToken
Description: Takes a given string and concatenates '@@' to it
Input: string
Output: N/A
**************************/
void appendToken(char *buffer)
{
    buffer[strcspn(buffer, "\n")] = 0; // Remove trailing new line
    strcat(buffer, "@@");              // Append the token
    strcat(buffer, "\n");              // Add a new line
}