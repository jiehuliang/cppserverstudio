#include "H264Rtp.h"
#include <arpa/inet.h>


int H264RtpDecoder::inputRtp(const RtpPacket::Ptr& rtp) {
	auto seq = rtp->getSeq();
	auto ret = decodeRtp(rtp);
}

int H264RtpDecoder::decodeRtp(const RtpPacket::Ptr& rtp) {
	
}


H264RtpEncoder::H264RtpEncoder(uint32_t ssrc, uint8_t interleaved, uint8_t pt, uint32_t sample_rate, size_t mtu_size) {
    _pt = pt;
    _interleaved = interleaved;
    _ssrc = ssrc;
    _sample_rate = sample_rate;
    _mtu_size = mtu_size;
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
    if (len + 3 <= _mtu_size - RtpPacket::RtpHeaderSize) {
        packRtpStapA();
    } else {
        packRtpFu();
    }
}

void H264RtpEncoder::packRtpStapA() {

}

void H264RtpEncoder::packRtpFu() {

}


RtpPacket::Ptr H264RtpEncoder::makeRtp(int type,const char* data,size_t len,bool mark,uint32_t stamp) {
    uint16_t playload_len = (uint16_t)(len + RtpPacket::RtpHeaderSize);
    RtpPacket::Ptr rtp = RtpPacket::CreateRtp();
    //rtsp over tcp 头
    auto pack = rtp->getData();
    std::string capacity(RtpPacket::RtpTcpHeaderSize + playload_len, 0);
    pack->Append(capacity.c_str(), RtpPacket::RtpTcpHeaderSize + playload_len);
    auto ptr = pack->Peek();
    ptr[0] = '$';
    ptr[1] = _interleaved;
    ptr[2] = playload_len >> 8;
    ptr[3] = playload_len & 0xFF;

    auto header = rtp->getHeader();
    header->version = RtpPacket::RtpVersion;
    header->padding = 0;
    header->ext = 0;
    header->csrc = 0;
    header->mark = mark;
    header->pt = _pt;
    header->seq = htons(_seq);
    ++_seq;
    header->timestamp = htonl(uint64_t(stamp) * _sample_rate / 1000);
    header->ssrc = htonl(_ssrc);

    //有效负载
    if (data) {
        memcpy(&ptr[RtpPacket::RtpHeaderSize + RtpPacket::RtpTcpHeaderSize], data, len);
    }
    return rtp;
}

