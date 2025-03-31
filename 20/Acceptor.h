#pragma once
#include<functional>
#include"MySocket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"

class Acceptor
{
private:
    EventLoop* loop_;           //acceptor对应的事件循环，在构造函数传入
    MySocket* servsock_;        //服务端用于监听的socket，在构造函数创建
    Channel* acceptchannel_;    //acceptor对应的channel，在构造函数创建
    std::function<void(MySocket*)> newconnectioncb_;
public:
    Acceptor(EventLoop* loop, const std::string& ip, const uint16_t port);
    ~Acceptor();

    void newconnection();           //处理新客户端连接
    void setnewconnectioncb(std::function<void(MySocket*)> func);
};