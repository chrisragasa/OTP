/*******************************
Christopher Ragasa
OSU - CS 344, Program 4 (OTP)
11/30/18
Program Description: 

otp_enc.c accepts a plaintext file, a key, and a port number. This program reads the files
and sends this information (using sockets) to otp_enc_d.c, where it will be encrypted.
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
int isValidFile(char *fileName);
void readFile(char *fileName, char *string);
int setupSocket(int portNumber);
void appendToken(char *buffer);
void removeToken(char *buffer);

int main(int argc, char *argv[])
{
    int portNumber, textLength, keyLength, socketFD, bytesSent, r, charsText, charsKey, checkSockSending = 0, checkSockRecieving = 1;
    struct sockaddr_in serverAddress;
    struct hostent *server;
    char stringText[SIZE]; // String holding the plaintext content
    char stringKey[SIZE];  // String holding the key content
    char readBuffer[512];
    FILE *fpText, *fpKey;           // File pointers
    memset(stringText, '\0', SIZE); // Fill arrays with null terminators and clear garbage
    memset(stringKey, '\0', SIZE);  // Fill arrays with null terminators and clear garbage
    memset(readBuffer, '\0', 512);

    /* Check for the correct number of arguments */
    if (argc < 4)
        error("ERROR: Incorrect number of arguments.\nSYNTAX: opt_enc plaintext key port", 1);

    /* Check that the files are readable and contain valid characters */
    if (!isValidFile(argv[1]))
        error(" The file could not be opened.", 1);
    if (!isValidFile(argv[2]))
        error(" The file could not be opened.", 1);

    /* Open the files */
    fpText = fopen(argv[1], "r");
    fpKey = fopen(argv[2], "r");

    /* Check files were opened successfully */
    if (fpText == NULL)
        error("ERROR: problem opening plaintext file", 1);
    if (fpKey == NULL)
        error("ERROR: problem opening key file", 1);

    /* Read plaintext and key files into strings */
    readFile(argv[1], stringText); // Copy contents of fpText into stringText
    readFile(argv[2], stringKey);  // Copy contents of fpKey into stringKey

    /* Validate key size is larger than plaintext size */
    textLength = strlen(stringText);
    keyLength = strlen(stringKey);
    if (keyLength < textLength)
        error("ERROR: key size smaller than plaintext size", 1); // Error if the key size is smaller than the plaintext size

    /* Port and Socket Setup */
    portNumber = atoi(argv[3]);                 // Get the port number, convert to an integer from a string
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0)
        error("ERROR: client couldn't open socket", 1); // If socket < 0, then it was not set
    server = gethostbyname("localhost");                //connect to self
    if (server == NULL)
    {
        error("ERROR: client couldn't connect to the server", 1); //If the server is null, then it is not connected
    }
    memset((char *)&serverAddress, '\0', sizeof(serverAddress));                             // Clear out the address struct
    serverAddress.sin_family = AF_INET;                                                      // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber);                                              // Store the port number
    bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length); // Copy the server information
    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {                                                                            // Check connection to the server
        printf("ERROR: could not contact the daemon at port: %d\n", portNumber); // Error if connection to daemon was not successful
        exit(2);
    }

    /* Authorization */
    send(socketFD, &checkSockSending, sizeof(checkSockSending), 0);
    recv(socketFD, &checkSockRecieving, sizeof(checkSockRecieving), 0);
    if (checkSockRecieving != 0)
        error("ERROR: otp_enc can't connect to otp_dec_d", 1);

    /* Sending the plaintext */
    appendToken(stringText);                             // Add the token
    bytesSent = 0;                                       // Keep track of bytes sent
    charsText = send(socketFD, stringText, SIZE - 1, 0); // Write information to server
    bytesSent = bytesSent + charsText;                   // Keep track of bytes sent
    if (charsText < 0)
    { // If less than 0, then information was not sent
        error("ERROR: client couldn't write plaintext to the socket", 1);
    }
    // Make sure everything is sent properly
    while (bytesSent < SIZE - 1)
    {
        charsText = send(socketFD, &stringText[bytesSent], SIZE - (bytesSent - 1), 0); // Send the bytes that haven't been sent yet
        bytesSent = bytesSent + charsText;                                             // Keep track of bytes sent
    }

    /* Sending the key */
    appendToken(stringKey); // Add the token
    bytesSent = 0;
    charsKey = send(socketFD, stringKey, SIZE - 1, 0); // Write information to server
    bytesSent = bytesSent + charsText;                 // Keep track of bytes sent
    if (charsKey < 0)
    { // If less than 0, then information was not sent
        error("ERROR: client couldn't write key to the socket", 1);
    }
    // Make sure everything is sent properly
    while (bytesSent < SIZE - 1)
    {
        charsText = send(socketFD, &stringKey[bytesSent], SIZE - (bytesSent - 1), 0); // Send the bytes that haven't been sent yet
        bytesSent = bytesSent + charsText;                                            // Keep track of bytes sent
    }

    /* Recieve the ciphered text */
    memset(stringText, '\0', SIZE); // Fill arrays with null terminators and clear garbage
    charsText = recv(socketFD, stringText, SIZE - 1, 0);
    if (charsText < 0)
    {
        error("ERROR: client error reading ciphered text from socket", 1);
    }

    // While the total amount of bytes sent does not equal the size of the message
    while (strstr(stringText, "@@") == NULL)
    {
        memset(readBuffer, '\0', sizeof(readBuffer));              // Clear the buffer
        r = recv(socketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Get the next chunk
        strcat(stringText, readBuffer);                            // Add chunk to what we have so far
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
    removeToken(stringText);
    printf("%s", stringText);
    close(socketFD);
    return 0;
}

/**************************
Function: isValidFile
Description: Parses a given file and validates that the file is readable 
and that all characters are valid per assignment specs (A-Z or space)
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
        fprintf(stderr, "ERROR: %s is not a valid filename.", fileName);
        return 0; // Error opening file
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
            fprintf(stderr, "ERROR: %s contains invalid characters.", fileName);
            fclose(file);
            return 0;
        }
    }

    // File contains all valid characters
    fclose(file);
    return 1;
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
Function: setupSocket
Description: Socket setup
Input: portNo (int) - a valid port number that will be used for the socket
Output: sockFD (int) - int that represents socket file descriptor
Cited: client.c from OSU Lectures Block 4 and Beej's Guide to Network Programming (https://beej.us/guide/bgnet/html/multi/index.html)
**************************/
int setupSocket(int portNumber)
{
    int socketFD;                                        // Socket file descriptor
    struct sockaddr_in serverAddress;                    // Server address struct
    struct hostent *server = gethostbyname("localhost"); // Get host IP
    if (server == NULL)
        error("ERROR: host was not found\n", 1); // error if host not found

    /* Open the socket */
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // returns a socket descriptor that we can use in later system calls. returns -1 on error.
    if (socketFD < 0)
    {
        error("ERROR: opening socket", 1); // check if the return value was -1 to signify an error
    }

    /* Set up server address struct */
    memset((char *)&serverAddress, '\0', sizeof(serverAddress));                             // Clear out the address struct
    serverAddress.sin_family = AF_INET;                                                      // Create a network-capable socket
    bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length); // copy serv_addr ip into server->h_addr
    serverAddress.sin_port = htons(portNumber);                                              // Store the port number

    /* Connect to server */
    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        error("ERROR: connecting to socket", 1); // Connection error
    }

    return socketFD;
}

/**************************
Function: appendToken
Description: Takes a given string and concatenates a '@@' token
Input: string
Output: N/A
**************************/
void appendToken(char *buffer)
{
    buffer[strcspn(buffer, "\n")] = 0; // Remove trailing new line
    strcat(buffer, "@@");              // Append the token
    strcat(buffer, "\n");              // Add a new line
}

/**************************
Function: removeToken
Description: Takes a given string and removes the '@@' token
Input: string
Output: N/A
**************************/
void removeToken(char *buffer)
{
    buffer[strcspn(buffer, "\n")] = 0; // Remove trailing new line
    buffer[strcspn(buffer, "@@")] = 0; // Remove the token
    strcat(buffer, "\n");              // Add a new line
}