/*******************************
Christopher Ragasa
OSU - CS 344, Program 4 (OTP)
11/30/18
Program Description: 

OTP is a system that will encypt and decrypt information using a one-time pad-like system.
This specific program (Keygen) is 1 of the 5 small programs that make up the entire system. 
Keygen creates a key file of specified length. The characters in the file generated will be any of the 27
allowed characters, generated using the standard UNIX randomization methods.  
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

int main(int argc, char *argv[])
{
    // First check for the correct number of arguments using argc
    if (argc != 2)
    {
        fprintf(stderr, "Error: Incorrect number of arguments\n");
        exit(0);
    }

    srand(time(NULL)); // Seed random

    char characters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; // Array of characters

    // Convert char count argument from string to integer
    int keyLength;
    keyLength = atoi(argv[1]);

    // Create the random key
    int i;
    for (i = 0; i < keyLength; i++)
    {
        int randInt;
        randInt = rand() % 27;                      // Return a random integer between 0 - 26
        fprintf(stdout, "%c", characters[randInt]); // Print random character to stdout
    }

    fprintf(stdout, "\n"); // Add newline character
    return 0;
}
