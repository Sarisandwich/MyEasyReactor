#pragma once
#include"MyEpoll.h"

class MyEpoll;
class Channel;

class EventLoop
{
private:
    MyEpoll* ep_;   //每个事件循环只有一个epoll
    std::function<void(EventLoop*)> epolltimeoutcallback_;   //epoll_wait()超时的回调函数，将回调TcpServer::epolltimeout
public:
    EventLoop();    //在构造函数中创建epoll对象
    ~EventLoop();   //在析构函数中销毁

    void run();     //运行事件循环
    void updatechannel(Channel* ch);
    void setepolltimeoutcallback(std::function<void(EventLoop*)> func);
};