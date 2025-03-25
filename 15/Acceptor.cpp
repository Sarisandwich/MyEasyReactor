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
    _acceptchannel->setreadcallback(std::bind(&Acceptor::newconnection, this));
    _acceptchannel->enablereading();
}

Acceptor::~Acceptor()
{
    delete _servsock;
    // _servsock=nullptr;
    delete _acceptchannel;
    // _acceptchannel=nullptr;
}


#include"Connection.h"
void Acceptor::newconnection()
{
    InetAddress clientaddr; //客户端的地址与协议

    MySocket* clientsock=new MySocket(_servsock->accept(clientaddr));

    printf("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientsock->fd(), clientaddr.ip(), clientaddr.port());

    //为新客户端连接准备读事件，添加到epoll
    // ev.data.fd=clientsock->fd();
    // ev.events=EPOLLIN|EPOLLET;
    // epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock->fd(), &ev);
    //ep.addfd(clientsock->fd(), EPOLLIN|EPOLLET);
    //Connection* conn=new Connection(_loop, clientsock);
    _newconnectioncb(clientsock);
}

void Acceptor::setnewconnectioncb(std::function<void(MySocket*)> func)
{
    _newconnectioncb=func;
}