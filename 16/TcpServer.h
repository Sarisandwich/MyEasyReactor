#pragma once
#include"EventLoop.h"
#include"Acceptor.h"
#include"Connection.h"
#include<map>

class TcpServer
{
private:
    EventLoop _loop;     //一个TcpServer可以有多个事件循环
    Acceptor* _acceptor; //一个TcpServer只有一个Acceptor对象
    std::map<int, Connection*> _conns;  //map存放管理connection指针
public:
    TcpServer(const std::string& ip, const uint16_t port);
    ~TcpServer();

    void start();

    void newconnection(MySocket* clientsock);           //处理新客户端连接
};