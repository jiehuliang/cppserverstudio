
#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")
#else
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif // defined(_WIN32)

#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>

#include "util.h"

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	errif(sockfd == -1,"sockfd create error");

	struct sockaddr_in serv_addr;
	memset(&serv_addr,0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(8888);

	errif(bind(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr)) == -1,"sock bind error");

	errif(listen(sockfd, SOMAXCONN) == -1,"sock listen error");

	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);
	memset(&clnt_addr,0,sizeof(clnt_addr));

	int clnt_sockfd = accept(sockfd,(sockaddr*)&clnt_addr,&clnt_addr_size);
	errif(clnt_sockfd == -1,"sock accept error");

	printf("new client fd %d! IP: %s Port: %d\r\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
	while (true) {
		char buf[1024];
		memset(buf,0,sizeof(buf));
		ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf));
		if (read_bytes > 0) {
			printf("message from client fd %zd: %s\r\n", read_bytes, buf);
			write(clnt_sockfd, buf, read_bytes);
		}
		else if (read_bytes == 0) {
			std::cout << "clkient fd "<< clnt_sockfd <<" disconnected\n";
			close(clnt_sockfd);
		}
		else if (read_bytes == -1) {
			close(clnt_sockfd);
			errif(true,"socket read error");
		}
	}
	close(sockfd);
	return 0;
}