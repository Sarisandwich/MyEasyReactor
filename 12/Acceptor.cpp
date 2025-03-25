#include"Acceptor.h"

Acceptor::Acceptor(EventLoop* loop, const std::string& ip, const uint16_t port):_loop(loop)
{
    _servsock=new MySocket(createnonblocking());
    InetAddress servaddr(ip, port);
    _servsock->setkeepalive(true);
    _servsock->setreuseaddr(true);
    _servsock->setreuseport(true);
    _servsock->settcpnodelay(true);
    _servsock->bind(servaddr);
    _servsock->listen();
    
    //ep.addfd(servsock.fd(), EPOLLIN);   //epoll监视listenfd的读事件，水平触发
    _acceptchannel=new Channel(_loop, _servsock->fd());
    _acceptchannel->setreadcallback(std::bind(&Channel::newconnection, _acceptchannel, _servsock));
    _acceptchannel->enablereading();
}

Acceptor::~Acceptor()
{
    delete _servsock;
    // _servsock=nullptr;
    delete _acceptchannel;
    // _acceptchannel=nullptr;
}