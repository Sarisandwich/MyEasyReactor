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