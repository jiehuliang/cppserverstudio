#ifndef RTP_H
#define RTP_H
#include <stdint.h>
#include <endian.h>
#include <string>
#include "Buffer.h"


typedef enum {
	RTP_Invalid = -1,
	RTP_TCP = 0,
	RTP_UDP = 1,
	RTP_MULTICAST = 2,
} eRtpType;

/**
*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|V=2|P|X|  CC   |M|     PT      |       sequence number         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           timestamp                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           synchronization source (SSRC) identifier            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            contributing source (CSRC) identifiers             |
|                             ....                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      defined by profile       |           length              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        extension header                        |
|                             ....                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           payload data                         |
|                             ....                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           padding data                         |
|                             ....                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**/

#pragma pack(push, 1)
class RtpHeader {
public:
#if __BYTE_ORDER == __BIG_ENDIAN
	uint32_t version : 2;	/* protocol version */
	uint32_t padding : 1;	/* padding flag */
	uint32_t ext : 1;	/* header extension flag */
	uint32_t csrc : 4;	/* CSRC count */
	uint32_t mark : 1;	/* marker bit */
	uint32_t pt : 7;	/* payload type */

#else
	uint32_t csrc : 4;	/* CSRC count */
	uint32_t ext : 1;	/* header extension flag */
	uint32_t padding : 1;	/* padding flag */
	uint32_t version : 2;	/* protocol version */
	uint32_t pt : 7;	/* payload type */
	uint32_t mark : 1;	/* marker bit */
#endif
	uint32_t seq : 16;	/* sequence number */
	uint32_t timestamp;	/* timestamp */
	uint32_t ssrc;	/* synchronization source */
	// 负载，如果有csrc和ext，前面为 4 * csrc + (4 + 4 * ext_len)  [AUTO-TRANSLATED:fcd87b19]
	// Payload, if csrc and ext exist, the front is 4 * csrc + (4 + 4 * ext_len)
	uint8_t payload;

public:
	size_t getCsrcSize() const;
	uint8_t* getCsrcData();

	size_t getExtSize() const;
	uint16_t getExtReserved() const;
	uint8_t* getExtData();

	uint8_t* getPayloadData();
	ssize_t getPayloadSize(size_t rtp_size) const;

private:
	size_t getPayloadOffset() const;
	size_t getPaddingSize(size_t rtp_size) const;

};
#pragma pack(pop)

class RtpPacket {
public:
	using Ptr = std::shared_ptr<RtpPacket>;
	enum {RtpVersion = 2, RtpHeaderSize = 12, RtpTcpHeaderSize = 4};

	// Get the rtp header
	RtpHeader* getHeader();
	const RtpHeader* getHeader() const;

	//主机字节序的seq
	//Host byte order seq
	uint16_t getSeq() const;
	uint32_t getStamp() const;
	// Host byte order timestamp, converted to milliseconds
	uint64_t getStampMS(bool ntp = true) const;
	// Host byte order ssrc
	uint32_t getSSRC() const;
	// Valid payload, skip csrc, ext
	uint8_t* getPayload();
	// Valid payload length, excluding csrc, ext, padding
	size_t getPayloadSize() const;

	static Ptr CreateRtp();

private:
	std::shared_ptr<Buffer> data_;
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