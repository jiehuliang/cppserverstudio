#ifndef SOCKET_H
#define SOCKET_H

class InetAddress;

class Socket {
public:
  Socket();
  Socket(int sockfd);
  ~Socket();

  void bind(InetAddress *address);
  void listen();
  void setnonblocking();

  int accept(InetAddress *address);

  int getfd();

private:
  int sockfd;
  InetAddress *local_address;
};

#endif // SOCKET_H
