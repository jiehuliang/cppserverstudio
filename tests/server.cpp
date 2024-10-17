
#include "util.h"
#include "InetAddress.h"
#include "Epoll.h"
#include "Socket.h"


#define MAX_EVENTS 1024
#define READ_BUFFER 1024

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

	int epfd = epoll_create1(0);
	errif(epfd == -1,"epoll create error");
	
	struct epoll_event events[MAX_EVENTS], ev;
	memset(events, 0, sizeof(events));
	memset(&ev, 0, sizeof(ev));

	ev.data.fd = sockfd;
	ev.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

	while (true) {
		int nfds = epoll_wait(epfd,events,MAX_EVENTS,-1);
		errif(nfds == -1, "epoll wait error");
		for (int i = 0; i <= nfds; i++) {
			if (events[i].data.fd == sockfd) {
				struct sockaddr_in clnt_addr;
				socklen_t clnt_addr_size = sizeof(clnt_addr);
				memset(&clnt_addr, 0, sizeof(clnt_addr));

				int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_size);
				errif(clnt_sockfd == -1, "sock accept error");

				printf("new client fd %d! IP: %s Port: %d\r\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

				struct epoll_event clnev;
				memset(&clnev, 0, sizeof(clnev));
				clnev.data.fd = clnt_sockfd;
				clnev.events = EPOLLIN | EPOLLET;
				setnonblocking(clnt_sockfd);
				epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sockfd,&clnev);
			}
			else if (events[i].events == EPOLLIN) {
				char buf[READ_BUFFER];
				while (true) {
					memset(buf, 0, sizeof(buf));
					ssize_t read_bytes = read(events[i].data.fd, buf, sizeof(buf));
					if (read_bytes > 0) {
						printf("message from client fd %d: %s\n", sockfd, buf);
						write(sockfd, buf, sizeof(buf));
					}
					else if (read_bytes == -1 && errno == EINTR) {
						printf("continue reading");
						continue;
					}
					else if (read_bytes == -1 && ((errno == EAGAIN)|| (errno == EWOULDBLOCK))){
						printf("finish reading once, errno: %d\n", errno);
						break;
					}
					else if (read_bytes == 0) {
						printf("EOF, client fd %d disconnected\n", sockfd);
						close(events[i].data.fd);
						break;
					}
				}
			}
		}
	}
	close(sockfd);
	return 0;
}