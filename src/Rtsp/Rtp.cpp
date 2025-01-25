#include "Rtp.h"
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

#define AV_RB16(x) ((((const uint8_t *)(x))[0] << 8) | ((const uint8_t *)(x))[1])

size_t RtpHeader::getCsrcSize() const {
	//each csrc occupies 4 bytes
	return csrc << 2;
}

uint8_t* RtpHeader::getCsrcData() {
	if (!csrc) {
		return nullptr;
	}
	return &payload;
}

size_t RtpHeader::getExtSize() const {
	//rtp has ext
	if (!ext) {
		return 0;
	}
	auto ext_ptr = &payload + getCsrcSize();
	//uint16_t reserved = AV_RB16(ext_ptr);
	//each ext occupies 4 bytes
	return AV_RB16(ext_ptr + 2) << 2;
}
uint16_t RtpHeader::getExtReserved() const{
	//rtp has ext
	if (!ext) {
		return 0;
	}
	auto ext_ptr = &payload + getCsrcSize();
	return AV_RB16(ext_ptr);
}

uint8_t* RtpHeader::getExtData() {
	if (!ext) {
		return nullptr;
	}
	auto ext_ptr = &payload + getCsrcSize();
	//the extra 4 bytes is reserved , ext_len
	return ext_ptr + 4;
}

size_t RtpHeader::getPayloadOffset() const {
	//when there is ext ,you also need to ignore the reserved, ext_len 4 bytes
	return getCsrcSize() + (ext ? (4 + getExtSize()) : 0);
}

uint8_t* RtpHeader::getPayloadData() {
	return &payload + getPayloadOffset();
}

size_t RtpHeader::getPaddingSize(size_t rtp_size) const {
	if (!padding) {
		return 0;
	}
	auto end = (uint8_t*)this + rtp_size - 1;
	return *end;
}

ssize_t RtpHeader::getPayloadSize(size_t rtp_size) const {
	auto invalid_size = getPayloadOffset() + getPaddingSize(rtp_size);
	return (ssize_t)rtp_size - invalid_size - RtpPacket::RtpHeaderSize;
}


RtpHeader* RtpPacket::getHeader() {
	//need to remove rtp over tcp 4 bytes length
	return (RtpHeader*)(data_->Peek() + RtpPacket::RtpTcpHeaderSize);
}

const RtpHeader* RtpPacket::getHeader() const {
	return (RtpHeader*)(data_->Peek() + RtpPacket::RtpTcpHeaderSize);
}

RtpPacket::Ptr RtpPacket::CreateRtp() {
	auto rtp = Ptr(new RtpPacket);
	rtp->data_ = std::shared_ptr<Buffer>(new Buffer);
	return rtp;
}



/*
* +---------------+----------------+----------------+
*| Forbidden Bit  | NAL Reference  | NAL Unit Type  |
*| (F, 1 bit)     | IDC(NRI,2bits) | (Type, 5 bits) |
*+----------------+----------------+----------------+
*/
int Nalu::get_annexb_nalu(const char* in, size_t size) {

	const char* stream = in;
	int nal_start = 0;
	int nal_end = 0;
	int i = 0;

	while ((stream[0] != 0 || stream[1] != 0 || stream[2] != 0x01)
		&& (stream[0] != 0 || stream[1] != 0 || stream[2] != 0 || stream[3] != 0x01))
	{
		i++;
		if (i + 4 > size) { 
			return 0; 
		}
	}

	if (stream[0] != 0 || stream[1] != 0 || stream[2] != 0x01) {
		i++;
	}

	if (stream[i] != 0 || stream[i + 1] != 0 || stream[i + 2] != 0x01) { return 0; }

	i += 3;
	nal_start = i;

	while ((stream[i] != 0 || stream[i+1] != 0 || stream[i+2] != 0)
		&& (stream[i] != 0 || stream[i+1] != 0 || stream[i+2] != 0x01)) {
		i++;
		if (i + 3 >= size) { 
			nal_end = size;
			len = nal_end - nal_start;
			buf = new char[len];
			std::memcpy(this->buf, stream + nal_start, len);
			forbidden_bit = buf[0] & 0x80;
			nal_reference_idc = buf[0] & 0x60;
			nal_unit_type = buf[0] & 0x1F;
			return nal_end;
		}
	}
	nal_end = i;

	len = nal_end - nal_start;
	buf = new char[len];
	std::memcpy(this->buf, stream + nal_start, len);
	forbidden_bit = buf[0] & 0x80;
	nal_reference_idc = buf[0] & 0x60;
	nal_unit_type = buf[0] & 0x1F;

	return nal_end;
}
