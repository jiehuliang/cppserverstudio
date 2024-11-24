#include "Server.h"
#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "ThreadPool.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : mainReactor(_loop) {
  acceptor = new Acceptor(mainReactor);
  std::function<void(Socket *)> cb =
      std::bind(&Server::newConnection, this, std::placeholders::_1);
  acceptor->setNewConnectionCallback(cb);

  int size = std::thread::hardware_concurrency();
  thPool = new ThreadPool(size);
  for (int i = 0; i < size; ++i) {
    subReactors.push_back(new EventLoop());
  }

  for (int i = 0; i < size; ++i) {
    std::function<void()> sub_loop =
        std::bind(&EventLoop::loop, subReactors[i]);
    thPool->add(sub_loop);
  }
}

Server::~Server() {
  delete acceptor;
  delete thPool;
}

void Server::handleEvent(int sockfd) {}

void Server::newConnection(Socket *sock) {
  int random = sock->getfd() % subReactors.size();
  Connection *conn = new Connection(subReactors[random], sock);
  std::function<void(Socket *)> cb =
      std::bind(&Server::deleteConnection, this, std::placeholders::_1);
  conn->setDeleteConnectionCallback(cb);
  connections[sock->getfd()] = conn;
}

void Server::deleteConnection(Socket *sock) {
  Connection *conn = connections[sock->getfd()];
  connections.erase(sock->getfd());
  delete conn;
}
