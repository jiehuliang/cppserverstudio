#include "RtspServer.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "Logging.h"
#include "Buffer.h"
#include "HttpContext.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <functional>

RtspServer::RtspServer(EventLoop* loop,const char* ip,const int port,bool auto_close_conn):loop_(loop),auto_close_conn_(auto_close_conn) {
	server_ = std::unique_ptr<TcpServer>(new TcpServer(loop,ip,port));
	server_->set_connection_callback(std::bind(&RtspServer::OnConnection, this, std::placeholders::_1));
	server_->set_message_callback(std::bind(&RtspServer::OnMessage,this,std::placeholders::_1));
	LOG_INFO << "RtspServer Listening on [ " << ip << ":" << port << " ]";
}

RtspServer::~RtspServer() {

}

void RtspServer::OnConnection(const std::shared_ptr<TcpConnection>& conn) {
	int clnt_fd = conn->fd();
	struct sockaddr_in peeraddr;
	socklen_t peer_addrlength = sizeof(peeraddr);
	getpeername(clnt_fd,(struct sockaddr *)&peeraddr,&peer_addrlength);

	LOG_INFO << "RtspServer::OnNewConnection : Add connection "
		<< "[ fd#" << clnt_fd << "-id#" << conn->id() << " ]"
		<< " from " << inet_ntoa(peeraddr.sin_addr) << ":" << ntohs(peeraddr.sin_port);
}

void RtspServer::OnMessage(const std::shared_ptr<TcpConnection>& conn) {
	auto str = conn->read_buf()->RetrieveAllAsString();
	HttpContext* context = conn->context();
	if (!context->ParaseRequest(str))
	{
		LOG_INFO << "RtspServer::onMessage : Receive non Rtsp message";
		conn->Send("Rtsp/1.0 400 Bad Request\r\n\r\n");
		conn->HandleClose();
	}

	if (context->GetCompleteRequest())
	{
		conn->session()->onWholeRtspPacket(conn,*context->request());
		context->ResetContextStatus();
	}
}



void RtspServer::SetThreadNums(int num) {
	server_->SetThreadNums(num);
}

void RtspServer::start(){
	server_->Start();
}