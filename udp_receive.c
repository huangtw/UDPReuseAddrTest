#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int createSocket(int port)
{
    int fd;
    struct sockaddr_in local_addr;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket open failed!\n");
        exit(1);
    }

    int sock_opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_opt, sizeof(sock_opt)) < 0) {
        printf("setsockopt failed!\n");
        exit(1);
    }

    bzero((char *) &local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = ntohl(INADDR_ANY);
    local_addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        printf("bind failed\n");
    }

    return fd;
}

void* worker(void* pfd)
{
    int fd = *(int*)pfd;
    char buffer[1024];

    while (1)
    {
        if (recv(fd, buffer, sizeof(buffer) - 1, 0) < 0)
        {
            printf("udp recv failed!\n");
            exit(1);
        }
        printf("socket %d received: %s\n", fd, buffer);
        //cout << "socket " << fd << " received:" << buffer << endl;
    }
}

int main(int argc, char* argv[])
{
    int *pfd, localPort, peerPort;
    char *peerIP;
    char buffer[1024];
    struct sockaddr_in local_addr, peer_addr;

    if (4 != argc)
    {
        exit(1);
    }

    localPort = atoi(argv[1]);
    peerIP = argv[2];
    peerPort = atoi(argv[3]);

    int i = 0;
    while (1)
    {
        pfd = malloc(sizeof(int));
        *pfd = createSocket(localPort);
        printf("socket %d opened\n", *pfd);

        if ((i >= 3) && (i < 6))
        {
            bzero((char *) &peer_addr, sizeof(peer_addr));
            peer_addr.sin_family = AF_INET;
            peer_addr.sin_addr.s_addr = inet_addr(peerIP);
            peer_addr.sin_port = htons(peerPort);

            if (connect(*pfd , (struct sockaddr *)&peer_addr , sizeof(peer_addr)) < 0)
            {
                printf("udp connect failed!\n");
                exit(1);
            }
        }

        pthread_t workerThread;
        pthread_create(&workerThread, NULL, worker, pfd);
        pthread_detach(workerThread);
        getchar();
        i++;
    }

    return 0;
}
