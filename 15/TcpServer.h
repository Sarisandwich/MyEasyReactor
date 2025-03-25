#pragma once
#include"EventLoop.h"
#include"Acceptor.h"
#include"Connection.h"

class TcpServer
{
private:
    EventLoop _loop;
    Acceptor* _acceptor; //一个TcpServer只有一个Acceptor对象
public:
    TcpServer(const std::string& ip, const uint16_t port);
    ~TcpServer();

    void start();

    void newconnection(MySocket* clientsock);           //处理新客户端连接
};