#ifndef H264_H
#define H264_H

#include <string>
#include <memory>

class H264Nalu {
public:
    enum H264NaluType {
        NAL_IDR = 5,
        NAL_SEI = 6,
        NAL_SPS = 7,
        NAL_PPS = 8,
        NAL_AUD = 9,
        NAL_B_P = 1,
    };

    using Ptr = std::shared_ptr<H264Nalu>;

    int get_annexb_nalu(const char* stream, size_t size);

    bool keyFrame();

    bool configFrame();

    bool dropAble();

    bool decodeAble();

public:
    int startcodeprefix_len;	//! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
    unsigned len;	//! Length of the NAL unit (include the start code, which does not belong to the NALU)
    unsigned max_size;	//! Nal Unit Buffer size
    int forbidden_bit;	//! should be always FALSE
    int nal_reference_idc;	//! NALU_PRIORITY_xxxx
    H264NaluType nal_unit_type;	//! NALU_TYPE_xxxx
    std::string buffer;	//! include start code
    unsigned short lost_packets;	//! true, if packet loss is detected

public:
    uint32_t _dts = 0;
    uint32_t _pts = 0;
};

#endif // H264_H