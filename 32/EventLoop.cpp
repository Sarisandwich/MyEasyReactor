#include"EventLoop.h"
#include"thread"
#include<iostream>
#include<errno.h>

EventLoop::EventLoop():ep_(new MyEpoll()), wakeupfd_(eventfd(0, EFD_NONBLOCK)), wakechannel_(new Channel(this, wakeupfd_))
{
    wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup, this));
    wakechannel_->enablereading();
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

