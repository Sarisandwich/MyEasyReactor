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
    sleep(1000);
    printf("开始时间：%ld\n", time(0));

    for(int i=0; i<10; ++i)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "这是第%d个测试报文。", i);

        char tmpbuf[1024];  //临时的buffer，报文头部+报文内容
        memset(tmpbuf, 0, sizeof(tmpbuf));
        int len=strlen(buf);    //计算报文的大小
        memcpy(tmpbuf, &len, 4);    //拼接报文头部
        memcpy(tmpbuf+4, buf, len); //拼接报文内容

        if(send(sockfd, tmpbuf, len+4, 0)<=0)
        {
            printf("send() failed.\n"); close(sockfd); return -1;
        }
    // }
    // for(int i=0; i<1; ++i)
    // {
        //int len;
        recv(sockfd, &len, 4, 0);   //先读取4字节的报文头部
        memset(buf, 0, sizeof(buf));
        if(recv(sockfd, buf, len, 0)<=0)
        {
            printf("recv() failed.\n"); close(sockfd); return -1;
        }

        printf("recv: %s\n", buf);
        sleep(1);
    }

    printf("结束时间：%ld\n", time(0));

    sleep(2);
}