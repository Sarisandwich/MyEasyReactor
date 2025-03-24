#pragma once
#include"EventLoop.h"

class TcpServer
{
private:
    EventLoop _loop;
public:
    TcpServer(const std::string& ip, const uint16_t port);
    ~TcpServer();

    void start();
};