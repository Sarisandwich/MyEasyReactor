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
    std::function<void(Connection*)> _closecallback;    //关闭_fd的回调函数，将回调TcpServer::closeconnection
    std::function<void(Connection*)> _errorcallback;    //_fd出现错误的回调函数，将回调TcpServer::errorconnection
public:
    Connection(EventLoop* loop, MySocket* clientsock);
    ~Connection();

    int fd() const; //返回_fd
    std::string ip() const;
    uint16_t port() const;
    void closecallback();
    void errorcallback();
    void setclosecallback(std::function<void(Connection*)> func);
    void seterrorcallback(std::function<void(Connection*)> func);
};