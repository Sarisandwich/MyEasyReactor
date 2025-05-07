#include"EventLoop.h"
#include"thread"
#include<iostream>
#include<errno.h>

EventLoop::EventLoop():ep_(new MyEpoll())
{}

EventLoop::~EventLoop()
{
    // delete ep_;
}

#include<unistd.h>
#include<sys/syscall.h>

void EventLoop::run()
{
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
