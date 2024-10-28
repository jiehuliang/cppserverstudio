#include "Server.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "Connection.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>



#define READ_BUFFER 1024

Server::Server(EventLoop *_loop):loop(_loop) {
    acceptor = new Acceptor(loop);
	std::function<void(Socket*)> cb = std::bind(&Server::newConnection,this,std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server() {
    delete acceptor;
}

void Server::handleEvent(int sockfd) {
}

void Server::newConnection(Socket* sock) {
    Connection* conn = new Connection(loop, sock);
    std::function<void(Socket*)>cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    connections[sock->getfd()]= conn;
}

void Server::deleteConnection(Socket* sock) {
    Connection* conn = connections[sock->getfd()];
    connections.erase(sock->getfd());
    delete conn;
}
