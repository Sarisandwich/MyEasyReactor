#include"TcpServer.h"

TcpServer::TcpServer(const std::string& ip, const uint16_t port)
{
    MySocket* servsock=new MySocket(createnonblocking());
    InetAddress servaddr(ip, port);
    servsock->setkeepalive(true);
    servsock->setreuseaddr(true);
    servsock->setreuseport(true);
    servsock->settcpnodelay(true);
    servsock->bind(servaddr);
    servsock->listen();
    
    //ep.addfd(servsock.fd(), EPOLLIN);   //epoll监视listenfd的读事件，水平触发
    Channel* servchannel=new Channel(_loop.ep(), servsock->fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, servsock));
    servchannel->enablereading();
}

TcpServer::~TcpServer()
{
    
}

void TcpServer::start()
{
    _loop.run();
}