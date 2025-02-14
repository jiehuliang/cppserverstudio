#ifndef H264_RTP_H
#define H264_RTP_H
#include <cstdint>
#include <cstddef>
#include "Rtsp.h"

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
	int inputFrame(Nalu::Ptr nalu);



private:
	void packRtp();
	void packRtpFu();
	void packRtpStapA();

private:
	Nalu::Ptr sps_;
	Nalu::Ptr pps_;
	Nalu::Ptr last_nale_;
};


#endif // H264_RTP_H