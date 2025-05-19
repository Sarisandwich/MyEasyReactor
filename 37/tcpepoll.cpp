#include"EchoServer.h"
#include<signal.h>

EchoServer* echoserver;

void Stop(int sig)
{
    printf("sig=%d\n", sig);
    //调用EchoServer::Stop()停止服务。
    echoserver->Stop();
    printf("echoserver已停止。\n");
    delete echoserver;
    printf("delete echoserver.\n");
    exit(0);
}

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        printf("usage: ./tcpepoll ip port\n"); return -1;
    }

    signal(SIGTERM, Stop);
    signal(SIGINT, Stop);
    
    //TcpServer tcpserver(argv[1], atoi(argv[2]));
    
    //tcpserver.start();

    echoserver=new EchoServer(argv[1], atoi(argv[2]), 30, 0);
    echoserver->Start();
}