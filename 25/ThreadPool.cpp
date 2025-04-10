#include"ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum):stop_(false)
{
    for(size_t i=0; i<threadnum; ++i)
    {
        threads_.emplace_back([this]
        {
            std::cout<<"create thread("<<syscall(SYS_gettid)<<")."<<std::endl;//显示线程id
            std::cout<<"子线程："<<std::this_thread::get_id()<<std::endl;

            while(stop_==false)
            {
                std::function<void()> task;     //用于存放出队的元素

                {   //锁作用域的开始。/////////////////////////////////
                    std::unique_lock<std::mutex> lock(this->mtx_);

                    //等待生产者的条件变量
                    this->condition_.wait(lock, [this]
                    {
                        return ((this->stop_==true)||(this->taskqueue_.empty()==false));
                    });

                    //在线程池停止之前，如果队列中还有任务，执行完再退出。
                    if((this->stop_==true)&&(this->taskqueue_.empty()==true)) return;

                    //出队一个任务。
                    task=std::move(this->taskqueue_.front());
                    this->taskqueue_.pop();
                }   //锁作用域的结束。/////////////////////////////////

                std::cout<<"thread is "<<syscall(SYS_gettid)<<"."<<std::endl;
                task(); //执行任务。
            }
        });
    }
}

void ThreadPool::addtask(std::function<void()> task)
{
    {   //锁作用域的开始。/////////////////////////////////
        std::lock_guard<std::mutex> lock(mtx_);
        taskqueue_.push(task);
    }   //锁作用域的结束。/////////////////////////////////

    condition_.notify_one();    //唤醒一个线程。
}

ThreadPool::~ThreadPool()
{
    stop_=true;

    condition_.notify_all();    //唤醒所有线程。

    //等待全部线程执行完任务后退出。
    for(std::thread& th:threads_)
    {
        th.join();
    }
}

void show(int num, const std::string& name)
{
    printf("我是%s, 学号是%d。\n", name.c_str(), num);
}

void test()
{
    printf("hello world!\n");
}

int main()
{
    ThreadPool threadpool(3);

    std::string name="小明";
    threadpool.addtask(std::bind(show, 23, name));
    sleep(1);
    threadpool.addtask(std::bind(test));
    sleep(1);
    threadpool.addtask(std::bind([]{printf("HELLOWORLD.\n");}));
    sleep(1);
}