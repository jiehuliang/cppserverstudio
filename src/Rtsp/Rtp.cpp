#include "Rtp.h"
#include <cstring>
#include <iostream>


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
