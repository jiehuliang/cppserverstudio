#ifndef INETADDRESS_H
#define INETADDRESS_H

#if defined(_WIN32)

#else
#include <arpa/inet.h>
#endif // _WIN32



class InetAddress {
public:
	struct sockaddr_in addr;
	socklen_t addr_len;
	
	InetAddress();
	InetAddress(const char* ip, uint16_t port);
	~InetAddress();
	void setInetAddr(sockaddr_in _addr, socklen_t _addr_len);
	sockaddr_in getAddr();
	socklen_t getAddr_len();
};

#endif //INETADDRESS_H