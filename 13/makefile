all: client tcpepoll

client: client.cpp
	g++ -g -o client client.cpp

tcpepoll: tcpepoll.cpp InetAddress.cpp MySocket.cpp MyEpoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp
	g++ -g -o tcpepoll tcpepoll.cpp InetAddress.cpp MySocket.cpp MyEpoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp

clean:
	rm -f client tcpepoll