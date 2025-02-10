#ifndef RTSP_SESSION_H
#define RTSP_SESSION_H
#include <string>
#include <memory>
class HttpRequest;
class TcpConnection;

class RtspSession {
	using RtspRequest = HttpRequest;
	using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
public:
	RtspSession();
	~RtspSession();
	void onWholeRtspPacket(const TcpConnectionPtr& conn,const RtspRequest& request);
	void handleOptions(const TcpConnectionPtr& conn, const RtspRequest& request);
	void handleDescribe(const TcpConnectionPtr& conn, const RtspRequest& request);
	void handleSetup(const TcpConnectionPtr& conn, const RtspRequest& request);
	void handlePlay(const TcpConnectionPtr& conn, const RtspRequest& request);
	void handleTeardown(const TcpConnectionPtr& conn, const RtspRequest& request);
	void handlePause(const TcpConnectionPtr& conn, const RtspRequest& request);
private:
	//收到的seq，回复时一致
	int _cseq = 0;

	//Session号
	std::string _sessionid;
};

#endif // RTSP_SESSION_H