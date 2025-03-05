#include "RtcpContext.h"
#include "TimeStamp.h"
#include <arpa/inet.h>

std::shared_ptr<Buffer> RtcpContextForSend::createRtcpSR(uint32_t rtcp_ssrc) {
	auto rtcp = RtcpSR::create(0);
	rtcp->setNtpStamp(_last_ntp_stamp_ms);
	rtcp->rtpts = htonl(_last_rtp_stamp);
	rtcp->ssrc = htonl(rtcp_ssrc);
	rtcp->packet_count = htonl((uint32_t)_packets);
	rtcp->octet_count = htonl((uint32_t) _bytes);

	//记录上次发送的sender report信息，用于后续统计rtt
	auto last_sr_lsr = ((ntohl(rtcp->ntpmsw) & 0xFFFF << 16) | ((ntohl(rtcp->ntplsw) >> 16) & 0xFFFF));
	_sender_report_ntp[last_sr_lsr] = TimeStamp::Now().microseconds();

}