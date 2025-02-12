#include "RtspMediaStream.h"
#include "File.h"
#include "Rtsp.h"

RtspMediaStream::RtspMediaStream(std::string url):url_(url) {}

void RtspMediaStream::createFromEs() {
	stream_ = File::loadFile(url_);
	const char* data = stream_.c_str();
	size_t size = stream_.size();
	int got_sps_pps = 0;
	Nalu sps;
	Nalu pps;

	while (size > 0 && got_sps_pps == 0) {
		Nalu nalu;
		int nalu_len = nalu.get_annexb_nalu(data, size);
		size -= nalu_len;
		data += nalu_len;

		if (nalu.nal_unit_type == 7 && got_sps_pps == 0) {
			sps = nalu;
			sps.buf = new char[nalu.len];
			memcpy(sps.buf, nalu.buf, nalu.len);
		}
		if (nalu.nal_unit_type == 8 && got_sps_pps == 0) {
			pps = nalu;
			pps.buf = new char[nalu.len];
			memcpy(pps.buf, nalu.buf, nalu.len);
			h264_sdp_create("h264.sdp", "127.0.0.1", 5004, sps.buf, sps.len, pps.buf, pps.len, 96, 90000, 300000);
			got_sps_pps = 1;
		}
	}
}