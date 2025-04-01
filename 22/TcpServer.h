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
public:
    TcpServer(const std::string& ip, const uint16_t port);
    ~TcpServer();

    void start();

    void newconnection(MySocket* clientsock);           //处理新客户端连接，在Acceptor类中回调此函数
    void closeconnection(Connection* conn);             //关闭客户端连接，在Connection类中回调此函数
    void errorconnection(Connection* conn);             //客户端连接发生错误，在Connection类中回调此函数
    void onmessage(Connection* conn, std::string message);  //处理报文，在Connection类中回调此函数

    void sendcomplete(Connection* conn);    //发送完成之后的通知，在Connection类中回调此函数
    void epolltimeout(EventLoop* loop);     //epoll_wait()超时，在Eventloop类中回调此函数
};