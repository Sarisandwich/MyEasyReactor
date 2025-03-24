all: client tcpepoll

client: client.cpp
	g++ -g -o client client.cpp

tcpepoll: tcpepoll.cpp InetAddress.cpp MySocket.cpp MyEpoll.cpp Channel.cpp EventLoop.cpp
	g++ -g -o tcpepoll tcpepoll.cpp InetAddress.cpp MySocket.cpp MyEpoll.cpp Channel.cpp EventLoop.cpp