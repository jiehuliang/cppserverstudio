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
	int inputFrame(const H264Nalu::Ptr& nalu);

private:
	void insertConfigFrame(uint32_t pts);
	bool inputFrame_l(const H264Nalu::Ptr& nalu, bool is_mark);
	void packRtp(const char* data, size_t len, uint32_t pts, bool is_mark, bool gop_pos);
	void packRtpFu();
	void packRtpStapA();

private:
	H264Nalu::Ptr sps_;
	H264Nalu::Ptr pps_;
	H264Nalu::Ptr last_nalu_;
};


#endif // H264_RTP_H