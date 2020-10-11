#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

char *recvMessage(int fd)
{
    char *msg;
    uint32_t netLen;
    int recvLen = recv(fd, &netLen, sizeof(netLen), 0);
    if (recvLen != sizeof(netLen)) {
        fprintf(stderr, "recv got invalid length value (got %d)\n", recvLen);
        exit(1);
    }
    int len = ntohl(netLen);
    msg = malloc(len + 1);
    if (recv(fd, msg, len, 0) != len) {
        fprintf(stderr, "recv got invalid length msg\n");
        exit(1);
    }
    msg[len] = '\0';

    return msg;
}

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket()");
        return 1;
    }
    int opt_enable = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt_enable, sizeof(opt_enable));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind()");
        return 1;
    }

    if (listen(fd, 10) == -1) {
        perror("listen()");
        return 1;
    }

    struct sockaddr clientaddr;
    socklen_t clientaddr_len;

    int clientfd = accept(fd, &clientaddr, &clientaddr_len);
    if (clientfd == -1) {
        perror("accept()");
        return 1;
    }

    // Do stuff with socket
    // char *msg1 = recvMessage(clientfd);
    // printf("msg1: %s\n", msg1);
    // char *msg2 = recvMessage(clientfd);
    // printf("msg2: %s\n", msg2);

    //int le = (clientaddr);

    //printf("%d\n",le);

    for (int i; i < 2; i++) /////////NEED TO FIND A WAY TO FIND OUT MANY SETS OF DATA IS BEING SENT, CURRENTLY SET TO 2
    {
        char *msg = recvMessage(clientfd);
        printf("msg1: %s\n", msg);
    }

    // free(msg1);
    // free(msg2);

    if (shutdown(clientfd, SHUT_RDWR) == -1) {
        perror("shutdown()");
        return 1;
    }

    close(clientfd);

    close(fd);
}
