#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<time.h>

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: /client ip port\n"); return -1;
    }

    int sockfd;
    sockaddr_in servaddr;
    char buf[1024];

    if((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        perror("socket() failed.\n");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);

    if(connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))<0)
    {
        printf("connect(%s: %s) failed.\n", argv[1], argv[2]); close(sockfd); return -1;
    }

    printf("connected.\n");

    //printf("开始时间：%d", time(0));

    for(int i=0; i<100000; ++i)
    {
        memset(buf, 0, sizeof(buf));
        printf("input: "); scanf("%s", buf);

        if(send(sockfd, buf, strlen(buf), 0)<=0)
        {
            printf("send() failed.\n"); close(sockfd); return -1;
        }

        memset(buf, 0, sizeof(buf));
        if(recv(sockfd, buf, sizeof(buf), 0)<=0)
        {
            printf("recv() failed.\n"); close(sockfd); return -1;
        }

        printf("recv: %s\n", buf);
    }

    //printf("结束时间：%d", time(0));
}