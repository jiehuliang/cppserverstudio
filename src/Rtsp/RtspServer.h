#ifndef RtspServer_H
#define RtspServer_H

#include "common.h"
#include <memory>

class EventLoop;
class TcpServer;
class TcpConnection;

// 自动关闭的时间，以秒为单位
#define AUTOCLOSETIMEOUT 100

class RtspServer{
public:
	RtspServer(EventLoop* loop,const char* ip,const int port,bool auto_close_conn);
	~RtspServer();

	void OnConnection(const std::shared_ptr<TcpConnection>& conn);

	void OnMessage(const std::shared_ptr<TcpConnection>& conn);

	void SetThreadNums(int num);

	void start();

private:
	EventLoop* loop_;
	std::unique_ptr<TcpServer> server_;

	// 是否自动关闭连接。
	bool auto_close_conn_;
};

#endif // RtspServer_H