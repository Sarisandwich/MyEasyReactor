#pragma once
#include<functional>
#include"MySocket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"
#include"Buffer.h"
#include<memory>
#include<atomic>
#include<sys/syscall.h>

class Connection;
using spConnection=std::shared_ptr<Connection>;

class Connection: public std::enable_shared_from_this<Connection>
{
private:
    EventLoop* loop_;           //Connection对应的事件循环，在构造函数传入
    std::unique_ptr<MySocket> clientsock_;      //与客户端通讯的Socket
    std::unique_ptr<Channel> clientchannel_;    //Connection对应的channel，在构造函数创建
    Buffer inputbuffer_;         //接收缓冲区
    Buffer outputbuffer_;        //发送缓冲区
    std::atomic_bool disconnected_;  //判断是否已断开

    std::function<void(spConnection)> closecallback_;    //关闭_fd的回调函数，将回调TcpServer::closeconnection()
    std::function<void(spConnection)> errorcallback_;    //_fd出现错误的回调函数，将回调TcpServer::errorconnection()
    std::function<void(spConnection, std::string&)> onmessagecallback_;  //处理报文的回调函数，将回调TcpServer::onmessage()
    std::function<void(spConnection)> sendcompletecallback_;     //数据发送完成的回调函数，将回调TcpServer::sendcomplete()
public:
    Connection(EventLoop* loop, std::unique_ptr<MySocket> clientsock);
    ~Connection();

    int fd() const; //返回_fd
    std::string ip() const;
    uint16_t port() const;

    void init();//初始化。延迟绑定回调，因为构造函数里使用shared_from_this()是未定义行为。

    void onmessage();   //处理对端发送过来的报文
    void closecallback();
    void errorcallback();
    void writecallback();   //处理写事件的回调函数，供channel回调

    void setclosecallback(std::function<void(spConnection)> func);
    void seterrorcallback(std::function<void(spConnection)> func);
    void setonmessagecallback(std::function<void(spConnection, std::string&)> func);
    void setsendcompletecallback(std::function<void(spConnection)> func);


    void send(const char* data, size_t size);   //发送数据。无论在任何线程中，都是调用此函数发送数据
    void sendinloop(const char* data, size_t size); //发送数据。如果是IO线程，直接调用此函数，如果是工作线程，将此函数传给IO线程去执行
};