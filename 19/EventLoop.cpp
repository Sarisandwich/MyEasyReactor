#include"EventLoop.h"
#include<iostream>
#include<errno.h>

EventLoop::EventLoop():ep_(new MyEpoll())
{}

EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::run()
{
    while(true)
    {
        std::vector<Channel*> channels=ep_->loop();

        for(auto& ch:channels)
        {
            ch->handleevent();
        }
    }
}

void EventLoop::updatechannel(Channel* ch)
{
    ep_->updatechannel(ch);
}
