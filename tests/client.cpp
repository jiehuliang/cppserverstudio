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


#include <string>
#include <cstring>
#include <iostream>

#include "util.h"

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	errif(sockfd == -1,"socket create_error");

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(8888);

	int ret = connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr));
	errif(ret == -1, "sockfd connect error");

	while (true) {
		char buf[1024];
		memset(&buf, 0, sizeof(buf));
		//std::cin >> buf;
		std::cin.getline(buf, sizeof(buf));
		ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
		if (write_bytes == -1) {
			std::cout << "sock is already disconnected,can't write any more!\n";
			break;
		}
		memset(&buf,0,sizeof(buf));
		ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
		if (read_bytes > 0) {
			std::cout<<"message form server: \n" << buf;
		}
		else if (read_bytes == 0) {
			std::cout << "server is already disconnected!\n";
			break;
		}else if (read_bytes == -1) {
			std::cout << "socket read error!\n";
			break;
		}
	}
	close(sockfd);
	return 0;
}