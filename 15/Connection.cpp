#include"Connection.h"

Connection::Connection(EventLoop* loop, MySocket* clientsock):_loop(loop), _clientsock(clientsock)
{
    _clientchannel=new Channel(_loop, _clientsock->fd());
    _clientchannel->setreadcallback(std::bind(&Channel::onmessage, _clientchannel));
    _clientchannel->enablereading();
    _clientchannel->useet();
}

Connection::~Connection()
{
    delete _clientsock;
    delete _clientchannel;
}