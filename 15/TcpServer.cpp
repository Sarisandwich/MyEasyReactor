#include"TcpServer.h"

TcpServer::TcpServer(const std::string& ip, const uint16_t port)
{
    _acceptor=new Acceptor(&_loop, ip, port);
    _acceptor->setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete _acceptor;
}

void TcpServer::start()
{
    _loop.run();
}

//处理客户端连接
void TcpServer::newconnection(MySocket* clientsock)
{
    Connection* conn=new Connection(&_loop, clientsock);
}