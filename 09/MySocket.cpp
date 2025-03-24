#include"MySocket.h"

/*
class MySocket
{
private:
    const int _fd;
public:
    MySocket(int fd);   //构造函数，传入fd
    ~MySocket();    //析构函数close这个fd

    int fd() const; //返回_fd

    void setreuseaddr(bool on); //设置SO_REUSEADDR
    void setreuseport(bool on); //设置SO_REUSEPORT
    void settcpnodelay(bool on); //设置TCP_NODELAY
    void setkeepalive(bool on); //设置SO_KEEPALIVE
    void bind(const InetAddress& servaddr); //服务端的socket调用此函数
    void listen(int listeningnum=128); //服务端的socket调用此函数
    void accept(InetAddress& clientaddr); //服务端的socket调用此函数
};
*/

MySocket::MySocket(int fd):_fd(fd){}

MySocket::~MySocket()
{
    ::close(_fd);
}

int MySocket::fd() const
{
    return _fd;
}

void MySocket::settcpnodelay(bool on)
{
    int optval=on?1:0;
    ::setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void MySocket::setreuseaddr(bool on)
{
    int optval=on?1:0;
    ::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void MySocket::setreuseport(bool on)
{
    int optval=on?1:0;
    ::setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void MySocket::setkeepalive(bool on)
{
    int optval=on?1:0;
    ::setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

void MySocket::bind(const InetAddress& servaddr)
{
    if(::bind(_fd, servaddr.addr(), sizeof(sockaddr))<0)
    {
        perror("bind() failed."); close(_fd); exit(-1);
    }
}
void MySocket::listen(int listeningnum)
{
    if(::listen(_fd, listeningnum)!=0)
    {
        perror("listen() failed."); close(_fd); exit(-1);
    }
}


int MySocket::accept(InetAddress& clientaddr)
{
    sockaddr_in peeraddr;
    socklen_t len=sizeof(peeraddr);
    int clientfd=accept4(_fd, (sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);

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