#include"Channel.h"



Channel::Channel(EventLoop* loop, int fd): _loop(loop), _fd(fd)
{}

Channel::~Channel() //      Channel析构函数不能摧毁ep和fd，因为只对它们进行使用，这两个是从外面传进来的
{}


int Channel::fd()                       //返回_fd成员
{
    return _fd;
}

void Channel::useet()                   //采用边缘触发
{
    _events=_events|EPOLLET;
}

void Channel::enablereading()           //让epoll_wait()监视_fd的读事件
{
    _events|=EPOLLIN;
    _loop->updatechannel(this);
}

void Channel::setinepoll()             //将_inepoll成员的值设置为true
{
    _inepoll=true;
}

void Channel::setrevents(uint32_t ev)   //设置_revents成员的值为参数ev
{
    _revents=ev;
}

bool Channel::inepoll()                 //返回_inepoll成员
{
    return _inepoll;
}

uint32_t Channel::events()              //返回_events成员
{
    return _events;
}

uint32_t Channel::revents()             //返回_revents成员
{
    return _revents;
}

void Channel::handleevent()             //epoll_wait返回后执行，处理事件
{
    if(_revents&EPOLLRDHUP)    //Read Hang Up 对端关闭了连接
    {
        printf("client(fd=%d) disconnected.\n", _fd);
        close(_fd);
    }
    else if(_revents&(EPOLLIN|EPOLLPRI))   //接收缓冲区有数据可读
    {
        _readcallback();
    }
    else if(_revents&EPOLLOUT) //有数据要写
    {

    }
    else    //其他，视为错误
    {
            printf("client(eventfd=%d) error.\n", _fd);
            close(_fd);
    }
}
/*
#include"Connection.h"

//处理新客户端连接
void Channel::newconnection(MySocket* servsock)
{
    InetAddress clientaddr; //客户端的地址与协议

    MySocket* clientsock=new MySocket(servsock->accept(clientaddr));

    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

    //为新客户端连接准备读事件，添加到epoll
    // ev.data.fd=clientsock->fd();
    // ev.events=EPOLLIN|EPOLLET;
    // epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock->fd(), &ev);
    //ep.addfd(clientsock->fd(), EPOLLIN|EPOLLET);
    Connection* conn=new Connection(_loop, clientsock);
}
*/

//处理对端发送的数据
void Channel::onmessage()
{
    char buffer[1024];
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t nread=read(_fd, buffer, sizeof(buffer));
        if(nread>0) //成功读取数据
        {
            printf("recv(eventfd=%d): %s\n", _fd, buffer);
            send(_fd, buffer, strlen(buffer), 0);
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
            printf("client(eventfd=%d) disconnected.\n", _fd);
            close(_fd);
            break;
        }
    }
}

//设置回调函数
void Channel::setreadcallback(std::function<void()> func)
{
    _readcallback=func;
}
