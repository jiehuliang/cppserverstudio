#include "RtspSession.h"
#include "HttpRequest.h"
#include "TcpConnection.h"

RtspSession::RtspSession() {}

RtspSession::~RtspSession() {}

void RtspSession::onWholeRtspPacket(const TcpConnectionPtr& conn, const RtspRequest& request) {
	_cseq = atoi(request.GetRequestValue("CSeq").c_str());
	if (request.method() == "OPTIONS")
		handleOptions(conn, request);
	else if (request.method() == "DESCRIBE")
		handleDescribe(conn, request);
	else if (request.method() == "SETUP")
		handleSetup(conn, request);
	else if (request.method() == "PLAY")
		handlePlay(conn, request);
	else
		;
}

void RtspSession::handleOptions(const TcpConnectionPtr& conn, const RtspRequest& request) {

}
void RtspSession::handleDescribe(const TcpConnectionPtr& conn, const RtspRequest& request) {

}
void RtspSession::handleSetup(const TcpConnectionPtr& conn, const RtspRequest& request) {

}

void RtspSession::handlePlay(const TcpConnectionPtr& conn, const RtspRequest& request) {

}

void RtspSession::handleTeardown(const TcpConnectionPtr& conn, const RtspRequest& request) {

}

void RtspSession::handlePause(const TcpConnectionPtr& conn, const RtspRequest& request) {

}