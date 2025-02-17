#ifndef H264_RTP_H
#define H264_RTP_H
#include <cstdint>
#include <cstddef>
#include "Rtsp.h"
#include "H264.h"

class H264RtpDecoder {
public:
	int inputRtp(const RtpPacket::Ptr& rtp);

private:
	int decodeRtp(const RtpPacket::Ptr& rtp);

	bool singleFrame();
	bool unpackStapA();
	bool unpackStapB();
	bool unpackMtap16();
	bool unpackMtap24();
	bool unpackFuA();
	bool unpackFuB();

	void outputFrame();
};


class H264RtpEncoder {
public:
	H264RtpEncoder(uint32_t ssrc, uint8_t interleaved, uint8_t pt = 96, uint32_t sample_rate = 90000, size_t mtu_size = 1400);
	~H264RtpEncoder() {}

	int inputFrame(const H264Nalu::Ptr& nalu);

private:
	void insertConfigFrame(uint32_t pts);
	bool inputFrame_l(const H264Nalu::Ptr& nalu, bool is_mark);
	void packRtp(const char* data, size_t len, uint32_t pts, bool is_mark, bool gop_pos);
	void packRtpFu();
	void packRtpStapA();
	RtpPacket::Ptr makeRtp(int type, const char* data, size_t len, bool mark, uint32_t stamp);

private:
	H264Nalu::Ptr sps_;
	H264Nalu::Ptr pps_;
	H264Nalu::Ptr last_nalu_;

private:
	uint8_t _pt;
	uint8_t _interleaved;
	uint16_t _seq = 0;
	uint32_t _ssrc;
	uint32_t _sample_rate;
	size_t _mtu_size;
};


#endif // H264_RTP_H