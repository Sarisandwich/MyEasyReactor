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
#include<netinet/tcp.h>
#include"InetAddress.h"
#include"MySocket.h"


int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: ./tcpepoll ip port\n"); return -1;
    }
    /*
    //创建服务端用于监听的socket
    int listenfd=socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
    if(listenfd<0)
    {
        perror("socket() failed."); return -1;
    }

    //设置listenfd的属性
    int opt=1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof(opt)));
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof(opt)));

    InetAddress servaddr(argv[1], atoi(argv[2]));

    if(bind(listenfd, servaddr.addr(), sizeof(sockaddr))<0)
    {
        perror("bind() failed."); close(listenfd); return -1;
    }

    if(listen(listenfd, 128)!=0)
    {
        perror("listen() failed."); close(listenfd); return -1;
    }
    */

    //创建epoll句柄
    MySocket servsock(createnonblocking());
    InetAddress servaddr(argv[1], atoi(argv[2]));
    servsock.setkeepalive(true);
    servsock.setreuseaddr(true);
    servsock.setreuseport(true);
    servsock.settcpnodelay(true);
    servsock.bind(servaddr);
    servsock.listen();


    int epollfd=epoll_create(1);

    //为listenfd准备读事件
    epoll_event ev;
    ev.data.fd=servsock.fd();
    ev.events=EPOLLIN;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock.fd(), &ev);   //将监视的listenfd和事件加入epoll

    epoll_event evs[10];

    while(true)
    {
        int infds=epoll_wait(epollfd, evs, 10, -1);

        if(infds<0)
        {
            perror("epoll_wait() failed."); break;
        }
        if(infds==0)
        {
            perror("epoll_wait() timeout."); continue;
        }

        for(int i=0;i<infds; ++i){
            if(evs[i].events&EPOLLRDHUP)    //Read Hang Up 对端关闭了连接
            {
                printf("client(fd=%d) disconnected.\n", evs[i].data.fd);
                close(evs[i].data.fd);
            }
            else if(evs[i].events&(EPOLLIN|EPOLLPRI))   //接收缓冲区有数据可读
            {
                if(evs[i].data.fd==servsock.fd())    //listenfd有事件，有新的客户端连接
                {
                    // sockaddr_in peeraddr;
                    // socklen_t len=sizeof(peeraddr);
                    // int clientfd=accept4(listenfd, (sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);

                    InetAddress clientaddr; //客户端的地址与协议

                    MySocket* clientsock=new MySocket(servsock.accept(clientaddr));

                    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

                    //为新客户端连接准备读事件，添加到epoll
                    ev.data.fd=clientsock->fd();
                    ev.events=EPOLLIN|EPOLLET;
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock->fd(), &ev);
                }
                else    //客户端fd有事件
                {
                    char buffer[1024];
                    while(true)
                    {
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t nread=read(evs[i].data.fd, buffer, sizeof(buffer));
                        if(nread>0) //成功读取数据
                        {
                            printf("recv(eventfd=%d): %s\n", evs[i].data.fd, buffer);
                            send(evs[i].data.fd, buffer, strlen(buffer), 0);
                        }
                        else if(nread<0&&errno==EINTR)  //读取数据时被信号中断，继续提取
                        {
                            continue;
                        }
                        else if(nread<0&&(errno==EAGAIN||errno==EWOULDBLOCK))   //已读取完数据，退出循环
                        {
                            break;
                        }
                        else if(nread==0)   //客户端已断开连接
                        {
                            printf("client(eventfd=%d) disconnected.\n", evs[i].data.fd);
                            close(evs[i].data.fd);
                            break;
                        }
                    }
                }
            }
            else if(evs[i].events&EPOLLOUT) //有数据要写
            {

            }
            else    //其他，视为错误
            {
                    printf("client(eventfd=%d) error.\n", evs[i].data.fd);
                    close(evs[i].data.fd);
            }
        }
    }
}