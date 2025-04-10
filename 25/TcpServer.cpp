#include"TcpServer.h"

TcpServer::TcpServer(const std::string& ip, const uint16_t port)
{
    acceptor_=new Acceptor(&loop_, ip, port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
    loop_.setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    for(auto& p:conns_)
    {
        delete p.second;
    }
}

void TcpServer::start()
{
    loop_.run();
}

//处理客户端连接
void TcpServer::newconnection(MySocket* clientsock)
{
    Connection* conn=new Connection(&loop_, clientsock);
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));
    //printf("new connection(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
    conns_[conn->fd()]=conn;

    if(newconnectioncb_) newconnectioncb_(conn);
}

void TcpServer::closeconnection(Connection* conn)
{
    if(closeconnectioncb_) closeconnectioncb_(conn);
    //printf("client(fd=%d) disconnected.\n", conn->fd());
    // close(fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorconnection(Connection* conn)
{
    if(errorconnectioncb_)errorconnectioncb_(conn);
    //printf("client(eventfd=%d) error.\n", conn->fd());
    // close(fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::onmessage(Connection* conn, std::string& message)
{
    if(onmessagecb_) onmessagecb_(conn, message);
}

void TcpServer::sendcomplete(Connection* conn)
{
    //printf("send complete.\n");

    if(sendcompletecb_) sendcompletecb_(conn);
}

void TcpServer::epolltimeout(EventLoop* loop)
{
    //printf("epoll_wait() timeout.\n");

    if(timeoutcb_) timeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(std::function<void(Connection*)> func)
{
    newconnectioncb_=func;
}

void TcpServer::setcloseconnectioncb(std::function<void(Connection*)> func)
{
    closeconnectioncb_=func;
}

void TcpServer::seterrorconnectioncb(std::function<void(Connection*)> func)
{
    errorconnectioncb_=func;
}

void TcpServer::setsendcompletecb(std::function<void(Connection*)> func)
{
    sendcompletecb_=func;
}

void TcpServer::setonmessagecb(std::function<void(Connection*, std::string &message)> func)
{
    onmessagecb_=func;
}

void TcpServer::settimeoutcb(std::function<void(EventLoop*)> func)
{
    timeoutcb_=func;
}