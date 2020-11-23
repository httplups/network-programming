#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

int Socket(int family, int type, int flags) {
	int sockfd;
	if((sockfd = socket(family, type, flags)) < 0){
		perror("socket");
		exit(1);
	}
	else
		return sockfd;
}

void Listen(int fd, int backlog) {
	char *ptr;

	if ((ptr = getenv("LISTENQ")) != NULL) 
		backlog = atoi(ptr);
	if (listen(fd,backlog) < 0) {
		perror("listen error");
		exit(1);
	}
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	if (bind(sockfd, addr, addrlen) == -1) {
		perror("bind");
		exit(1);
	}
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int connfd;
	if ((connfd = accept(sockfd, addr, addrlen)) == -1) {
		// possible error in using invalid socket
		perror("accept");
		exit(1);
	}
	else
		return connfd;
}

void GetSockName(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
     if (getsockname(sockfd, addr, addrlen) == -1) {
        perror("getsockname");
        exit(1);
    }
}

void GetPeerName(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {

     if (getpeername(sockfd, addr, addrlen) == -1) {
        perror("getpeername");
        exit(1);
    }
}

void Write(int fd, const void *buf, size_t count){ 
    ssize_t n = write(fd, buf, count); 
    if (n == -1) {
	perror("write");
	exit(1);
    }
    //printf("return value of write: %li\n", n);
}

int Read(int fd, void *buf, size_t count){ 
    ssize_t n;
    n = read(fd, buf, count);
    if (n < 0) {
        perror("read error");
        exit(1);
    }
    else 
        return n;
}

void Inet_pton(int af, const char *src, void *dst) {
    if (inet_pton(af, src, dst) <= 0) {
        perror("inet_pton error");
        exit(1);
    }
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (connect(sockfd, addr, addrlen) < 0) {
        perror("connect error");
        exit(1);
    }
}

void Close(int fd) {
    close(fd);
}

pid_t Fork() {
	pid_t pid;
	pid = fork();
	return pid;
}

void Shutdown(int socket, int how) {
	if (shutdown(socket, how) == -1) 
		perror("shutdown");
}

typedef void Sigfunc(int);
Sigfunc * Signal (int signo, Sigfunc *func) {
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset (&act.sa_mask); /* Outros sinais não são bloqueados*/
    act.sa_flags = 0;
    if (signo == SIGALRM) { /* Para reiniciar chamadas interrompidas */
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
#endif
    } else {
#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
#endif
    }
    if (sigaction (signo, &act, &oact) < 0)
        return (SIG_ERR);
    return (oact.sa_handler);
}