#pragma once
#include"EventLoop.h"
#include"Acceptor.h"

class TcpServer
{
private:
    EventLoop _loop;
    Acceptor* _acceptor; //一个TcpServer只有一个Acceptor对象
public:
    TcpServer(const std::string& ip, const uint16_t port);
    ~TcpServer();

    void start();
};