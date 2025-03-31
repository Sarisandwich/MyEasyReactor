#include"Channel.h"



Channel::Channel(EventLoop* loop, int fd): loop_(loop), fd_(fd)
{}

Channel::~Channel() //      Channel析构函数不能摧毁ep和fd，因为只对它们进行使用，这两个是从外面传进来的
{}


int Channel::fd()                       //返回fd_成员
{
    return fd_;
}

void Channel::useet()                   //采用边缘触发
{
    events_=events_|EPOLLET;
}

void Channel::enablereading()           //让epoll_wait()监视fd_的读事件
{
    events_|=EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::setinepoll()             //将inepoll_成员的值设置为true
{
    inepoll_=true;
}

void Channel::setrevents(uint32_t ev)   //设置revents_成员的值为参数ev
{
    revents_=ev;
}

bool Channel::inepoll()                 //返回inepoll_成员
{
    return inepoll_;
}

uint32_t Channel::events()              //返回events_成员
{
    return events_;
}

uint32_t Channel::revents()             //返回revents_成员
{
    return revents_;
}

void Channel::handleevent()             //epoll_wait返回后执行，处理事件
{
    if(revents_&EPOLLRDHUP)    //Read Hang Up 对端关闭了连接
    {
        closecallback_();
    }
    else if(revents_&(EPOLLIN|EPOLLPRI))   //接收缓冲区有数据可读
    {
        readcallback_();
    }
    else if(revents_&EPOLLOUT) //有数据要写
    {

    }
    else    //其他，视为错误
    {
        errorcallback_();
    }
}

/*
//处理对端发送的数据
void Channel::onmessage()
{
    char buffer[1024];
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t nread=read(fd_, buffer, sizeof(buffer));
        if(nread>0) //成功读取数据
        {
            printf("recv(eventfd=%d): %s\n", fd_, buffer);
            send(fd_, buffer, strlen(buffer), 0);
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
            closecallback_();
            break;
        }
    }
}
*/

//设置回调函数
void Channel::setreadcallback(std::function<void()> func)
{
    readcallback_=func;
}

void Channel::setclosecallback(std::function<void()> func)
{
    closecallback_=func;
}

void Channel::seterrorcallback(std::function<void()> func)
{
    errorcallback_=func;
}
