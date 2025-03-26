#include"TcpServer.h"

TcpServer::TcpServer(const std::string& ip, const uint16_t port)
{
    _acceptor=new Acceptor(&_loop, ip, port);
    _acceptor->setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete _acceptor;
    for(auto& p:_conns)
    {
        delete p.second;
    }
}

void TcpServer::start()
{
    _loop.run();
}

//处理客户端连接
void TcpServer::newconnection(MySocket* clientsock)
{
    Connection* conn=new Connection(&_loop, clientsock);
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    printf("new connection(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
    _conns[conn->fd()]=conn;
}

void TcpServer::closeconnection(Connection* conn)
{
    printf("client(fd=%d) disconnected.\n", conn->fd());
    // close(fd());
    _conns.erase(conn->fd());
    delete conn;
}

void TcpServer::errorconnection(Connection* conn)
{
    printf("client(eventfd=%d) error.\n", conn->fd());
    // close(fd());
    _conns.erase(conn->fd());
    delete conn;
}