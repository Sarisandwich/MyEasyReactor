#include"MyEpoll.h"


MyEpoll::MyEpoll()
{
    if((epollfd_=epoll_create(1))==-1)
    {
        printf("epoll_create() failed(%d).\n", errno);
        exit(-1);
    }
}


MyEpoll::~MyEpoll()
{
    close(epollfd_);
}


void MyEpoll::updatechannel(Channel* ch)     //把channel添加/更新到红黑树上，channel中有fd和需要监视的事件
{
    epoll_event ev;
    ev.data.ptr=ch;
    ev.events=ch->events();

    if(ch->inepoll())
    {
        if(epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev)==-1)
        {
            perror("epoll_ctl() failed.\n"); exit(-1);
        }
    }
    else
    {
        if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev)==-1)
        {
            perror("epoll_ctl() failed.\n"); exit(-1);
        }
        ch->setinepoll();
    }
}


std::vector<Channel*> MyEpoll::loop(int timeout)
{
    std::vector<Channel*> channels;

    memset(events_, 0, sizeof(events_));
    int infds=epoll_wait(epollfd_, events_, MaxEvents, timeout);

    if(infds<0)
    {
        perror("epoll_wait() failed.\n"); exit(-1);
    }
    if(infds==0)
    {
        perror("epoll_wait() timeout.\n"); return channels;
    }

    for(int i=0; i<infds; ++i)
    {
        Channel* ch=(Channel*)events_[i].data.ptr;  //取出已发生事件的channel
        ch->setrevents(events_[i].events);  //设置channel的revents的成员
        channels.push_back(ch);
    }
    return channels;
}