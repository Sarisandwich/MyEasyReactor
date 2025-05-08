#pragma once
#include"EventLoop.h"
#include"Acceptor.h"
#include"Connection.h"
#include"ThreadPool.h"
#include<map>
#include<memory>

class TcpServer
{
private:
    std::unique_ptr<EventLoop> mainloop_;                   //主事件循环
    std::vector<std::unique_ptr<EventLoop>> subloop_;       //从事件循环
    int threadnum_;                         //线程池大小，从事件循环数量
    ThreadPool threadpool_;                //线程池
    Acceptor acceptor_; //一个TcpServer只有一个Acceptor对象
    std::map<int, spConnection> conns_;  //map存放管理connection指针

    std::function<void(spConnection)> newconnectioncb_;  //回调EchoServer::HandleNewConnection()
    std::function<void(spConnection)> closeconnectioncb_; //回调EchoServer::HandleClose()
    std::function<void(spConnection)> errorconnectioncb_; //回调EchoServer::HandleError()
    std::function<void(spConnection)> sendcompletecb_; //回调EchoServer::HandleSendComplete()
    std::function<void(spConnection, std::string &message)> onmessagecb_; //回调EchoServer::HandleMessage()
    std::function<void(EventLoop*)>timeoutcb_; //回调EchoServer::HandleTimeOut()

public:
    TcpServer(const std::string& ip, const uint16_t port, int threadnum);
    ~TcpServer();

    void start();

    void newconnection(std::unique_ptr<MySocket> clientsock);           //处理新客户端连接，在Acceptor类中回调此函数
    void closeconnection(spConnection conn);             //关闭客户端连接，在Connection类中回调此函数
    void errorconnection(spConnection conn);             //客户端连接发生错误，在Connection类中回调此函数
    void onmessage(spConnection conn, std::string& message);  //处理报文，在Connection类中回调此函数
    void sendcomplete(spConnection conn);    //发送完成之后的通知，在Connection类中回调此函数
    void epolltimeout(EventLoop* loop);     //epoll_wait()超时，在Eventloop类中回调此函数

    void setnewconnectioncb(std::function<void(spConnection)> func);
    void setcloseconnectioncb(std::function<void(spConnection)> func);
    void seterrorconnectioncb(std::function<void(spConnection)> func);
    void setsendcompletecb(std::function<void(spConnection)> func);
    void setonmessagecb(std::function<void(spConnection, std::string &message)> func);
    void settimeoutcb(std::function<void(EventLoop*)> func);
};