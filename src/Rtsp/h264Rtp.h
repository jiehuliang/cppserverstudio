#ifndef H264_RTP_H
#define H264_RTP_H
#include <cstdint>
#include <cstddef>
#include "Rtp.h"

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


#endif // H264_RTP_H