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

//设置非阻塞模式
void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_NONBLOCK);
}

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: ./tcpepoll ip port\n"); return -1;
    }

    //创建服务端用于监听的socket
    int listenfd=socket(AF_INET, SOCK_STREAM, 0);
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

    //设置非阻塞模式
    setnonblocking(listenfd);

    sockaddr_in servaddr;
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);
    servaddr.sin_port=htons(atoi(argv[2]));

    if(bind(listenfd, (sockaddr*)&servaddr, static_cast<socklen_t>(sizeof(servaddr)))<0)
    {
        perror("bind() failed."); close(listenfd); return -1;
    }

    if(listen(listenfd, 128)!=0)
    {
        perror("listen() failed."); close(listenfd); return -1;
    }

    //创建epoll句柄
    int epollfd=epoll_create(1);

    //为listenfd准备读事件
    epoll_event ev;
    ev.data.fd=listenfd;
    ev.events=EPOLLIN;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);   //将监视的listenfd和事件加入epoll

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
            if(evs[i].data.fd==listenfd)    //服务端有事件
            {
                sockaddr_in clientaddr;
                socklen_t len=sizeof(clientaddr);
                int clientfd=accept(listenfd, (sockaddr*)&clientaddr, &len);
                setnonblocking(clientfd);   //客户端必须设置为非阻塞模式

                printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientfd, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

                //为新客户端连接准备读事件，添加到epoll
                ev.data.fd=clientfd;
                ev.events=EPOLLIN|EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
            }
            else    //客户端有事件
            {
                if(evs[i].events&EPOLLRDHUP)    //Read Hang Up 对端关闭了连接
                {
                    printf("client(fd=%d) disconnected.\n", evs[i].data.fd);
                    close(evs[i].data.fd);
                }
                else if(evs[i].events&(EPOLLIN|EPOLLPRI))   //接收缓冲区有数据可读
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
}