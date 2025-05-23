#pragma once
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/tcp.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>

#include"InetAddress.h"

//MySocket类
class MySocket
{
private:
    const int fd_;      //存放fd，构造函数传入
    std::string ip_;    //存放ip，如果是listenfd，存放服务器监听的ip，如果是clientfd，存放对端的ip
    uint16_t port_;     //存放port，如果是listenfd，存放服务器监听的port，如果是clientfd，存放对端的port
public:
    MySocket(int fd);   //构造函数，传入fd
    ~MySocket();    //析构函数close这个fd

    int fd() const; //返回_fd
    std::string ip() const;
    uint16_t port() const;

    void setipport(const std::string& ip, uint16_t port);

    void setreuseaddr(bool on); //设置SO_REUSEADDR
    void setreuseport(bool on); //设置SO_REUSEPORT
    void settcpnodelay(bool on); //设置TCP_NODELAY
    void setkeepalive(bool on); //设置SO_KEEPALIVE
    void bind(const InetAddress& servaddr); //服务端的socket调用此函数
    void listen(int listeningnum=128); //服务端的socket调用此函数
    int accept(InetAddress& clientaddr); //服务端的socket调用此函数

    
};

int createnonblocking();