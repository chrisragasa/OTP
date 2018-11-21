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

/* Functions */
void error(const char *msg, int exitVal);
int isValidFile(char *fileName);

int main(int argc, char *argv[])
{
    /* Check for the correct number of arguments */
    if (argc < 3)
        error("ERROR: Incorrect number of arguments.\nUSAGE: opt_enc plaintext.txt keygen.txt port_number", 1);

    /* Check that the files are readable and contain valid characters */
    if (!isValidFile(argv[1]))
        error(" The file could not be opened.", 1);
    if (!isValidFile(argv[2]))
        error(" The file could not be opened.", 1);

    /******************** BEGIN SOCKET STUFF *************************
     
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent *serverHostInfo;
    char buffer[256];

    // Check usage & args
    if (argc < 3)
    {
        fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
        exit(0);
    }

    // Set up the server address struct
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[2]);                                  // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET;                          // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber);                  // Store the port number
    serverHostInfo = gethostbyname(argv[1]);                     // Convert the machine name into a special form of address
    if (serverHostInfo == NULL)
    {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0)
        error("CLIENT: ERROR opening socket");

    // Connect to server
    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
        error("CLIENT: ERROR connecting");

    // Get input message from user
    printf("CLIENT: Enter text to send to the server, and then hit enter: ");
    memset(buffer, '\0', sizeof(buffer));     // Clear out the buffer array
    fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
    buffer[strcspn(buffer, "\n")] = '\0';     // Remove the trailing \n that fgets adds

    // Send message to server
    charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
    if (charsWritten < 0)
        error("CLIENT: ERROR writing to socket");
    if (charsWritten < strlen(buffer))
        printf("CLIENT: WARNING: Not all data written to socket!\n");

    // Get return message from server
    memset(buffer, '\0', sizeof(buffer));                      // Clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
    if (charsRead < 0)
        error("CLIENT: ERROR reading from socket");
    printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

    close(socketFD); // Close the socket
    
    ******************** END SOCKET STUFF *************************/

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
