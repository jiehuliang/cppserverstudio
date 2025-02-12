#include "RtspSession.h"
#include "HttpRequest.h"
#include "TcpConnection.h"
#include"common.h"

#include <cinttypes>


RtspSession::RtspSession() {}

RtspSession::~RtspSession() {}

void RtspSession::onWholeRtspPacket(const TcpConnectionPtr& conn, const RtspRequest& request) {
	_cseq = atoi(request.GetRequestValue("CSeq").c_str());
	if (_content_base.empty() && request.method() != "GET") {
		_content_base = request.url();
		_param_strs = request.request_params();
		parse(request.url());
		_schema = "rtsp";
	}
	if (request.method() == "OPTIONS")
		handleOptions(conn, request);
	else if (request.method() == "DESCRIBE")
		handleDescribe(conn, request);
	else if (request.method() == "SETUP")
		handleSetup(conn, request);
	else if (request.method() == "PLAY")
		handlePlay(conn, request);
	else
		conn->Send(getRtspResponse("403 Forbidden"));
}

void RtspSession::handleOptions(const TcpConnectionPtr& conn, const RtspRequest& request) {
	//支持这些命令
	auto resp = getRtspResponse("200 OK", { "Public" , "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, ANNOUNCE, RECORD" });
	conn->Send(resp);
}
void RtspSession::handleDescribe(const TcpConnectionPtr& conn, const RtspRequest& request) {
	//conn->loop()->RunEvery();
	_stream = std::make_shared<RtspMediaStream>(_streamid);
	auto ready = _stream->createFromEs();
	_sessionid = makeRandStr(12);
	if (!ready) {
		conn->Send(getRtspResponse("404 Stream Not Found", { "Connection","Close" }));
		return;
	}
	conn->Send(getRtspResponse("200 OK",
		{ "Content-Base", _content_base + "/",
		 "x-Accept-Retransmit","our-retransmit",
		 "x-Accept-Dynamic-Rate","1"
		}, _stream->getSdp()));

}
void RtspSession::handleSetup(const TcpConnectionPtr& conn, const RtspRequest& request) {
	if (_rtp_type == eRtpType::RTP_Invalid) {
		auto transport = request.GetRequestValue("Transport");
		if (transport.find("TCP") != std::string::npos) {
			_rtp_type = eRtpType::RTP_TCP;
		}
		else if (transport.find("multicast") != std::string::npos) {
			_rtp_type = eRtpType::RTP_MULTICAST;
		}
		else {
			_rtp_type = eRtpType::RTP_UDP;
		}
	}

	switch (_rtp_type) {
	case eRtpType::RTP_TCP:

	case eRtpType::RTP_UDP:
	case eRtpType::RTP_MULTICAST:
	}

}

void RtspSession::handlePlay(const TcpConnectionPtr& conn, const RtspRequest& request) {

}

void RtspSession::handleTeardown(const TcpConnectionPtr& conn, const RtspRequest& request) {

}

void RtspSession::handlePause(const TcpConnectionPtr& conn, const RtspRequest& request) {

}

static std::string dateStr() {
	char buf[64];
	time_t tt = time(NULL);
	strftime(buf, sizeof buf, "%a, %b %d %Y %H:%M:%S GMT", gmtime(&tt));
	return buf;
}

std::string RtspSession::getRtspResponse(const std::string& res_code, const std::initializer_list<std::string>& header,
	const std::string& sdp, const std::string& protocol) {
	std::string key;
	std::map<std::string,std::string> header_map;
	int i = 0;
	for (auto& val : header) {
		if (++i % 2 == 0) {
			header_map.emplace(key, val);
		}
		else {
			key = val;
		}
	}
	return getRtspResponse(res_code, header_map, sdp, protocol);
}

std::string RtspSession::getRtspResponse(const std::string& res_code, const std::map<std::string, std::string>& headers,
	const std::string& sdp, const std::string& protocol) {
	auto header = headers;
	header.emplace("CSeq", std::to_string(_cseq));
	if (!_sessionid.empty()) {
		header.emplace("Session", _sessionid);
	}
	header.emplace("Server", "RtspServer Study");
	header.emplace("Date", dateStr());

	if (!sdp.empty()) {
		header.emplace("Content-Length", std::to_string(sdp.size()));
		header.emplace("Content-Type", "application/sdp");
	}
	std::string message;
	message += protocol + " " + res_code + "\r\n";
	for (auto& pr : header) {
		message +=  pr.first + ": " + pr.second + "\r\n";
	}
	message += "\r\n";
	if (!sdp.empty()) {
		message += sdp;
	}
	return message;
}

void RtspSession::parse(const std::string& url_in) {
	std::string url = url_in;
	auto schema_pos = url.find("://");
	if (schema_pos != std::string::npos) {
		_schema = url.substr(0, schema_pos);
		url = url.substr(schema_pos + 3);
	}

	auto pos = url.find("/");
	auto host_pos = url.substr(0,pos).rfind(":");
	if (host_pos == std::string::npos) {
		_host = url.substr(0, pos);
		_vhost = _host;
		return;
	}
	_host = url.substr(0, host_pos);
	sscanf(url.substr(0, pos).data() + host_pos + 1, "%" SCNu16, &_port);
	_vhost = _host;

	url = url.substr(pos + 1);
	pos = url.find("/");
	if (pos == std::string::npos) {
		_app = url;
		return;
	}
	_app = url.substr(0, pos);

	url = url.substr(pos + 1);
	if (url.back() == '/') {
		url.pop_back();
	}
	_streamid = url;
}
