#include"Connection.h"

Connection::Connection(EventLoop* loop, MySocket* clientsock):_loop(loop), _clientsock(clientsock)
{
    _clientchannel=new Channel(_loop, _clientsock->fd());
    _clientchannel->setreadcallback(std::bind(&Channel::onmessage, _clientchannel));
    _clientchannel->setclosecallback(std::bind(&Connection::closecallback, this));
    _clientchannel->seterrorcallback(std::bind(&Connection::errorcallback, this));
    _clientchannel->enablereading();
    _clientchannel->useet();
}

Connection::~Connection()
{
    delete _clientsock;
    delete _clientchannel;
}

int Connection::fd() const
{
    return _clientsock->fd();
}

std::string Connection::ip() const
{
    return _clientsock->ip();
}

uint16_t Connection::port() const
{
    return _clientsock->port();
}

void Connection::closecallback()
{
    _closecallback(this);
}

void Connection::errorcallback()
{
    _errorcallback(this);
}

void Connection::setclosecallback(std::function<void(Connection*)> func)
{
    _closecallback=func;
}

void Connection::seterrorcallback(std::function<void(Connection*)> func)
{
    _errorcallback=func;
}