#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int fd;
    int localPort, peerPort;
    char *peerIP;
    struct sockaddr_in local_addr, peer_addr;
    char buffer[1024];

    if (4 != argc)
    {
        exit(1);
    }

    localPort = atoi(argv[1]);
    peerIP = argv[2];
    peerPort = atoi(argv[3]);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket open failed!\n");
        exit(1);
    }

    bzero((char *) &local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = ntohl(INADDR_ANY);
    local_addr.sin_port = htons(localPort);

    if (bind(fd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        printf("bind failed\n");
    }

    bzero((char *) &peer_addr, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr(peerIP);
    peer_addr.sin_port = htons(peerPort);

    if (connect(fd , (struct sockaddr *)&peer_addr , sizeof(peer_addr)) < 0)
    {
        printf("udp connect failed!\n");
        exit(1);
    }

    int i = 0;
    while(1)
    {
        int size,ss;
        int ssize = 0;

        size = sprintf(buffer, "%d\n", i++);
        sleep(2);
        while (ssize < size)
        {
            if ((ss = send(fd, buffer + ssize, size - ssize, 0)) < 0)
            {
                //cout << "send error" << endl;
                printf("send error\n");
                exit(1);
            }

            ssize += ss;
        }
    }

    return 0;
}
