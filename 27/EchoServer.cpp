#include"EchoServer.h"

// class EchoServer
// {
// private:
//     TcpServer tcpserver_;
// public:
//     EchoServer(const std::string& ip, const uint16_t port);
//     ~EchoServer();

//     void Start();   //启动服务

//     void HandleNewConnection(MySocket* clientsock);           //处理新客户端连接，在TcpServer类中回调此函数
//     void HandleClose(Connection* conn);             //关闭客户端连接，在TcpServer类中回调此函数
//     void HandleError(Connection* conn);             //客户端连接发生错误，在TcpServer类中回调此函数
//     void HandleMessage(Connection* conn, std::string message);  //处理报文，在TcpServer类中回调此函数

//     void HandleSendComplete(Connection* conn);    //发送完成之后的通知，在TcpServer类中回调此函数
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

//处理新客户端连接，在TcpServer类中回调此函数
void EchoServer::HandleNewConnection(Connection* conn)
{
    //printf("EchoServer::HandleNewConnection() thread is %ld\n", syscall(SYS_gettid));
    std::cout<<"New Connection Come in."<<std::endl;

    //可根据业务需求扩展代码
}

//关闭客户端连接，在TcpServer类中回调此函数
void EchoServer::HandleClose(Connection* conn)
{
    std::cout<<"EchoServer conn close."<<std::endl;

    //可根据业务需求扩展代码
}

//客户端连接发生错误，在TcpServer类中回调此函数
void EchoServer::HandleError(Connection* conn)
{
    std::cout<<"EchoServer conn error."<<std::endl;

    //可根据业务需求扩展代码
}        

//处理报文，在TcpServer类中回调此函数
void EchoServer::HandleMessage(Connection* conn, std::string& message)
{
    //printf("EchoServer::HandleMessage() thread is %ld\n", syscall(SYS_gettid));
    threadpool_.addtask(std::bind(&EchoServer::OnMessage, this, conn, message));
}

//处理报文
void EchoServer::OnMessage(Connection* conn, std::string& message)
{
    //此处对报文经过某些计算
    message="reply:"+message;
    conn->send(message.data(), message.size());   //把数据发送出去
}

//发送完成之后的通知，在TcpServer类中回调此函数
void EchoServer::HandleSendComplete(Connection* conn)
{
    std::cout<<"Message send complete."<<std::endl;

    //可根据业务需求扩展代码
}  

// //epoll_wait()超时，在TcpServer类中回调此函数
// void EchoServer::HandleTimeOut(EventLoop* loop)
// {
//     std::cout<<"EchoServer timeout."<<std::endl;

//     //可根据业务需求扩展代码
// }