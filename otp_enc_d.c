#define h_addr h_addr_list[0]
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

void error(char *);			//Prototype

//This is the server
int main(int argc, char * argv[]){
	//declare variables
	int portNo, textLength, keyLength, sockfd, newsockfd, nText, nKey, pID, checkSockSending = 1, checkSockRecieving = 0;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	char textArray[2048];
	char keyArray[2048];
	char cipherArray[2048];
	socklen_t cliLength;

	if(argc < 2){											
		error("There are not enough arguments provided");	//if there are not enough arguments error and exit
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);				//set up the socket file descriptor
	portNo = atoi(argv[1]);									//set up the port number
	if(sockfd < 0){											//If the socket fd is 0 then error and exit
		error("Error opening the socket");
	}
	bzero((char *) &serverAddress, sizeof(serverAddress));	//Clear the address
	serverAddress.sin_family = AF_INET;						//set up the address
	serverAddress.sin_addr.s_addr = INADDR_ANY;				//set up the address
	serverAddress.sin_port = htons(portNo);					//set up the address
	if(bind(sockfd,(struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
		error("ERROR on binding");							//If can't bind error and exit
	}
	listen(sockfd,5);										//5 concurrent connections
	cliLength = sizeof(clientAddress);						//set the length of the client address`

	while(1){												//infinite loop
		newsockfd = accept(sockfd, (struct sockaddr *) &clientAddress, &cliLength);	//set up a new socket fd
		if(newsockfd < 0){									//if can't set up socket then error
			error("ERROR on accept");
		}
		pID = fork();										//create child
		if(pID < 0){										//if the child process is 0 then error
			error("Error forking");
		}else if( pID == 0){									   //enter the child
			read(newsockfd, &checkSockRecieving, sizeof(checkSockRecieving));
			if(checkSockRecieving == 0){
				write(newsockfd, &checkSockRecieving, sizeof(checkSockRecieving));
			}else{
				write(newsockfd, &checkSockSending, sizeof(checkSockSending));
				exit(1);
			}
			close(sockfd);
			//this is the child
			bzero(textArray, 2047);							//set the buffer size for the text array
			nText = read(newsockfd, textArray, 2047);		//read the text data from otp_enc
			if(nText < 0){									//if its 0 then no data could be read
				error("Error reading from socket");
			}
			bzero(keyArray, 2047);							//set the buffer size for key array
			nKey = read(newsockfd, keyArray, 2047);			//read the key data from otp_enc
			if(nKey < 0){									//if its 0 then no data could be read
				error("Error reading from socket");
			}
			for(int x = 0; x < strlen(textArray)-1; x++){	//for loop for the length of the array
				int textNumber, keyNumber, cipherNumber;
				
				//Finding the value of the text array at that position
				switch(textArray[x]){
					case 'A': textNumber = 0; break;
					case 'B': textNumber = 1; break;
					case 'C': textNumber = 2; break;
					case 'D': textNumber = 3; break;
					case 'E': textNumber = 4; break;
					case 'F': textNumber = 5; break;
					case 'G': textNumber = 6; break;
					case 'H': textNumber = 7; break;
					case 'I': textNumber = 8; break;
					case 'J': textNumber = 9; break;
					case 'K': textNumber = 10; break;
					case 'L': textNumber = 11; break;
					case 'M': textNumber = 12; break;
					case 'N': textNumber = 13; break;
					case 'O': textNumber = 14; break;
					case 'P': textNumber = 15; break;
					case 'Q': textNumber = 16; break;
					case 'R': textNumber = 17; break;
					case 'S': textNumber = 18; break;
					case 'T': textNumber = 19; break;
					case 'U': textNumber = 20; break;
					case 'V': textNumber = 21; break;
					case 'W': textNumber = 22; break;
					case 'X': textNumber = 23; break;
					case 'Y': textNumber = 24; break;
					case 'Z': textNumber = 25; break;
					case ' ': textNumber = 26; break;
					default:
						printf("There is an invalid character in the file!\n");	//if there was an invalid character used in file 5
						exit(1);	
				}
				
				//key file value
				switch(keyArray[x]){
					case 'A': keyNumber = 0; break;
					case 'B': keyNumber = 1; break;
					case 'C': keyNumber = 2; break;
					case 'D': keyNumber = 3; break;
					case 'E': keyNumber = 4; break;
					case 'F': keyNumber = 5; break;
					case 'G': keyNumber = 6; break;
					case 'H': keyNumber = 7; break;
					case 'I': keyNumber = 8; break;
					case 'J': keyNumber = 9; break;
					case 'K': keyNumber = 10; break;
					case 'L': keyNumber = 11; break;
					case 'M': keyNumber = 12; break;
					case 'N': keyNumber = 13; break;
					case 'O': keyNumber = 14; break;
					case 'P': keyNumber = 15; break;
					case 'Q': keyNumber = 16; break;
					case 'R': keyNumber = 17; break;
					case 'S': keyNumber = 18; break;
					case 'T': keyNumber = 19; break;
					case 'U': keyNumber = 20; break;
					case 'V': keyNumber = 21; break;
					case 'W': keyNumber = 22; break;
					case 'X': keyNumber = 23; break;
					case 'Y': keyNumber = 24; break;
					case 'Z': keyNumber = 25; break;
					case ' ': keyNumber = 26; break;
				}

				cipherNumber = keyNumber + textNumber;		//find the new value for the cipher
				if(cipherNumber > 26){						//if greater than 26 subtract 27 to make it valid again
					cipherNumber = cipherNumber - 27;
				}
				
				//Take the cipher number and make it a character again
				switch(cipherNumber){
					case 0: cipherArray[x] = 'A'; break;
					case 1: cipherArray[x] = 'B'; break;
					case 2: cipherArray[x] = 'C'; break;
					case 3: cipherArray[x] = 'D'; break;
					case 4: cipherArray[x] = 'E'; break;
					case 5: cipherArray[x] = 'F'; break;
					case 6: cipherArray[x] = 'G'; break;
					case 7: cipherArray[x] = 'H'; break;
					case 8: cipherArray[x] = 'I'; break;
					case 9: cipherArray[x] = 'J'; break;
					case 10: cipherArray[x] = 'K'; break;
					case 11: cipherArray[x] = 'L'; break;
					case 12: cipherArray[x] = 'M'; break;
					case 13: cipherArray[x] = 'N'; break;
					case 14: cipherArray[x] = 'O'; break;
					case 15: cipherArray[x] = 'P'; break;
					case 16: cipherArray[x] = 'Q'; break;
					case 17: cipherArray[x] = 'R'; break;
					case 18: cipherArray[x] = 'S'; break;
					case 19: cipherArray[x] = 'T'; break;
					case 20: cipherArray[x] = 'U'; break;
					case 21: cipherArray[x] = 'V'; break;
					case 22: cipherArray[x] = 'W'; break;
					case 23: cipherArray[x] = 'X'; break;
					case 24: cipherArray[x] = 'Y'; break;
					case 25: cipherArray[x] = 'Z'; break;
					case 26: cipherArray[x] = ' '; break;	
				}
			}
			nText = write(newsockfd, cipherArray, 2047);	//return the encrypted text to otp_enc
			if(nText < 0){									//if 0 then there was a problem writing the data
				error("ERROR writing to socket");
			}
			close(newsockfd);								//close the socket
			close(sockfd);									//close the socket
			exit(0);										//child dies
		}else{
			close(newsockfd);								//parent closes the new socket
		}
	}
   	return 0;
}

void error(char *msg){
	printf("%s\n", msg);									//print error message
	exit(1);												//exit
}
