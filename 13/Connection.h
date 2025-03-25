#pragma once
#include<functional>
#include"MySocket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"

class Connection
{
private:
    EventLoop* _loop;           //Connection对应的事件循环，在构造函数传入
    MySocket* _clientsock;      //与客户端通讯的Socket
    Channel* _clientchannel;    //Connection对应的channel，在构造函数创建
public:
    Connection(EventLoop* loop, MySocket* clientsock);
    ~Connection();
};