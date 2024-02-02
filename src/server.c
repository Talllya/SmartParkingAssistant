#define _GNU_SOURCE // some new things added from the standard C version

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sqlite3.h>
#include "baza.h"

// the used port
#define PORT 2025
#define MAX_CLIENTS 10

struct responseToClient
{
    char responseMessage[100];
};

int clientCount = 0; // the number of connected clients
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

struct ThreadArgs
{
    int client;
    pthread_t thread_id;
};

int giveBackTheChosenSpot(char theGivenString[])
{
    char theRefferenceSymbol = 't';
    char *theStringAfter = strchr(theGivenString, theRefferenceSymbol);
    char *theStringWithSpot = theStringAfter + 2;
    return atoi(theStringWithSpot);
}

void *treat(void *givenArg)
{
    struct ThreadArgs *args = (struct ThreadArgs *)givenArg;

    int client = args->client;
    pthread_t thread_id = args->thread_id;

    free(givenArg);
    int active = 1;
    int connected = 0;
    int chosenSpot = 0;
    int seedParamOff = 0;
    int call = 0;

    initDb(seedParamOff);
    // we realise the treatment for client
    while (active)
    {
        struct responseToClient respToClient;
        char msgFromClient[100];

        printf("S: Thread ID for client %d: %ld\n", client, thread_id);
        bzero(msgFromClient, 100);
        printf("S: We wait for the message...\n");
        fflush(stdout);

        // reading the message from client
        if (read(client, msgFromClient, 100) <= 0)
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            perror("S: Error at read() from client.\n");
            break;
        }

        printf("S: Thread ID for client %d: %ld\n", client, thread_id);
        printf("S: The message was received...%s\n", msgFromClient);

        // prepare the message for client
        bzero(&respToClient, sizeof(struct responseToClient));

        if (strstr(msgFromClient, "connect") != NULL && connected == 0)
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            connected = 1;
            strcpy(respToClient.responseMessage, "connected");
            printf("S: Succesfully connected to the parking system.\n");
        }
        else if (strstr(msgFromClient, "connect") != NULL && connected == 1)
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            connected = 1;
            strcpy(respToClient.responseMessage, "alreadyc");
            printf("S: Already connected to the parking system.\n");
        }
        else if (connected == 0)
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            strcpy(respToClient.responseMessage, "notc");
            printf("S: The client isn't connected to the system.\n");
        }
        else if (strstr(msgFromClient, "available parking spots") != NULL && connected == 1)
        {
            char message_back[200] = "available : ";
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            struct AvailableSpots spots = getAvailableParkingSpots();
            if (spots.length == 0)
            {
                strcpy(respToClient.responseMessage, "badluck");
                printf("S: There aren't any available parking spots.\n");
            }
            else
            {
                strncat(message_back, theStringWithTheSpots(spots), strlen(theStringWithTheSpots(spots)));
                strcpy(respToClient.responseMessage, message_back);
                printf("S: Sent back the available parking spots.\n");
            }
        }
        else if (strstr(msgFromClient, "choose spot") != NULL && connected == 1 && chosenSpot == 0 && call == 1)
        {
            struct SpotData spots = getSpotData(giveBackTheChosenSpot(msgFromClient));
            if (spots.status == 0)
            {
                printf("S: Thread ID for client %d: %ld\n", client, thread_id);
                updateLotStatus(giveBackTheChosenSpot(msgFromClient), 2);
                chosenSpot = 1;
                strcpy(respToClient.responseMessage, "thanks");
                printf("S: A new spot was broned.\n");
            }
            else
            {
                printf("S: Thread ID for client %d: %ld\n", client, thread_id);
                strcpy(respToClient.responseMessage, "busy");
                printf("S: The spot was already taken.\n");
            }
        }
        else if (strstr(msgFromClient, "choose spot") != NULL && connected == 1 && chosenSpot == 0)
        {
            struct SpotData spots = getSpotData(giveBackTheChosenSpot(msgFromClient));
            if (spots.status == 0)
            {
                printf("S: Thread ID for client %d: %ld\n", client, thread_id);
                updateLotStatus(giveBackTheChosenSpot(msgFromClient), 2);
                chosenSpot = 1;
                strcpy(respToClient.responseMessage, "thanks");
                printf("S: A new spot was broned.\n");
            }
            else
            {
                printf("S: Thread ID for client %d: %ld\n", client, thread_id);
                strcpy(respToClient.responseMessage, "busy");
                printf("S: The spot was already taken.\n");
            }
        }
        else if (strstr(msgFromClient, "choose spot") != NULL && connected == 1 && chosenSpot == 1)
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            strcpy(respToClient.responseMessage, "impossible");
            printf("S: A spot was already chosen.\n");
        }
        else if (strstr(msgFromClient, "call") != NULL && connected == 1 && call == 0 && chosenSpot == 0)
        {
            call = 1;
            char message_back[200] = "call + ";
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            struct AvailableSpots spots = getAvailableParkingSpots();
            if (spots.length == 0)
            {
                strcpy(respToClient.responseMessage, "badluck");
                printf("S: There aren't any available parking spots.\n");
            }
            else
            {
                strncat(message_back, theStringWithTheSpots(spots), strlen(theStringWithTheSpots(spots)));
                strcpy(respToClient.responseMessage, message_back);
                printf("S: Sent back the spots after the call.\n");
            }
        }
        else if (strstr(msgFromClient, "call") != NULL && connected == 1 && call == 0 && chosenSpot == 1)
        {
            call = 1;
            strcpy(respToClient.responseMessage, "impossible");
            printf("S: A spot was already chosen.\n");
        }
        else if (strstr(msgFromClient, "call") != NULL && connected == 1 && call == 1)
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            strcpy(respToClient.responseMessage, "rang");
            printf("S: Already called to the parking system.\n");
        }
        else if (strstr(msgFromClient, "exit") != NULL)
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            connected = 0;
            active = 0;
            strcpy(respToClient.responseMessage, "exit");
            printf("S: Closing this thread.\n");
        }
        else
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            strcpy(respToClient.responseMessage, "unknown");
            printf("S: Unknown command. Try again!\n");
        }

        printf("S: Thread ID for client %d: %ld\n", client, thread_id);
        printf("S: We send the message back to the client...%s\n", respToClient.responseMessage);

        printf("%s\n", respToClient.responseMessage);
        // send the message back to the client
        if (write(client, respToClient.responseMessage, strlen(respToClient.responseMessage)) == -1)
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            perror("S: Error at write() to the client.\n");
            break;
        }
        else
        {
            printf("S: Thread ID for client %d: %ld\n", client, thread_id);
            printf("S: The message was sent with succes.\n");
        }
    }
    closeDb();

    // we're done with this client and close the connection
    close(client);
    pthread_exit(NULL);
}

