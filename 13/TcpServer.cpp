#include"TcpServer.h"

TcpServer::TcpServer(const std::string& ip, const uint16_t port)
{
    _acceptor=new Acceptor(&_loop, ip, port);
}

TcpServer::~TcpServer()
{
    delete _acceptor;
}

void TcpServer::start()
{
    _loop.run();
}