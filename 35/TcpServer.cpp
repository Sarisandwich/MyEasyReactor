#include"TcpServer.h"

TcpServer::TcpServer(const std::string& ip, const uint16_t port, int threadnum)
                    :threadnum_(threadnum), mainloop_(new EventLoop(true)), acceptor_(mainloop_.get(), ip, port), threadpool_(threadnum_, "IO")
{
    // mainloop_=new EventLoop;    //创建主事件循环
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));   //超时回调函数
    // acceptor_=new Acceptor(mainloop_, ip, port);
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
    
    // threadpool_=new ThreadPool(threadnum_, "IO"); //创建线程池

    //创建从事件循环
    for(int i=0; i<threadnum_; ++i)
    {
        subloop_.emplace_back(new EventLoop(false));   //创建从事件循环，加入subloop_容器
        subloop_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
        subloop_[i]->settimercallback(std::bind(&TcpServer::removeconn, this, std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run, subloop_[i].get()));
    }
}

TcpServer::~TcpServer()
{
    // delete acceptor_;
    //delete mainloop_;
    /*
    for(auto& p:conns_)
    {
        delete p.second;
    }
    */
    // for (auto& loop:subloop_)
    // {
    //     delete loop;
    // }
    // delete threadpool_;
}

void TcpServer::start()
{
    mainloop_->run();
}

void TcpServer::stop()
{
    //停止主事件循环。
    mainloop_->stop();
    printf("主事件循环已停止。\n");
    //停止从事件循环。
    for(int i=0;i<threadnum_; ++i)
    {
        subloop_[i]->stop();
    }
    printf("从事件循环已停止。\n");
    //停止IO线程。
    threadpool_.stop();
    printf("IO线程池已停止。\n");
}

//处理客户端连接
void TcpServer::newconnection(std::unique_ptr<MySocket> clientsock)
{
    //spConnection conn=new Connection(mainloop_, clientsock);
    spConnection conn=std::make_shared<Connection>(subloop_[clientsock->fd()%threadnum_].get(), std::move(clientsock));
    conn->init();
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));
    //printf("new connection(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
    {
        std::lock_guard<std::mutex> gd(mmtx_);
        conns_[conn->fd()]=conn;
    }

    subloop_[conn->fd()%threadnum_]->newconnection(conn);   //把conn存放入EventLoop的map容器中。

    if(newconnectioncb_) newconnectioncb_(conn);
}

void TcpServer::closeconnection(spConnection conn)
{
    if(closeconnectioncb_) closeconnectioncb_(conn);
    //printf("client(fd=%d) disconnected.\n", conn->fd());
    // close(fd());
    {
        std::lock_guard<std::mutex> gd(mmtx_);
        conns_.erase(conn->fd());
    }
    // delete conn;
}

void TcpServer::errorconnection(spConnection conn)
{
    if(errorconnectioncb_)errorconnectioncb_(conn);
    //printf("client(eventfd=%d) error.\n", conn->fd());
    // close(fd());
    {
        std::lock_guard<std::mutex> gd(mmtx_);
        conns_.erase(conn->fd());
    }
    // delete conn;
}

void TcpServer::onmessage(spConnection conn, std::string& message)
{
    if(onmessagecb_) onmessagecb_(conn, message);
}

void TcpServer::sendcomplete(spConnection conn)
{
    //printf("send complete.\n");

    if(sendcompletecb_) sendcompletecb_(conn);
}

void TcpServer::epolltimeout(EventLoop* loop)
{
    //printf("epoll_wait() timeout.\n");

    if(timeoutcb_) timeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(std::function<void(spConnection)> func)
{
    newconnectioncb_=func;
}

void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> func)
{
    closeconnectioncb_=func;
}

void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> func)
{
    errorconnectioncb_=func;
}

void TcpServer::setsendcompletecb(std::function<void(spConnection)> func)
{
    sendcompletecb_=func;
}

void TcpServer::setonmessagecb(std::function<void(spConnection, std::string &message)> func)
{
    onmessagecb_=func;
}

void TcpServer::settimeoutcb(std::function<void(EventLoop*)> func)
{
    timeoutcb_=func;
}

void TcpServer::removeconn(int fd)
{
    {
        std::lock_guard<std::mutex> gd(mmtx_);
        conns_.erase(fd);
    }
}