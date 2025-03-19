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
#include"MyEpoll.h"


int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: ./tcpepoll ip port\n"); return -1;
    }

    
    MySocket servsock(createnonblocking());
    InetAddress servaddr(argv[1], atoi(argv[2]));
    servsock.setkeepalive(true);
    servsock.setreuseaddr(true);
    servsock.setreuseport(true);
    servsock.settcpnodelay(true);
    servsock.bind(servaddr);
    servsock.listen();
    /*
    //创建epoll句柄
    int epollfd=epoll_create(1);

    //为listenfd准备读事件
    epoll_event ev;
    ev.data.fd=servsock.fd();
    ev.events=EPOLLIN;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, servsock.fd(), &ev);   //将监视的listenfd和事件加入epoll

    epoll_event evs[10];
    */

    MyEpoll ep;
    ep.addfd(servsock.fd(), EPOLLIN);   //epoll监视listenfd的读事件，水平触发

    std::vector<epoll_event> evs;   //存放epoll_wait()返回的事件

    while(true)
    {
        /*
        int infds=epoll_wait(epollfd, evs, 10, -1);

        if(infds<0)
        {
            perror("epoll_wait() failed."); break;
        }
        if(infds==0)
        {
            perror("epoll_wait() timeout."); continue;
        }
        */

        evs=ep.loop();

        for(auto& ev:evs)
        {
            if(ev.events&EPOLLRDHUP)    //Read Hang Up 对端关闭了连接
            {
                printf("client(fd=%d) disconnected.\n", ev.data.fd);
                close(ev.data.fd);
            }
            else if(ev.events&(EPOLLIN|EPOLLPRI))   //接收缓冲区有数据可读
            {
                if(ev.data.fd==servsock.fd())    //listenfd有事件，有新的客户端连接
                {

                    InetAddress clientaddr; //客户端的地址与协议

                    MySocket* clientsock=new MySocket(servsock.accept(clientaddr));

                    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

                    //为新客户端连接准备读事件，添加到epoll
                    // ev.data.fd=clientsock->fd();
                    // ev.events=EPOLLIN|EPOLLET;
                    // epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock->fd(), &ev);
                    ep.addfd(clientsock->fd(), EPOLLIN|EPOLLET);
                }
                else    //客户端fd有事件
                {
                    char buffer[1024];
                    while(true)
                    {
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t nread=read(ev.data.fd, buffer, sizeof(buffer));
                        if(nread>0) //成功读取数据
                        {
                            printf("recv(eventfd=%d): %s\n", ev.data.fd, buffer);
                            send(ev.data.fd, buffer, strlen(buffer), 0);
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
                            printf("client(eventfd=%d) disconnected.\n", ev.data.fd);
                            close(ev.data.fd);
                            break;
                        }
                    }
                }
            }
            else if(ev.events&EPOLLOUT) //有数据要写
            {

            }
            else    //其他，视为错误
            {
                    printf("client(eventfd=%d) error.\n", ev.data.fd);
                    close(ev.data.fd);
            }
        }
    }
}