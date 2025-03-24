#include"EventLoop.h"

EventLoop::EventLoop():_ep(new MyEpoll)
{}

EventLoop::~EventLoop()
{
    delete _ep;
    _ep=nullptr;
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

MyEpoll* EventLoop::ep()
{
    return _ep;
}