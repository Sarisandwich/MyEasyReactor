#pragma once
#include"EventLoop.h"
#include"Acceptor.h"
#include"Connection.h"
#include<map>

class TcpServer
{
private:
    EventLoop loop_;     //一个TcpServer可以有多个事件循环
    Acceptor* acceptor_; //一个TcpServer只有一个Acceptor对象
    std::map<int, Connection*> conns_;  //map存放管理connection指针

    std::function<void(Connection*)> newconnectioncb_;  //回调EchoServer::HandleNewConnection()
    std::function<void(Connection*)> closeconnectioncb_; //回调EchoServer::HandleClose()
    std::function<void(Connection*)> errorconnectioncb_; //回调EchoServer::HandleError()
    std::function<void(Connection*)> sendcompletecb_; //回调EchoServer::HandleSendComplete()
    std::function<void(Connection*, std::string &message)> onmessagecb_; //回调EchoServer::HandleMessage()
    std::function<void(EventLoop*)>timeoutcb_; //回调EchoServer::HandleTimeOut()

public:
    TcpServer(const std::string& ip, const uint16_t port);
    ~TcpServer();

    void start();

    void newconnection(MySocket* clientsock);           //处理新客户端连接，在Acceptor类中回调此函数
    void closeconnection(Connection* conn);             //关闭客户端连接，在Connection类中回调此函数
    void errorconnection(Connection* conn);             //客户端连接发生错误，在Connection类中回调此函数
    void onmessage(Connection* conn, std::string& message);  //处理报文，在Connection类中回调此函数
    void sendcomplete(Connection* conn);    //发送完成之后的通知，在Connection类中回调此函数
    void epolltimeout(EventLoop* loop);     //epoll_wait()超时，在Eventloop类中回调此函数

    void setnewconnectioncb(std::function<void(Connection*)> func);
    void setcloseconnectioncb(std::function<void(Connection*)> func);
    void seterrorconnectioncb(std::function<void(Connection*)> func);
    void setsendcompletecb(std::function<void(Connection*)> func);
    void setonmessagecb(std::function<void(Connection*, std::string &message)> func);
    void settimeoutcb(std::function<void(EventLoop*)> func);
};