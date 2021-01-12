#include <stdio.h>      /* for printf() and fprintf()*/
#include <stdlib.h>   //for atoi()
#include <sys/time.h>       /* for struct timeval {} */
#include <fcntl.h>       /* for fcntl() */
#include <sys/socket.h> /* for accept() ,socket(), bind(), and connect(),recv() and send() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */


void DieWithError(char *errorMessage);  /* Error handling function */
void HandleTCPClient(int clntSocket);   /* TCP client handling function */
int CreateTCPServerSocket(unsigned short port); /* Create TCP server socket */
int AcceptTCPConnection(int servSock);  /* Accept TCP connection request */


int main(int argc, char *argv[])
{
    int *servSock;                   /* Socket descriptors for server */
    int maxDescriptor;               /* Maximum socket descriptor value */
    fd_set sockSet;                  /* Set of socket descriptors for select() */
    long timeout;                    /* Timeout value given on command-line */
    struct timeval selTimeout;       /* Timeout for select() */
    int running = 1;                 /* 1 if server should be running; 0 otherwise */
    int noPorts;                     /* Number of port specified on command-line */
    int port;                        /* Looping variable for ports */
    unsigned short portNo;           /* Actual port number */

    if (argc < 3)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s ...\n", argv[0]);
        exit(1);
    }

    timeout = atol(argv[1]);        /* First arg: Timeout */
    noPorts = argc - 2;             /* Number of ports is argument count minus 2 */

    /* Allocate list of sockets for incoming connections */
    servSock = (int *) malloc(noPorts * sizeof(int));
    /* Initialize maxDescriptor for use by select() */
    maxDescriptor = -1;
 
    /* Create list of ports and sockets to handle ports */
    for (port = 0; port < noPorts; port++)
    {
        /* Add port to port list */
        portNo = atoi(argv[port + 2]);  /* Skip first two arguments */

        /* Create port socket */
        servSock[port] = CreateTCPServerSocket(portNo);

        /* Determine if new descriptor is the largest */
        if (servSock[port] > maxDescriptor)
            maxDescriptor = servSock[port];
    }

    printf("Starting server:  Hit return to shutdown\n");
    while (running)
    {
        /* Zero socket descriptor vector and set for server sockets */
        /* This must be reset every time select() is called */
        FD_ZERO(&sockSet);
        /* Add keyboard to descriptor vector */
        FD_SET(STDIN_FILENO, &sockSet);
        for (port = 0; port < noPorts; port++)
            FD_SET(servSock[port], &sockSet);

        /* Timeout specification */
        /* This must be reset every time select() is called */
        selTimeout.tv_sec = timeout;       /* timeout (secs.) */
        selTimeout.tv_usec = 0;            /* 0 microseconds */

        /* Suspend program until descriptor is ready or timeout */
        if (select(maxDescriptor + 1, &sockSet, NULL, NULL, &selTimeout) == 0)
            printf("No echo requests for %ld secs...Server still alive\n", timeout);
        else
        {
            if (FD_ISSET(0, &sockSet)) /* Check keyboard */
            {
                printf("Shutting down server\n");
                getchar();
                running = 0;
            }

            for (port = 0; port < noPorts; port++)
                if (FD_ISSET(servSock[port], &sockSet))
                {
                    printf("Request on port %d:  ", port);
                    HandleTCPClient(AcceptTCPConnection(servSock[port]));
                }
        }
    }

    /* Close sockets */
    for (port = 0; port < noPorts; port++)
        close(servSock[port]);

    /* Free list of sockets */
    free(servSock);

    exit(0);
}

int AcceptTCPConnection(int servSock)
{
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */

    /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);
   
    /* Wait for a client to connect */
    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,
           &clntLen)) < 0)
        DieWithError("accept() failed");
   
    /* clntSock is connected to a client! */
   
    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    return clntSock;
}

int CreateTCPServerSocket(unsigned short port)
{
    int sock;                        /* socket to create */
    struct sockaddr_in echoServAddr; /* Local address */

    /* Create socket for incoming connections */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
     
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(port);              /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(sock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    return sock;
}
void HandleTCPClient(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize, i;                    /* Size of received message */

    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0)      /* zero indicates end of transmission */
    {
         for( i = 0; echoBuffer[ i ]; i++)
    {
        if( echoBuffer[i]=='a' ||  echoBuffer[i]=='e' ||  echoBuffer[i]=='i' || echoBuffer[i]=='o' ||  echoBuffer[i]=='u' )
                echoBuffer[ i ] = toupper(  echoBuffer[ i ] );
        else
            echoBuffer[ i ] = echoBuffer[ i ] ;
               
    }
        /* Echo message back to client */
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            DieWithError("send() failed");

        /* See if there is more data to receive */
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            DieWithError("recv() failed");
    }

    close(clntSocket);    /* Close client socket */
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

/*Client.c */

#include  stdio.h      /* for printf() and fprintf() */
#include  sys/socket.h /* for socket(), connect(), send(), and recv() */
#include  arpa/inet.h  /* for sockaddr_in and inet_addr() */
#include  stdlib.h     /* for atoi() and exit() */
#include  string.h     /* for memset() */
#include  unistd.h     /* for close() */

#define RCVBUFSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv()
                                        and total bytes read */

    if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s []\n",
               argv[0]);
       exit(1);
    }

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    echoString = argv[2];         /* Second arg: string to echo */

    if (argc == 4)
        echoServPort = atoi(argv[3]); /* Use given port, if any */
    else
        echoServPort = 7;  /* 7 is the well-known port for the echo service */

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

    echoStringLen = strlen(echoString);          /* Determine input length */

    /* Send the string to the server */
    if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
        DieWithError("send() sent a different number of bytes than expected");

    /* Receive the same string back from the server */
    totalBytesRcvd = 0;
    printf("Received: ");                /* Setup to print the echoed string */
    while (totalBytesRcvd < echoStringLen)
    {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        echoBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
        printf("%s", echoBuffer);      /* Print the echo buffer */
    }

    printf("\n");    /* Print a final linefeed */

    close(sock);
    exit(0);
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}