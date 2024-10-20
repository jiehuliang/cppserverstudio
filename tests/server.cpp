#include "EventLoop.h"
#include "Server.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024


int main() {
	EventLoop *loop =new EventLoop();
	Server *server = new Server(loop);
	loop->loop();
	return 0;
}