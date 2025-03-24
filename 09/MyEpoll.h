#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<strings.h>
#include<string.h>
#include<sys/epoll.h>
#include<vector>
#include<unistd.h>
#include"Channel.h"

class Channel;

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

    //void addfd(int fd, uint32_t op);    //将fd和它需要监视的事件添加到红黑树上
    void updatechannel(Channel* ch);     //把channel添加/更新到红黑树上，channel中有fd和需要监视的事件
    //std::vector<epoll_event> loop(int timeout=-1);  //运行epoll_wait()，等待事件的发生，已发生的事件用vector容器返回
    std::vector<Channel*> loop(int timeout=-1);  //运行epoll_wait()，等待事件的发生，已发生的事件用vector容器返回
};