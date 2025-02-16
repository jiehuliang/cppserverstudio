#include "H264Rtp.h"



int H264RtpDecoder::inputRtp(const RtpPacket::Ptr& rtp) {
	auto seq = rtp->getSeq();
	auto ret = decodeRtp(rtp);
}

int H264RtpDecoder::decodeRtp(const RtpPacket::Ptr& rtp) {
	
}


void H264RtpEncoder::insertConfigFrame(uint32_t pts) {
    if (!sps_ || !pps_) {
        return;
    }
    //gop缓存从sps开始，sps、pps后面还有时间戳相同的关键帧，所以mark bit为false
    packRtp(sps_->buffer.c_str(), sps_->len, pts, false, true);
    packRtp(pps_->buffer.c_str(), sps_->len, pts, false, false);
}

int H264RtpEncoder::inputFrame(const H264Nalu::Ptr& nalu) {
    switch (nalu->nal_unit_type) {
    case H264Nalu::H264NaluType::NAL_SPS: {
        sps_ = std::move(nalu);
        return true;
    }
    case H264Nalu::H264NaluType::NAL_PPS: {
        pps_ = std::move(nalu);
        return true;
    }
    default: break;
    }
    if (last_nalu_) {
        //如果时间戳发生了变化，那么markbit才置true
        inputFrame_l(last_nalu_, last_nalu_->_pts != nalu->_pts);
    }
    last_nalu_ = std::move(nalu);
}

bool H264RtpEncoder::inputFrame_l(const H264Nalu::Ptr& nalu, bool is_mark){
    if (nalu->keyFrame()) {
        //保证每一个关键帧前都有SPS与PPS
        insertConfigFrame(nalu->_pts);
    }
    packRtp(nalu->buffer.c_str(), nalu->len, nalu->_pts, is_mark, false);
    return true;
}

void H264RtpEncoder::packRtp(const char* data, size_t len, uint32_t pts, bool is_mark, bool gop_pos){

}

void H264RtpEncoder::packRtpFu() {}
void H264RtpEncoder::packRtpStapA() {}