#pragma once
#include"MyEpoll.h"
#include<memory>
#include<unistd.h>
#include<sys/syscall.h>
#include<queue>
#include<mutex>
#include<map>
#include<functional>
#include<sys/eventfd.h>
#include<sys/timerfd.h>
#include"Connection.h"

class MyEpoll;
class Channel;
class Connection;
using spConnection=std::shared_ptr<Connection>;

class EventLoop
{
private:
    std::unique_ptr<MyEpoll> ep_;   //每个事件循环只有一个epoll
    std::function<void(EventLoop*)> epolltimeoutcallback_;   //epoll_wait()超时的回调函数，将回调TcpServer::epolltimeout
    pid_t threadid_;    //事件循环所在的线程id。
    std::queue<std::function<void()>> taskqueue_;   //事件循环线程被eventfd唤醒后执行的任务队列
    std::mutex mtx_;    //任务队列同步的互斥锁。

    int timetvl_;   //闹钟间隔时间。单位：秒。
    int timeout_;   //Connection对象超时的时间。单位：秒。
    int wakeupfd_;  //用于唤醒事件循环线程的eventfd。
    std::unique_ptr<Channel> wakechannel_;  //eventfd的Channel。
    int timerfd_;   //定时器的fd。
    std::unique_ptr<Channel> timerchannel_; //定时器的channel。

    std::map<int, spConnection> conns_; //用于存放该事件循环的所有connection对象。
    std::function<void(int)> timercallback_;    //回调函数。删除TcpServer中超时的connection对象。将回调TcpServer::removeconn()。
    std::mutex mmtx_;   //维护conns_的互斥锁。

    bool ismainloop_;    //true为主事件循环，false为从事件循环。
public:
    EventLoop(bool ismainloop, int timetvl=30, int timeout=80);    //在构造函数中创建epoll对象
    ~EventLoop();   //在析构函数中销毁

    void run();     //运行事件循环
    void updatechannel(Channel* ch);    //把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件
    void removechannel(Channel* ch);    //从红黑树删除channel
    void setepolltimeoutcallback(std::function<void(EventLoop*)> func);
    bool isinloopthread();  //判断当前线程是否为事件循环线程
    void enqueueloop(std::function<void()> func); //将任务添加到队列中

    void wakeup();  //用eventfd唤醒事件循环线程
    void handlewakeup();    //事件循环线程被eventfd唤醒后执行的函数

    void handletimer();     //闹钟响时执行的函数

    void newconnection(spConnection conn);  //把connection对象存入conns_中。
    void settimercallback(std::function<void(int)> func);    //设置回调函数timercallback_。
};