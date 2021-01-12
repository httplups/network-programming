#include "mysockfunctions.h"
// #define FD_SETSIZE 10
#include <sys/time.h>       /* for struct timeval {} */

int n_users = 0;

typedef struct {
    char ip[15];
    int port;
    int socket_conn;
    int available;
    int points;
} Client;

Client clients[FD_SETSIZE];

void initialize_clients() {
    int i;
    for(i=0; i< FD_SETSIZE; i++) {
        strcpy(clients[i].ip, "NULL");
        clients[i].port = -1;
        clients[i].socket_conn = -1; /* -1 indicates available entry */
        clients[i].available = 1;
        clients[i].points = 0;
    }
}

void show_clients() {
    printf("\nID\tIP\t\tPort\tFree\tSocket\tPoints\n");
    int i;
    for(i=0; i< FD_SETSIZE; i++) {
        if (clients[i].socket_conn > 0)
            // printf("%d -\t%s\n",i,clients[i].username);
            printf("%d\t%s\t%d\t%d\t%d\t%d\n", i, clients[i].ip, clients[i].port, clients[i].available, clients[i].socket_conn, clients[i].points);
    }
}

int get_index_by_port(int port) {
    int i;
    for (i = 0; i < FD_SETSIZE; i++) {
        if(clients[i].port == port)
            break;
    }
    return i;
}

char * show_other_players(int socket) {
    char info_players[MAXDATASIZE];
    char * i_ptr = info_players;
    char aux[MAXDATASIZE];

    memset(info_players, 0, strlen(info_players));
    memset(aux, 0, strlen(aux));
    
    // printf("\nID\tIP\tPort\n");
    int i;
    for(i=0; i< FD_SETSIZE; i++) {
        // printf("%d %d\n", clients[i].socket_conn, socket);
        if ((clients[i].socket_conn > 0) && (clients[i].socket_conn != socket) && (clients[i].available == 1)) {
            snprintf(aux, sizeof(aux),"%d\t%s\t%d\n", i, clients[i].ip, clients[i].port);
            strcat(info_players, aux);
        }
    }

    // printf("1 - %s\n", info_players);
    if ( strlen(info_players) == 0) 
        snprintf(info_players, sizeof(info_players),"NULL");

    info_players[strlen(info_players)] = 0;
    // printf("2 - %s\n", info_players);
    return i_ptr;
}

char * get_info_player(int pos) {
    char info[10];
    char * i_ptr = info;

    snprintf(info, sizeof(info),"%d",clients[pos].port);
    return i_ptr;
}

int insert_client_socket(int socket) {
    int i;
    for (i = 0; i < FD_SETSIZE; i++) {
        /* Add client at the first avaiable position */
        if (clients[i].socket_conn < 0) {
            clients[i].socket_conn = socket;
            break;
        }
    }
    printf("New client inserted at %d\n", i);
    /*The client's info is still empty */
    return i;
}

void set_as_offline(int pos) {
    clients[pos].available = 0;
}

void set_as_online(int pos) {
    clients[pos].available = 1;
}

int  insert_client(char * ip, int port, int socket) {
    int i;
    for (i = 0; i < FD_SETSIZE; i++) {
        /* Add client at the first avaiable position */
        if (clients[i].socket_conn < 0) {
            strcpy(clients[i].ip, ip);
            clients[i].port = port;
            clients[i].socket_conn = socket;
            break;
        }
    }

    printf("(%s:%d) inserted at %d\n", ip, port, i);

    return i;
}



