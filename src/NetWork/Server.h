#ifndef SERVER_H
#define SERVER_H

#include <map>

class Socket;
class EventLoop;
class Acceptor;
class Connection;

class Server {
public:
    Server(EventLoop*);
    ~Server();
    
    void handleEvent(int);
    void newConnection(Socket *serv_sock);
    void deleteConnection(Socket* sock);
private:
    EventLoop* loop;
    Acceptor* acceptor;
    std::map<int, Connection*> connections;
};


#endif //SERVER_H