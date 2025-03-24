#include"MyEpoll.h"

/*
//MyEpoll类
class MyEpoll
{
private:
    static const int MaxEvents=100; //epoll_wait()返回事件数组的大小
    int _epollfd=-1;    //epoll句柄，在构造函数中创建
    epoll_event _events[MaxEvents]; //存放epoll_wait()返回事件的数组，在构造函数中分配内存
public:
    MyEpoll();  //在构造函数中创建_epollfd
    ~MyEpoll(); //在析构函数中关闭_epollfd

    void addfd(int fd, uint32_t op);    //将fd和它需要监视的事件添加到红黑树上
    std::vector<epoll_event> loop(int timeout=-1);  //运行epoll_wait()，等待事件的发生，已发生的事件用vector容器返回
};
*/

MyEpoll::MyEpoll()
{
    if((_epollfd=epoll_create(1))==-1)
    {
        printf("epoll_create() failed(%d).\n", errno);
        exit(-1);
    }
}

MyEpoll::~MyEpoll()
{
    close(_epollfd);
}

/*
void MyEpoll::addfd(int fd, uint32_t op)
{
    epoll_event ev;
    ev.data.fd=fd;
    ev.events=op;

    if(epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev)==-1)
    {
        printf("epoll_ctl() failed(%d).\n", errno);
        exit(-1);
    }
}
*/

void MyEpoll::updatechannel(Channel* ch)     //把channel添加/更新到红黑树上，channel中有fd和需要监视的事件
{
    epoll_event ev;
    ev.data.ptr=ch;
    ev.events=ch->events();

    if(ch->inepoll())
    {
        if(epoll_ctl(_epollfd, EPOLL_CTL_MOD, ch->fd(), &ev)==-1)
        {
            perror("epoll_ctl() failed.\n"); exit(-1);
        }
    }
    else
    {
        if(epoll_ctl(_epollfd, EPOLL_CTL_ADD, ch->fd(), &ev)==-1)
        {
            perror("epoll_ctl() failed.\n"); exit(-1);
        }
        ch->setinepoll();
    }
}

/*
std::vector<epoll_event> MyEpoll::loop(int timeout)
{
    std::vector<epoll_event> evs;

    memset(_events, 0, sizeof(_events));
    int infds=epoll_wait(_epollfd, _events, MaxEvents, timeout);

    if(infds<0)
    {
        perror("epoll_wait() failed."); exit(-1);
    }
    if(infds==0)
    {
        perror("epoll_wait() timeout."); return evs;
    }

    for(int i=0;i<infds; ++i)
    {
        evs.push_back(_events[i]);
    }

    return evs;
}
*/


std::vector<Channel*> MyEpoll::loop(int timeout)
{
    std::vector<Channel*> channels;

    memset(_events, 0, sizeof(_events));
    int infds=epoll_wait(_epollfd, _events, MaxEvents, timeout);

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
        Channel* ch=(Channel*)_events[i].data.ptr;  //取出已发生事件的channel
        ch->setrevents(_events[i].events);  //设置channel的revents的成员
        channels.push_back(ch);
    }
    return channels;
}