#ifndef RTP_H
#define RTP_H
#include <stdint.h>
#include <endian.h>
#include <string>

#pragma pack(push, 1)
class RtpHeader {
public:
#if __BYTE_ORDER == __BIG_ENDIAN
	uint32_t version : 2;	/* protocol version */
	uint32_t padding : 1;	/* padding flag */
	uint32_t ext : 1;	/* header extension flag */
	uint32_t cc : 4;	/* CSRC count */
	uint32_t mark : 1;	/* marker bit */
	uint32_t pt : 7;	/* payload type */

#else
	uint32_t cc : 4;	/* CSRC count */
	uint32_t ext : 1;	/* header extension flag */
	uint32_t padding : 1;	/* padding flag */
	uint32_t version : 2;	/* protocol version */
	uint32_t pt : 7;	/* payload type */
	uint32_t mark : 1;	/* marker bit */
#endif
	uint32_t seq : 16;	/* sequence number */
	uint32_t timestamp;	/* timestamp */
	uint32_t ssrc;	/* synchronization source */
};

class RtpPacket {
public:
	enum {RtpVersion = 2, RtpHeaderSize = 12, RtpTcpHeaderSize = 4};
	RtpHeader rtpheader;
	uint32_t csrc[16]; //×î¶à16¸öcsrc
	const void* externsion;	//extension(valid only if rtp.ext =1)
	uint16_t extlen;	//extesion length in bytes
	uint16_t reserved;	//extension reserved
	const void* payload; //rtp payload
	int payloadlen;	//payload length in bytes
};

class Nalu {
public:
	int startcodeprefix_len;	//! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
	unsigned len;	//! Length of the NAL unit (include the start code, which does not belong to the NALU)
	unsigned max_size;	//! Nal Unit Buffer size
	int forbidden_bit;	//! should be always FALSE
	int nal_reference_idc;	//! NALU_PRIORITY_xxxx
	int nal_unit_type;	//! NALU_TYPE_xxxx
	char* buf;	//! include start code
	unsigned short lost_packets;	//! true, if packet loss is detected

	int get_annexb_nalu(const char* stream, size_t size);
};

#endif //RTP_H