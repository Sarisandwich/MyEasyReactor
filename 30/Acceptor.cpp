#include"Acceptor.h"

Acceptor::Acceptor(const std::unique_ptr<EventLoop>& loop, const std::string& ip, const uint16_t port)
            :loop_(loop), servsock_(createnonblocking()), acceptchannel_(loop_, servsock_.fd())
{
    //servsock_=new MySocket(createnonblocking());
    InetAddress servaddr(ip, port);
    servsock_.setkeepalive(true);
    servsock_.setreuseaddr(true);
    servsock_.setreuseport(true);
    servsock_.settcpnodelay(true);
    servsock_.bind(servaddr);
    servsock_.listen();
    
    //ep.addfd(servsock.fd(), EPOLLIN);   //epoll监视listenfd的读事件，水平触发
    //acceptchannel_=new Channel(loop_, servsock_.fd());
    acceptchannel_.setreadcallback(std::bind(&Acceptor::newconnection, this));
    acceptchannel_.enablereading();
}

Acceptor::~Acceptor()
{
    // delete servsock_;
    // servsock_=nullptr;
    // delete acceptchannel_;
    // acceptchannel_=nullptr;
}


#include"Connection.h"
void Acceptor::newconnection()
{
    InetAddress clientaddr; //客户端的地址与协议

    std::unique_ptr<MySocket> clientsock(new MySocket(servsock_.accept(clientaddr)));
    clientsock->setipport(clientaddr.ip(), clientaddr.port());
    newconnectioncb_(std::move(clientsock));
}

void Acceptor::setnewconnectioncb(std::function<void(std::unique_ptr<MySocket>)> func)
{
    newconnectioncb_=func;
}