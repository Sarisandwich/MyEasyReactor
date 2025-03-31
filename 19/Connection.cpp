#include"Connection.h"

Connection::Connection(EventLoop* loop, MySocket* clientsock):loop_(loop), clientsock_(clientsock)
{
    clientchannel_=new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->enablereading();
    clientchannel_->useet();
}

Connection::~Connection()
{
    delete clientsock_;
    delete clientchannel_;
}

int Connection::fd() const
{
    return clientsock_->fd();
}

std::string Connection::ip() const
{
    return clientsock_->ip();
}

uint16_t Connection::port() const
{
    return clientsock_->port();
}

void Connection::closecallback()
{
    closecallback_(this);
}

void Connection::errorcallback()
{
    errorcallback_(this);
}

void Connection::setclosecallback(std::function<void(Connection*)> func)
{
    closecallback_=func;
}

void Connection::seterrorcallback(std::function<void(Connection*)> func)
{
    errorcallback_=func;
}

void Connection::onmessage()
{
    char buffer[1024];
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        ssize_t nread=read(fd(), buffer, sizeof(buffer));
        if(nread>0) //成功读取数据
        {
            // printf("recv(eventfd=%d): %s\n", fd(), buffer);
            // send(fd(), buffer, strlen(buffer), 0);
            inputbuffer_.append(buffer, nread);
        }
        else if(nread<0&&errno==EINTR)  //读取数据时被信号中断，继续提取
        {
            continue;
        }
        else if(nread<0&&(errno==EAGAIN||errno==EWOULDBLOCK))   //已读取完数据，退出循环
        {
            printf("recv(eventfd=%d): %s\n", fd(), inputbuffer_.data());
            //将inputbuffer_里的数据经过某些计算，再存到outputbuffer_，这里先什么都不做
            outputbuffer_=inputbuffer_;
            inputbuffer_.clear();
            send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
            break;
        }
        else if(nread==0)   //客户端已断开连接
        {
            closecallback();
            break;
        }
    }
}