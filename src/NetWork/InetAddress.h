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
};

#endif //INETADDRESS_H