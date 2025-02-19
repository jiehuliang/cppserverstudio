#include "RtspServer.h"
#include "EventLoop.h"
#include <iostream>

int main(int argc, char* argv[]) {
    int port;
    if (argc <= 1)
    {
        port = 1234;
    }
    else if (argc == 2) {
        port = atoi(argv[1]);
    }
    else {
        printf("error");
        exit(0);
    }
	int size = std::thread::hardware_concurrency() - 1;
	EventLoop* loop = new EventLoop();
	RtspServer* rtspServer = new RtspServer(loop, "10.72.90.27", port, true);
	rtspServer->SetThreadNums(size);
    rtspServer->start();
}