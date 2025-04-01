#include"Connection.h"

Connection::Connection(EventLoop* loop, MySocket* clientsock):loop_(loop), clientsock_(clientsock)
{
    clientchannel_=new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback, this));
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

void Connection::setonmessagecallback(std::function<void(Connection*, std::string)> func)
{
    onmessagecallback_=func;
}

void Connection::setsendcompletecallback(std::function<void(Connection*)> func)
{
    sendcompletecallback_=func;
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
            while(true)
            {
                /////////////////////////////////////////////////////////////////
                //以下代码可以封装到Buffer类中，还可以支持固定长度，指定报文长度和分隔符等多种格式
                int len;
                memcpy(&len, inputbuffer_.data(), 4);   //从inputbuffer中获取报文头部
                //如果inputbuffer中的数据量小于报文头部，说明inputbuffer中的报文内容不完整
                if(inputbuffer_.size()<len+4) break;

                std::string message(inputbuffer_.data()+4, len); //从inputbuffer中获取一个报文
                inputbuffer_.erase(0, len+4);       //从inputbuffer中删除刚刚已获取的报文
                /////////////////////////////////////////////////////////////////
                printf("message(eventfd=%d): %s\n", fd(), message.c_str());
                /*
                //将inputbuffer_里的数据经过某些计算，再存到outputbuffer_
                message="reply:"+message;
                len=message.size();
                std::string tmpbuf((char*)&len, 4);   //把报文头部填充到回应报文中
                tmpbuf.append(message);               //把报文内容填充到回应报文中
                send(fd(), tmpbuf.data(), tmpbuf.size(), 0);
                */
                onmessagecallback_(this, message);
            }
            break;
        }
        else if(nread==0)   //客户端已断开连接
        {
            closecallback();
            break;
        }
    }
}

void Connection::send(const char* data, size_t size)
{
    outputbuffer_.append(data, size);   //将数据添加到outputbuffer
    clientchannel_->enablewriting();    //注册写事件
}

void Connection::writecallback()
{
    printf("writecallback() called for fd=%d\n", fd());

    int writen=::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if(writen>0) outputbuffer_.erase(0, writen);    //从outputbuffer删除已发送的字节数的数据

    //如果发送缓冲区中没有数据了，表示数据已发送成功，不再关注写事件
    if(outputbuffer_.size()==0)
    {
        clientchannel_->disablewriting();
        sendcompletecallback_(this);
    }
}