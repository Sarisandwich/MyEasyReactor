#include"InetAddress.h"

/*
class InetAddress
{
private:
	sockaddr_in _addr;

public:
	InetAddress(const std::string &ip, uint16_t port);	//listenfd用的构造函数
	InetAddress(const sockaddr_in addr):_addr(addr){}	//客户端连接时用的构造函数
	~InetAddress();

	const char*ip() const;	//返回字符串表示的地址
	uint16_t port() const;	//返回整数表示的端口
	const sockaddr* addr() const;	//返回_addr成员的地址，转换成sockaddr
};
*/

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    _addr.sin_family=AF_INET;
    _addr.sin_addr.s_addr=inet_addr(ip.c_str());
    _addr.sin_port=htons(port);
}

InetAddress::InetAddress(const sockaddr_in addr):_addr(addr){}

InetAddress::~InetAddress(){}

const char*InetAddress::ip() const
{
    return inet_ntoa(_addr.sin_addr);
}

uint16_t InetAddress::port() const
{
    return ntohs(_addr.sin_port);
}

const sockaddr* InetAddress::addr() const
{
    return (sockaddr*)&_addr;
}