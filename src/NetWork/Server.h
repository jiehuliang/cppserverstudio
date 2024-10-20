#ifndef SERVER_H
#define SERVER_H

class Socket;
class EventLoop;

class Server {
public:
    Server(EventLoop*);
    ~Server();
    
    void handleEvent(int);
    void newConnection(Socket *serv_sock);
private:
    EventLoop* loop;
};


#endif //SERVER_H