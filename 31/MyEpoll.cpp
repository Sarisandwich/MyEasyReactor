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

void MyEpoll::removechannel(Channel* ch)
{
    if(ch->inepoll())
    {
        if(epoll_ctl(epollfd_, EPOLL_CTL_DEL, ch->fd(), 0)==-1)
        {
            perror("epoll_ctl() failed.\n"); exit(-1);
        }
    }
}


std::vector<Channel*> MyEpoll::loop(int timeout)
{
    std::vector<Channel*> channels;

    memset(events_, 0, sizeof(events_));
    int infds=epoll_wait(epollfd_, events_, MaxEvents, timeout);

    if(infds<0)
    {
        //EBADF:epfd不是一个有效的描述符
        //EFAULT:参数events指向的内存区域不可写
        //EINVAL:epfd不是一个epoll文件描述符，或者参数maxevents小于等于0
        //EINTR:阻塞过程中被信号中断，epoll_pwait()可以避免，或者错误处理中，解析error后重新调用epoll_wait()
        //在Reactor模型中，不建议使用信号，因为信号处理很麻烦   ————陈硕
        perror("epoll_wait() failed.\n"); exit(-1);
    }
    if(infds==0)
    {
        //如果超时，表示系统很空闲，返回的channels为空
        //perror("epoll_wait() timeout.\n"); 
        return channels;
    }

    for(int i=0; i<infds; ++i)
    {
        Channel* ch=(Channel*)events_[i].data.ptr;  //取出已发生事件的channel
        ch->setrevents(events_[i].events);  //设置channel的revents的成员
        channels.push_back(ch);
    }
    return channels;
}