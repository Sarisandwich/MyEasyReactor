#include"MySocket.h"



MySocket::MySocket(int fd):fd_(fd){}

MySocket::~MySocket()
{
    ::close(fd_);
}

int MySocket::fd() const
{
    return fd_;
}

std::string MySocket::ip() const
{
    return ip_;
}

uint16_t MySocket::port() const
{
    return port_;
}

void MySocket::settcpnodelay(bool on)
{
    int optval=on?1:0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void MySocket::setreuseaddr(bool on)
{
    int optval=on?1:0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void MySocket::setreuseport(bool on)
{
    int optval=on?1:0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void MySocket::setkeepalive(bool on)
{
    int optval=on?1:0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

void MySocket::bind(const InetAddress& servaddr)
{
    if(::bind(fd_, servaddr.addr(), sizeof(sockaddr))<0)
    {
        perror("bind() failed."); close(fd_); exit(-1);
    }
}

void MySocket::listen(int listeningnum)
{
    if(::listen(fd_, listeningnum)!=0)
    {
        perror("listen() failed."); close(fd_); exit(-1);
    }
}

void MySocket::setipport(const std::string& ip, uint16_t port)
{
    ip_=ip;
    port_=port;
}

int MySocket::accept(InetAddress& clientaddr)
{
    sockaddr_in peeraddr;
    socklen_t len=sizeof(peeraddr);
    int clientfd=accept4(fd_, (sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);

    clientaddr.setaddr(peeraddr);

    return clientfd;
}

int createnonblocking()
{
    int listenfd=socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP);
    if(listenfd<0)
    {
        printf("%s: %s: %d listen socket create error: %d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        exit(-1);
    }
    return listenfd;
}