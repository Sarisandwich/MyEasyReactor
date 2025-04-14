#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<queue>
#include<sys/syscall.h>
#include<mutex>
#include<unistd.h>
#include<thread>
#include<condition_variable>
#include<functional>
#include<future>
#include<atomic>

class ThreadPool
{
private:
    //保存所有工作线程
    std::vector<std::thread> threads_;
    //任务队列，存储需要执行的任务，每个任务是一个可调用对象
    std::queue<std::function<void()>> taskqueue_;
    //互斥锁，保证对任务队列的并发访问是安全的
    std::mutex mtx_;
    //条件变量，用于线程间通信（如有任务就唤醒线程）
    std::condition_variable condition_;
    //原子布尔变量，表示线程池是否正在停止（防止并发问题）
    std::atomic_bool stop_;
public:
    //构造函数启动threadnum个线程
    ThreadPool(size_t threadnum);
    ~ThreadPool();

    //向线程池中添加任务
    void addtask(std::function<void()> task);
};