#include"ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum, const std::string& threadtype):stop_(false), threadtype_(threadtype)
{
    for(size_t i=0; i<threadnum; ++i)
    {
        threads_.emplace_back([this]
        {
            printf("create %s thread(%ld).\n", threadtype_.c_str(), syscall(SYS_gettid));  //显示线程id

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

                printf("%s(%ld) execute task.\n", threadtype_.c_str(), syscall(SYS_gettid));
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

/*
#include<memory>
class AA
{
public:
    void show()
    {
        printf("helloworld\n");
    }
    ~AA()
    {
        printf("调用了析构函数\n");
    }
};

void func(std::shared_ptr<AA> aa)
{
    sleep(3);
    aa->show();
}

int main()
{
    ThreadPool tp(2, "TEST");
    {
        std::shared_ptr<AA> aa(new AA);
        tp.addtask(std::bind(func, aa));
    }

    sleep(5);
    return 0;
}
*/