void threadCreate(int client)
{
    pthread_t tid;
    int *clientCopy = malloc(sizeof(int));
    *clientCopy = client;

    struct ThreadArgs *args = malloc(sizeof(struct ThreadArgs));
    args->client = client;
    args->thread_id = tid;

    if (pthread_create(&tid, NULL, treat, (void *)args) != 0)
    {
        perror("S: Error at pthread_create().\n");
        close(client);
    }
    pthread_detach(tid);
}

int main()
{
    struct sockaddr_in server; // the struct used by server
    struct sockaddr_in from;
    int sd; // socket descriptor

    // create a socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("S: Error at socket().\n");
        return errno;
    }
    // use option SO_REUSEADDR
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // get ready the data structures
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    // prepare the struct used by server
    // establish family of sockets
    server.sin_family = AF_INET;
    // accept any adress
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    // utilise an user port
    server.sin_port = htons(PORT);

    // atach the socket
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("S: Error at bind().\n");
        return errno;
    }

    // the server listen if any clients try to connect
    if (listen(sd, MAX_CLIENTS) == -1)
    {
        perror("S: Error at listen().\n");
        return errno;
    }

    // serve concurrently the clients
    while (1)
    {
        int client;
        int length = sizeof(from);

        printf("S: We wait at the port %d...\n", PORT);
        fflush(stdout); // for freeing the output buffer

        // accept a client (blocking way till the connect realise)
        client = accept(sd, (struct sockaddr *)&from, &length);

        if (client < 0)
        {
            perror("S: Error at accept().\n");
            continue;
        }

        // Create an execution thread to treat the client
        threadCreate(client);
    }
    return 0;
}
