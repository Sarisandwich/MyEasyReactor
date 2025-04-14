#pragma once
#include"TcpServer.h"
#include"EventLoop.h"
#include"Connection.h"

class EchoServer
{
private:
    TcpServer tcpserver_;
public:
    EchoServer(const std::string& ip, const uint16_t port, int subthreadnum=5);
    ~EchoServer();

    void Start();   //启动服务

    void HandleNewConnection(Connection* conn);           //处理新客户端连接，在TcpServer类中回调此函数
    void HandleClose(Connection* conn);             //关闭客户端连接，在TcpServer类中回调此函数
    void HandleError(Connection* conn);             //客户端连接发生错误，在TcpServer类中回调此函数
    void HandleMessage(Connection* conn, std::string& message);  //处理报文，在TcpServer类中回调此函数

    void HandleSendComplete(Connection* conn);    //发送完成之后的通知，在TcpServer类中回调此函数
    // void HandleTimeOut(EventLoop* loop);     //epoll_wait()超时，在TcpServer类中回调此函数
};
