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
Function: encrypt
Description: Takes a given string and encrypts it using a key.
Input: string, key
Output: string
**************************/

int main()
{
    char characters[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    int i;
    for (i = 0; i < 27; i++)
    {
        printf("%c\n", intToChar(i));
    }
    printf("%c\n", intToChar(-1));

    return 0;
}
