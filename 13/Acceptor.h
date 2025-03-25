#pragma once
#include<functional>
#include"MySocket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"

class Acceptor
{
private:
    EventLoop* _loop;           //acceptor对应的事件循环，在构造函数传入
    MySocket* _servsock;        //服务端用于监听的socket，在构造函数创建
    Channel* _acceptchannel;    //acceptor对应的channel，在构造函数创建
public:
    Acceptor(EventLoop* loop, const std::string& ip, const uint16_t port);
    ~Acceptor();
};