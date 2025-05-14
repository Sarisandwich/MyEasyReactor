#include"EchoServer.h"

// class EchoServer
// {
// private:
//     TcpServer tcpserver_;
// public:
//     EchoServer(const std::string& ip, const uint16_t port);
//     ~EchoServer();

//     void Start();   //启动服务

//     void HandleNewConnection(std::unique_ptr<MySocket> clientsock);           //处理新客户端连接，在TcpServer类中回调此函数
//     void HandleClose(spConnection conn);             //关闭客户端连接，在TcpServer类中回调此函数
//     void HandleError(spConnection conn);             //客户端连接发生错误，在TcpServer类中回调此函数
//     void HandleMessage(spConnection conn, std::string message);  //处理报文，在TcpServer类中回调此函数

//     void HandleSendComplete(spConnection conn);    //发送完成之后的通知，在TcpServer类中回调此函数
//     void HandleTimeOut(EventLoop* loop);     //epoll_wait()超时，在TcpServer类中回调此函数
// };


EchoServer::EchoServer(const std::string& ip, const uint16_t port, int subthreadnum, int workthreadnum)
            :tcpserver_(ip, port, subthreadnum), threadpool_(workthreadnum, "WORKS")
{
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    // tcpserver_.settimeoutcb(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{}

void EchoServer::Start()
{
    tcpserver_.start();
}

void EchoServer::Stop()
{
    //停止工作线程。
    threadpool_.stop();
    printf("工作线程已停止。\n");
    //停止IO线程（事件循环）。
    tcpserver_.stop();
}

//处理新客户端连接，在TcpServer类中回调此函数
void EchoServer::HandleNewConnection(spConnection conn)
{
    //printf("EchoServer::HandleNewConnection() thread is %ld\n", syscall(SYS_gettid));
    //std::cout<<"New Connection Come in."<<std::endl;
    printf("new connection(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());

    //可根据业务需求扩展代码
}

//关闭客户端连接，在TcpServer类中回调此函数
void EchoServer::HandleClose(spConnection conn)
{
    //std::cout<<"EchoServer conn close."<<std::endl;
    printf("connection(fd=%d, ip=%s, port=%d) closed.\n", conn->fd(), conn->ip().c_str(), conn->port());
    //可根据业务需求扩展代码
}

//客户端连接发生错误，在TcpServer类中回调此函数
void EchoServer::HandleError(spConnection conn)
{
    //std::cout<<"EchoServer conn error."<<std::endl;

    //可根据业务需求扩展代码
}        

//处理报文，在TcpServer类中回调此函数
void EchoServer::HandleMessage(spConnection conn, std::string& message)
{
    if(threadpool_.size()==0)
    {
        //如果没有工作线程，表示交给IO线程进行计算
        OnMessage(conn, message);
    }
    else
    {
        //将业务添加到线程池的任务队列里，交给工作线程处理业务
        threadpool_.addtask(std::bind(&EchoServer::OnMessage, this, conn, message));
    }
}

//处理报文
void EchoServer::OnMessage(spConnection conn, std::string& message)
{
    //此处对报文经过某些计算
    message="reply:"+message;
    conn->send(message.data(), message.size());   //把数据发送出去
}

//发送完成之后的通知，在TcpServer类中回调此函数
void EchoServer::HandleSendComplete(spConnection conn)
{
    //std::cout<<"Message send complete."<<std::endl;

    //可根据业务需求扩展代码
}  

// //epoll_wait()超时，在TcpServer类中回调此函数
// void EchoServer::HandleTimeOut(EventLoop* loop)
// {
//     std::cout<<"EchoServer timeout."<<std::endl;

//     //可根据业务需求扩展代码
// }