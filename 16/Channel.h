#pragma once
#include<sys/epoll.h>
#include<functional>
#include"MySocket.h"
#include"InetAddress.h"
#include"EventLoop.h"

class EventLoop;

class Channel
{
private:
    int _fd=-1; //Channel拥有的fd，Channel和fd是一对一的关系
    EventLoop* _loop=nullptr;   //epoll与loop一对一绑定
    bool _inepoll=false;  //Channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()时用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD
    uint32_t _events=0; //_fd需要监视的事件
    uint32_t _revents=0; //_fd已发生的事件

    std::function<void()> _readcallback; //_fd读事件的回调函数
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    int fd();                       //返回_fd成员
    void useet();                   //采用边缘触发
    void enablereading();           //让epoll_wait()监视_fd的读事件
    void setinepoll();              //将_inepoll成员的值设置为true
    void setrevents(uint32_t ev);   //设置_revents成员的值为参数ev
    bool inepoll();                 //返回_inepoll成员
    uint32_t events();              //返回_events成员
    uint32_t revents();             //返回_revents成员

    void handleevent();             //epoll_wait返回后执行，处理事件
    //void newconnection(MySocket* servsock);           //处理新客户端连接
    void onmessage();                                 //处理对端发送的数据

    void setreadcallback(std::function<void()> func); //设置回调函数
};