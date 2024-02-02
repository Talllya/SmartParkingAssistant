#define _GNU_SOURCE // some new things added from the standard C version

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h> // for function inet_addr(the program adress given from argv[1])

extern int errno;

// the port to connect to server
int port;

char *giveBackTheAvailablesLots(char theGivenString[])
{
  char theRefferenceSymbol = ':';
  char *theStringAfter = strchr(theGivenString, theRefferenceSymbol);
  char *theStringWithSpots = theStringAfter + 2;
  return theStringWithSpots;
}

char *giveBackTheAvailablesLotsCall(char theGivenString[])
{
  char theRefferenceSymbol = '+';
  char *theStringAfter = strchr(theGivenString, theRefferenceSymbol);
  char *theStringWithSpots = theStringAfter + 2;
  return theStringWithSpots;
}

int main(int argc, char *argv[])
{
  int sd;                             // socket descriptor
  struct sockaddr_in connectToServer; // the struct used to connect
  char theMessageBetween[100];        // the sent message

  if (argc != 3)
  {
    printf("The syntax: %s <server_adress> <port>\n", argv[0]);
    return -1;
  }

  // transform the received port from string to int
  port = atoi(argv[2]);

  // create the socket
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("C: Error at socket().\n");
    return errno;
  }

  // prepare the struct used to connect with server
  // socket family
  connectToServer.sin_family = AF_INET;
  // IP server adress
  connectToServer.sin_addr.s_addr = inet_addr(argv[1]);
  // connect port
  connectToServer.sin_port = htons(port);

  // connect to the server
  if (connect(sd, (struct sockaddr *)&connectToServer, sizeof(struct sockaddr)) == -1)
  {
    perror("C: Error at connect().\n");
    return errno;
  }

  while (1)
  {
    printf("\n----------------\n");
    printf("------MENU------\n");
    printf("----------------\n");
    printf("connect\n");
    printf("available parking spots\n");
    printf("choose spot x (where x is a number)\n");
    printf("call\n");
    printf("exit\n");

    // read message
    bzero(theMessageBetween, 100);
    printf("C: Type a command: ");
    fflush(stdout);
    read(0, theMessageBetween, 100);

    // send the message to the server
    if (write(sd, theMessageBetween, 100) <= 0)
    {
      perror("C: Error at write() to server.\n");
      return errno;
    }

    bzero(theMessageBetween, 100);
    // read the response received from server (blocking way till the server respond)
    int result = read(sd, theMessageBetween, 100);
    if (result < 0)
    {
      perror("C: Error at read() from server.\n");
      return errno;
    }

    // printf("%s\n", theMessageBetween);

    if (strstr(theMessageBetween, "exit") != NULL)
    {
      printf("C: Have a good day!\n");
      return 0;
    }
    else if (strstr(theMessageBetween, "connected") != NULL)
    {
      printf("C: Succesfully connected to the parking system.\n");
    }
    else if (strstr(theMessageBetween, "alreadyc") != NULL)
    {
      printf("C: Already connected to the parking system.\n");
    }
    else if (strstr(theMessageBetween, "available : ") != NULL)
    {
      printf("C: The available parking spots:\n");
      printf("%s\n", giveBackTheAvailablesLots(theMessageBetween));
    }
    else if (strstr(theMessageBetween, "call +") != NULL)
    {
      printf("C: The available parking spots:\n");
      printf("%s\n", giveBackTheAvailablesLotsCall(theMessageBetween));
      printf("C: To chose a spot, type command: choose spot x .\n");
    }
    else if (strstr(theMessageBetween, "rang") != NULL)
    {
      printf("C: Already have called.\n");
    }
    else if (strstr(theMessageBetween, "badluck") != NULL)
    {
      printf("C: There aren't any available parking spots. Try again later!\n");
    }
    else if (strstr(theMessageBetween, "thanks") != NULL)
    {
      printf("C: Thank you for your time!\n");
    }
    else if (strstr(theMessageBetween, "busy") != NULL)
    {
      printf("C: The spot is already taken. Chose an available one 'choose spot x'!\n");
    }
    else if (strstr(theMessageBetween, "impossible") != NULL)
    {
      printf("C: Already chose a parking spot.\n");
    }
    else if (strstr(theMessageBetween, "notc") != NULL)
    {
      printf("C: You are not connected. Try command 'connect'.\n");
    }
    else if (strstr(theMessageBetween, "unknown") != NULL)
    {
      printf("C: Unknown command. Try again!\n");
    }
  }

  close(sd);
}
