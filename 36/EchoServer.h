#pragma once
#include"TcpServer.h"
#include"EventLoop.h"
#include"Connection.h"

class EchoServer
{
private:
    TcpServer tcpserver_;
    ThreadPool threadpool_;
public:
    EchoServer(const std::string& ip, const uint16_t port, int subthreadnum=5, int workthreadnum=5);
    ~EchoServer();

    void Start();   //启动服务。
    void Stop();    //停止服务。

    void HandleNewConnection(spConnection conn);           //处理新客户端连接，在TcpServer类中回调此函数
    void HandleClose(spConnection conn);             //关闭客户端连接，在TcpServer类中回调此函数
    void HandleError(spConnection conn);             //客户端连接发生错误，在TcpServer类中回调此函数
    void HandleMessage(spConnection conn, std::string& message);  //处理报文，在TcpServer类中回调此函数

    void HandleSendComplete(spConnection conn);    //发送完成之后的通知，在TcpServer类中回调此函数
    // void HandleTimeOut(EventLoop* loop);     //epoll_wait()超时，在TcpServer类中回调此函数

    void OnMessage(spConnection conn, std::string& message);  //处理报文
};
