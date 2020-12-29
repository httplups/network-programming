#include "mysockfunctions.h"
// #define FD_SETSIZE 10

int n_users = 0;

typedef struct {
    char username[15];
    char ip[15];
    int port;
    int socket_conn;
} Client;

Client clients[FD_SETSIZE];

void initialize_clients() {
    int i;
    for(i=0; i< FD_SETSIZE; i++) {
        strcpy(clients[i].username, "NULL");
        strcpy(clients[i].ip, "NULL");
        clients[i].port = -1;
        clients[i].socket_conn = -1; /* -1 indicates available entry */
    }
}

void show_clients() {
    printf("ID\tUsername\tIP\nPort\n");
    int i;
    for(i=0; i< FD_SETSIZE; i++) {
        if (clients[i].socket_conn > 0)
            // printf("%d -\t%s\n",i,clients[i].username);
            printf("%d\t%s\t%s\t%d\n", i, clients[i].username, clients[i].ip, clients[i].port);
    }
}

int  insert_client(char *username, char * ip, int port, int socket) {
    int i;
    for (i = 0; i < FD_SETSIZE; i++) {

        /* Add client at the first avaiable position */
        if (clients[i].socket_conn < 0) {
            strcpy(clients[i].username,  username);
            strcpy(clients[i].ip, ip);
            clients[i].port = port;
            clients[i].socket_conn = socket;
            break;
        }
    }

    if (i == FD_SETSIZE) {
        printf("too many clients");
        exit(1);
    }

    printf("New user: %s\t%s\t%d\t%d\n", username, ip, port, socket);

    return i;
}

int main(int argc, char **argv) {
    int     i, maxi, maxfd, listenfd, connfd, sockfd, port;
    int     nready, client[FD_SETSIZE];
    ssize_t n;
    fd_set  rset, allset;
    char    buf[MAXLINE];
    char *welcome = "Welcome! If you want to join the game, enter your username:\n"; 
    char *menu = "Choose one option below:\n\n1 - Invite someone to play with\n3 - Quit\n";
    socklen_t  clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &port); // getting port number from argv
	servaddr.sin_port = htons(port);

    // servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    maxfd = listenfd;            /* initialize */
    maxi = -1;                   /* index into client[] array */
    // for (i = 0; i < FD_SETSIZE;  i++)
    //     client[i] = -1;          /* -1 indicates available entry */
    initialize_clients();
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for ( ; ; ) {
        rset = allset;          /* structure assignment */
        nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);
        
        if (FD_ISSET(listenfd, &rset)) {       /* new client connection */
            clilen = sizeof(cliaddr);
            // connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
            connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

            char username[15], otheruser[15];
            memset(username, 0, strlen(username));
            memset(otheruser, 0, strlen(otheruser));

            Write(connfd, welcome, strlen(welcome));
            Read(connfd, username, 15);
            username[strlen(username) -1] = 0;
            GetPeerName(connfd, (struct sockaddr *)&cliaddr, &clilen);
            i = insert_client(username, inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port), connfd);
            printf("i: %d\n", i);
            
            // for (i = 0; i < FD_SETSIZE; i++)
            //     if (client[i] < 0) {
            //         client[i] = connfd; /* save descriptor */
            //         break;
            //     }

            

            FD_SET(connfd, &allset);       /* add new descriptor to set */

            show_clients();
            
            if (connfd > maxfd)
                maxfd = connfd; /* for select */
            if (i > maxi)
                maxi = i;          /* max index in client[] array */

            if (--nready <= 0)
                continue;          /* no more readable descriptors */
            
        }
        
        // for (i = 0; i <= maxi; i++) {       /* check all clients for data */

        //     if ( (sockfd = client[i]) < 0) /* check connection socket for each client */
        //         continue;

        //     if (FD_ISSET(sockfd, &rset)) {
        //         if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
        //             /* connection closed by client */
        //             Close(sockfd);
        //             FD_CLR(sockfd, &allset);
        //             client[i] = -1;
        //         } 
        //         else                      
        //             Write(sockfd, buf, n);
                
        //         if (--nready <= 0)
        //             break;         /* no more readable descriptors */
        //     }
        // } 
    }
}   