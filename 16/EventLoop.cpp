#include"EventLoop.h"
#include<iostream>
#include<errno.h>

EventLoop::EventLoop():_ep(new MyEpoll())
{}

EventLoop::~EventLoop()
{
    delete _ep;
}

void EventLoop::run()
{
    while(true)
    {
        std::vector<Channel*> channels=_ep->loop();

        for(auto& ch:channels)
        {
            ch->handleevent();
        }
    }
}

void EventLoop::updatechannel(Channel* ch)
{
    _ep->updatechannel(ch);
}
