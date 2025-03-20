#pragma once

#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>

//socket的地址协议类

class InetAddress
{
private:
	sockaddr_in _addr;

public:
	InetAddress();
	InetAddress(const std::string &ip, uint16_t port);	//listenfd用的构造函数
	InetAddress(const sockaddr_in addr);	//客户端连接时用的构造函数
	~InetAddress();

	const char*ip() const;	//返回字符串表示的地址
	uint16_t port() const;	//返回整数表示的端口
	const sockaddr* addr() const;	//返回_addr成员的地址，转换成sockaddr
	void setaddr(sockaddr_in clientaddr); //设置_addr
};
