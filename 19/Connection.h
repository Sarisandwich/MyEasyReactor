#pragma once
#include<functional>
#include"MySocket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"
#include"Buffer.h"

class Connection
{
private:
    EventLoop* loop_;           //Connection对应的事件循环，在构造函数传入
    MySocket* clientsock_;      //与客户端通讯的Socket
    Channel* clientchannel_;    //Connection对应的channel，在构造函数创建
    Buffer inputbuffer_;         //接收缓冲区
    Buffer outputbuffer_;        //发送缓冲区

    std::function<void(Connection*)> closecallback_;    //关闭_fd的回调函数，将回调TcpServer::closeconnection
    std::function<void(Connection*)> errorcallback_;    //_fd出现错误的回调函数，将回调TcpServer::errorconnection
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

    void onmessage();
};