#pragma once
#include"MyEpoll.h"
#include<memory>

class MyEpoll;
class Channel;

class EventLoop
{
private:
    std::unique_ptr<MyEpoll> ep_;   //每个事件循环只有一个epoll
    std::function<void(EventLoop*)> epolltimeoutcallback_;   //epoll_wait()超时的回调函数，将回调TcpServer::epolltimeout
public:
    EventLoop();    //在构造函数中创建epoll对象
    ~EventLoop();   //在析构函数中销毁

    void run();     //运行事件循环
    void updatechannel(Channel* ch);    //把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件
    void removechannel(Channel* ch);    //从红黑树删除channel
    void setepolltimeoutcallback(std::function<void(EventLoop*)> func);
};