#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/fcntl.h>
#include<sys/epoll.h>
#include<sys/eventfd.h>

int main(int argc, char* argv[])
{
    int efd=eventfd(2,EFD_NONBLOCK|EFD_SEMAPHORE);   //创建eventfd

    uint64_t buf=3;
    ssize_t ret;

    ret=write(efd, &buf, sizeof(uint64_t));

    ret=read(efd, &buf, sizeof(uint64_t));
    ret=read(efd, &buf, sizeof(uint64_t));
    ret=read(efd, &buf, sizeof(uint64_t));
    ret=read(efd, &buf, sizeof(uint64_t));
    ret=read(efd, &buf, sizeof(uint64_t));
    ret=read(efd, &buf, sizeof(uint64_t));

    printf("ret=%ld, buf=%ld\n", ret, buf);

    close(efd);
    return 0;
}