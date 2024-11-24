#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <vector>

class Socket;
class EventLoop;
class Acceptor;
class Connection;
class ThreadPool;

class Server {
public:
    Server(EventLoop*);
    ~Server();
    
    void handleEvent(int);
    void newConnection(Socket *serv_sock);
    void deleteConnection(Socket* sock);
private:
    EventLoop* mainReactor;
    Acceptor* acceptor;
    std::map<int, Connection*> connections;
    std::vector<EventLoop*> subReactors;
    ThreadPool* thPool;
};


#endif //SERVER_H