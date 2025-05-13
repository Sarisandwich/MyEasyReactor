#include"EventLoop.h"
#include"thread"
#include<iostream>
#include<errno.h>

int createtimerfd(int sec=30)
{
    int tfd=timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);  //创建timerfd。
    struct itimerspec timeout;  //定时时间的数据结构。
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec=sec;  //定时时间为5秒。
    timeout.it_value.tv_nsec=0;
    timerfd_settime(tfd, 0, &timeout, 0);

    return tfd;
}

EventLoop::EventLoop(bool ismainloop, int timetvl, int timeout):
            ep_(new MyEpoll()), ismainloop_(ismainloop), timetvl_(timetvl), timeout_(timeout),
            wakeupfd_(eventfd(0, EFD_NONBLOCK)), wakechannel_(new Channel(this, wakeupfd_)),
            timerfd_(createtimerfd(timeout_)), timerchannel_(new Channel(this, timerfd_))
{
    wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup, this));
    wakechannel_->enablereading();
    timerchannel_->setreadcallback(std::bind(&EventLoop::handletimer, this));
    timerchannel_->enablereading();
}

EventLoop::~EventLoop()
{
    // delete ep_;
}


void EventLoop::run()
{
    threadid_=syscall(SYS_gettid);
    //printf("EventLoop::run() thread is %ld\n", syscall(SYS_gettid));
    while(true)
    {
        std::vector<Channel*> channels=ep_->loop(10*1000);

        //如果channels为空，表示超时，回调TcpServer::epolltimeout
        if(channels.size()==0)
        {
            epolltimeoutcallback_(this);
        }
        else
        {
            for(auto& ch:channels)
            {
                ch->handleevent();
            }
        }
    }
}

void EventLoop::updatechannel(Channel* ch)
{
    ep_->updatechannel(ch);
}

void EventLoop::removechannel(Channel* ch)
{
    ep_->removechannel(ch);
}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> func)
{
    epolltimeoutcallback_=func;
}

bool EventLoop::isinloopthread()
{
    return threadid_==syscall(SYS_gettid);
}

//将任务添加到队列中
void EventLoop::enqueueloop(std::function<void()> func)
{
    {
        std::lock_guard<std::mutex> gd(mtx_);   //任务队列加锁
        taskqueue_.push(func);  //任务入队
    }
    //唤醒事件循环
    wakeup();
}

void EventLoop::wakeup()
{
    uint64_t val=1;
    write(wakeupfd_, &val, sizeof(val));
}

void EventLoop::handlewakeup()
{
    printf("handlewakeup() thread is %ld\n", syscall(SYS_gettid));
    uint64_t val;
    read(wakeupfd_, &val, sizeof(val)); //从eventfd中读取数据，如果不读取，eventfd的读事件会一直触发

    std::function<void()> func;

    //执行队列里的全部任务
    std::lock_guard<std::mutex> gd(mtx_);
    while(taskqueue_.size()>0)
    {
        func=std::move(taskqueue_.front()); //出列一个任务
        taskqueue_.pop();
        func(); //执行任务
    }
}

void EventLoop::handletimer()
{
    struct itimerspec timeout;  //定时时间的数据结构。
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec=timetvl_;  //定时时间
    timeout.it_value.tv_nsec=0;
    timerfd_settime(timerfd_, 0, &timeout, 0);

    if(ismainloop_)
    {
        // printf("主事件循环闹钟响了。\n");
    }
    else
    {
        printf("EventLoop::handletimer() thread is %ld.", syscall(SYS_gettid));
        // printf("从事件循环闹钟响了。\n");
        time_t now=time(0);
        std::vector<int> to_remove;
        for(auto c:conns_)
        {
            printf(" %d", c.first);
            if(c.second->timeout(now, timeout_))
            {
                to_remove.emplace_back(c.first);
            }
        }
        for(int fd:to_remove)
        {
            {
                std::lock_guard<std::mutex> gd(mmtx_);
                conns_.erase(fd);  //在EventLoop中删除超时的connection。
            }
            timercallback_(fd);    //在TcpServer中删除超时的connection。
        }
        printf("\n");
    }
}

void EventLoop::newconnection(spConnection conn)
{
    std::lock_guard<std::mutex> gd(mmtx_);
    conns_[conn->fd()]=conn;
}

void EventLoop::settimercallback(std::function<void(int)> func)
{
    timercallback_=func;
}