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
#define SIZE 150000

/* Functions */
void error(const char *msg, int exitVal);
void readFile(char *fileName, char *string);

int main(int argc, char *argv[])
{
    int portNumber, textLength, keyLength, socketFD, newsocketFD, charsText, charsKey, pid, checkSockSending = 1, checkSockRecieving = 0;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    char stringText[2048];
    char stringKey[2048];
    char cipherArray[2048];
    socklen_t cliLength;

    memset(stringText, '\0', 2048);  // Fill arrays with null terminators and clear garbage
    memset(stringKey, '\0', 2048);   // Fill arrays with null terminators and clear garbage
    memset(cipherArray, '\0', 2048); // Fill arrays with null terminators and clear garbage

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
            charsText = recv(newsocketFD, stringText, 2047, 0); //read the text data from otp_enc

            printf("fuck: %s", stringText);

            /* TO-DO Encryption */

            /* TO-DO Encryption */

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