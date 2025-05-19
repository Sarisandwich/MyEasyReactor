#include"Connection.h"

Connection::Connection(EventLoop* loop, std::unique_ptr<MySocket> clientsock)
            :loop_(loop), clientsock_(std::move(clientsock)), disconnected_(false), clientchannel_(new Channel(loop_, clientsock_->fd()))
{
    // clientchannel_=new Channel(loop_, clientsock_->fd());
}

void Connection::init() {
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback, this));
    clientchannel_->enablereading();
    clientchannel_->useet();
}


Connection::~Connection()
{
    // delete clientsock_;
    // delete clientchannel_;

    // printf("Connection已析构\n");
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
    disconnected_=true;
    clientchannel_->remove();
    closecallback_(shared_from_this());
}

void Connection::errorcallback()
{
    disconnected_=true;
    clientchannel_->remove();
    errorcallback_(shared_from_this());
}

void Connection::setclosecallback(std::function<void(spConnection)> func)
{
    closecallback_=func;
}

void Connection::seterrorcallback(std::function<void(spConnection)> func)
{
    errorcallback_=func;
}

void Connection::setonmessagecallback(std::function<void(spConnection, std::string&)> func)
{
    onmessagecallback_=func;
}

void Connection::setsendcompletecallback(std::function<void(spConnection)> func)
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
            std::string message;
            while(true)
            {
                if(inputbuffer_.pickmessage(message)==false) break;

                // printf("message(eventfd=%d): %s\n", fd(), message.c_str());
                lastatime_=Timestamp::now();
                // std::cout<<lastatime_.tostring()<<std::endl;

                onmessagecallback_(shared_from_this(), message);
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

//发送数据。无论在任何线程中，都是调用此函数发送数据
void Connection::send(const char* data, size_t size)
{
    if(disconnected_==true){printf("client disconnected. Connection::send() return.\n"); return;}
    if(loop_->isinloopthread()) //判断当前线程是否为事件循环线程(IO线程)
    {
        //如果当前线程是IO线程，直接执行发送数据的操作
        sendinloop(data, size);
    }
    else
    {
        //如果当前线程不是IO线程，调用EventLoop::enqueueloop(), 把enqueueloop()交给事件循环线程执行
        auto msg = std::make_shared<std::string>(data, size);
        loop_->enqueueloop(std::bind(&Connection::sendinloop_shared, this, msg));
    }
}

//发送数据。如果是IO线程，直接调用此函数，如果是工作线程，将此函数传给IO线程去执行
void Connection::sendinloop(const char* data, size_t size)
{
    outputbuffer_.appendwithsep(data, size);   //将数据添加到outputbuffer
    clientchannel_->enablewriting();    //注册写事件
}

void Connection::sendinloop_shared(std::shared_ptr<std::string> msg)
{
    sendinloop(msg->data(), msg->size());
}

void Connection::writecallback()
{
    int writen=::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if(writen>0) outputbuffer_.erase(0, writen);    //从outputbuffer删除已发送的字节数的数据
    //printf("Connection::writecallback() thread is %ld.\n", syscall(SYS_gettid));
    //如果发送缓冲区中没有数据了，表示数据已发送成功，不再关注写事件
    if(outputbuffer_.size()==0)
    {
        clientchannel_->disablewriting();
        sendcompletecallback_(shared_from_this());
    }
}

bool Connection::timeout(time_t now, int val)
{
    return now-lastatime_.toint()>val;
}