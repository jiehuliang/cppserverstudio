#include "h264Rtp.h"



int H264RtpDecoder::inputRtp(const RtpPacket::Ptr& rtp) {
	auto seq = rtp->getSeq();
	auto ret = decodeRtp(rtp);
}

int H264RtpDecoder::decodeRtp(const RtpPacket::Ptr& rtp) {
	
}

int H264RtpEncoder::inputFrame(Nalu::Ptr nalu) {

}