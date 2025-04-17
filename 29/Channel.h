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
    int fd_=-1; //Channel拥有的fd，Channel和fd是一对一的关系
    EventLoop* loop_=nullptr;   //epoll与loop一对一绑定
    bool inepoll_=false;  //Channel是否已添加到epoll树上，如果未添加，调用epoll_ctl()时用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD
    uint32_t events_=0; //fd_需要监视的事件
    uint32_t revents_=0; //fd_已发生的事件

    std::function<void()> readcallback_; //fd_读事件的回调函数
    std::function<void()> closecallback_; //关闭fd_的回调函数
    std::function<void()> errorcallback_; //fd_出现错误的回调函数
    std::function<void()> writecallback_; //fd_写事件的回调函数
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    int fd();                       //返回fd_成员
    void useet();                   //采用边缘触发
    void enablereading();           //让epoll_wait()监视fd_的读事件，注册读事件
    void disablereading();          //取消读事件
    void enablewriting();           //注册写事件
    void disablewriting();          //取消写事件
    void disableall();              //取消全部事件
    void remove();                  //删除channel

    void setinepoll();              //将inepoll_成员的值设置为true
    void setrevents(uint32_t ev);   //设置revents_成员的值为参数ev
    bool inepoll();                 //返回inepoll_成员
    uint32_t events();              //返回events_成员
    uint32_t revents();             //返回revents_成员

    void handleevent();             //epoll_wait返回后执行，处理事件
    //void newconnection(MySocket* servsock);           //处理新客户端连接
    //void onmessage();                                 //处理对端发送的数据

    //设置回调函数
    void setreadcallback(std::function<void()> func); 
    void setclosecallback(std::function<void()> func);
    void seterrorcallback(std::function<void()> func);
    void setwritecallback(std::function<void()> func);
};