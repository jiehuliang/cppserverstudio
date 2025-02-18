#include "RtspSession.h"
#include "HttpRequest.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include"common.h"
#include "Logging.h"

#include <cinttypes>
#include <iomanip>
#include <atomic>
#include <memory>


RtspSession::RtspSession() {
}

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

	Track::Ptr& trackRef = _stream->getMediaTrack();
	trackRef->_seq = 1;
	trackRef->_ssrc = generateRandomInt<uint32_t>();
	trackRef->_time_stamp = 0;
	trackRef->_type = 0;
	trackRef->_samplerate = 90000;
	trackRef->_pt = 96;
	trackRef->_interleaved = 2 * trackRef->_type;

	auto ready = _stream->createFromEs(trackRef->_pt, trackRef->_samplerate);
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
	Track::Ptr& trackRef = _stream->getMediaTrack();
	//trackRef->_inited = true; //现在初始化

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
	case eRtpType::RTP_TCP: {
		conn->Send(getRtspResponse("200 OK",
			{ "Transport",(std::string("RTP/AVP/TCP;unicast;") + "interleaved=") +
							std::to_string((int)trackRef->_interleaved) + "-" +
							std::to_string((int)trackRef->_interleaved + 1) + ";" +
							"ssrc=" + trackRef->getSSRC(),
				"x-Transport-Options","late-tolerance=1.400000",
				"x-Dynamic-Rate","1"
			}));
	}
		break;
	case eRtpType::RTP_UDP: {}
		break;
	case eRtpType::RTP_MULTICAST: {}
		break;
	}

}

void RtspSession::handlePlay(const TcpConnectionPtr& conn, const RtspRequest& request) {
	if(_stream->getMediaTrack() == nullptr || request.GetRequestValue("Session") != _sessionid) {
		conn->Send(getRtspResponse("454 Session Not Found", { "Connection","Close" }));
		return;
	}
	if (!_stream) {
		conn->Send(getRtspResponse("404 Stream Not Found", { "Connection","Close" }));
		return;
	}
	auto Scale = request.GetRequestValue("Scale");
	auto Range = request.GetRequestValue("Range");
	std::map<std::string, std::string> resMap;
	if (!Scale.empty()) {
	}
	if (!Range.empty()) {
	}
	Track::Ptr& trackRef = _stream->getMediaTrack();
	std::string rtp_info;
	rtp_info = "url=" + _content_base + ";" +
		"seq=" + std::to_string(trackRef->_seq) + ";" +
		"rtptime=" + std::to_string((int)(trackRef->_time_stamp * (trackRef->_samplerate / 1000))) + ",";
	rtp_info.pop_back();
	resMap.emplace("RTP-Info", rtp_info);
	resMap.emplace("Range", std::string("npt=") + std::to_string(trackRef->_time_stamp / 1000.0));
	conn->Send(getRtspResponse("200 OK", resMap));

	auto play = [conn](RtpPacket::Ptr packet) {
		auto videodata = packet->getData()->RetrieveAllAsString();
		LOG_INFO << "send data:" << videodata;
		conn->Send(videodata);
		};
	auto stream = getStream();
	std::weak_ptr<RtspMediaStream> weak_self = std::dynamic_pointer_cast<RtspMediaStream>(stream);
	_timer = conn->loop()->RunEvery(500, [weak_self, conn]() {
		auto strong_self = weak_self.lock();
		if (!strong_self) {
			//本对象已经销毁
			return;
		}
		strong_self->readFrame();
	},TimeUnit::MILLISECONDS);

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

std::shared_ptr<RtspMediaStream> RtspSession::getStream() {
	return _stream;
}