int main(int argc, char **argv) {
    int     i, maxi, maxfd, listenfd, connfd, sockfd, port;
    long timeout = 1;
    int     nready;
    ssize_t n;
    fd_set  rset, allset;
    char    buf[MAXLINE], resp[MAXLINE], otherclient[100], player[100];
    socklen_t  clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct timeval selTimeout;       /* Timeout for select() */

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
    
    
    for ( ; ; ) {
        FD_ZERO(&allset);
        FD_SET(listenfd, &allset);

        /* SETA NOVAMENTE OS DESCRITORES QUE TEM CONEXAO ATIVA*/
        for (i = 0; i < FD_SETSIZE; i++) {
            if (clients[i].socket_conn !=  -1) {
                FD_SET(clients[i].socket_conn, &allset);
            }
        }



        selTimeout.tv_sec = timeout;       /* timeout (secs.) */
        selTimeout.tv_usec = 0;            /* 0 microseconds */
        
        rset = allset;          /* structure assignment */

        // show_clients();


        if((nready = Select(maxfd + 1, &rset, NULL, NULL, &selTimeout)) == 0) 
            // printf("No echo requests for %ld secs...Server still alive\n", timeout);
            continue;
        else {

        
            if (FD_ISSET(listenfd, &rset)) {       /* new client connection */
                clilen = sizeof(cliaddr);
                connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
                GetPeerName(connfd, (struct sockaddr *)&cliaddr, &clilen);
                i = insert_client(inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port), connfd);
                if (i == FD_SETSIZE) {
                    printf("too many clients");
                    exit(1);
                }

                FD_SET(connfd, &allset);       /* add new descriptor to set */

                if (connfd > maxfd)
                    maxfd = connfd; /* for select */
                if (i > maxi)
                    maxi = i;          /* max index in client[] array */

                if (--nready <= 0)
                    continue;          /* no more readable descriptors */
                
            }

        
            for (i = 0; i <= maxi; i++) {       /* check all clients for data */


                if ( (sockfd = clients[i].socket_conn) < 0) /* check connection socket for each client */
                    continue;

                if (FD_ISSET(sockfd, &rset)) {

                    // printf("I got something at %d\n", sockfd);
                    memset(buf, 0, strlen(buf));
                    memset(player, 0, strlen(player));
                    if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
                        /* connection closed by client */
                        Close(sockfd);
                        FD_CLR(sockfd, &allset);
                        strcpy(clients[i].ip, "NULL");
                        clients[i].port = -1;
                        clients[i].socket_conn = -1; /* -1 indicates available entry */
                        clients[i].available = 0;
                        clients[i].points = 0;

                    } 
                    else{
                        printf("got %s from client: %d\n", buf, clients[i].port);

                        if (strcmp(buf, "get") == 0) {
                            /* send the players available */
                            memset(resp, 0, strlen(resp));
                            memset(otherclient, 0, strlen(otherclient));

                            strcpy(resp, show_other_players(sockfd));
                            printf("resp: %s\n\n", resp);
                            Write(sockfd, resp, strlen(resp));
                        }
                        else if (strcmp(buf, "won") == 0) {

                            clients[i].points++;
                            /* Put the client again on the array */
                            set_as_online(i);
                            show_clients();
                        }
                        else if (strcmp(buf, "lose") == 0) {

                            /* Put the client again on the array */
                            set_as_online(i);
                            show_clients();
                        }
                        else if (strcmp(buf, "playing") == 0) {

                            /* Getting the port number of the player playing with client i */
                            // Read(sockfd, player, 100);
                            // printf("porta player:%s\n", player);
                            set_as_offline(i);
                            // set_as_offline(get_index_by_port(atoi(player)));
                            show_clients();
                        }
                        else if (strcmp(buf, "points") == 0) {
                            continue;
                        }
                        else {

                            /* BUF must be the port number that was chosen to play */

                            if (atoi(buf) == -1)
                                continue;  

                            /* Tell the player of port number BUF that the clients[i] wants to play with it 
                                and only read messages from its port number
                            */

                            char* player_port_str = integer_to_string(clients[i].port);
                            Write(clients[get_index_by_port(atoi(buf))].socket_conn, player_port_str, strlen(player_port_str));                       

                            // /* Sending port of client */
                            // memset(resp, 0, strlen(resp));
                            // strcpy(resp, get_info_player(atoi(buf)));
                            // printf("resp %s\n", resp);
                            // Write(sockfd, resp, strlen(resp));

            //                 // set_as_offline(i);
            //                 // set_as_offline(otherclient);
                        }
            //             // Write(sockfd, buf, n);
                    }                   
                    
                    
                    if (--nready <= 0)
                        break;         /* no more readable descriptors */
                }
            } 
        }
    }
}   