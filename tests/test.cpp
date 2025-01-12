#include "File.h"
#include "Rtp.h"
#include "base64.h"	

#include <cstring>

void h264_sdp_create(const char* file, const char* ip, uint16_t port,
	const char* sps, const int sps_len,
	const char* pps, const int pps_len,
	int payload_type,
	int time_base,
	int bitrate) {

	char buff[1024] = { 0 };

	uint32_t profile_level_id = 0;
	if (sps_len >= 4) { // sanity check
		profile_level_id = sps[1] << 16;
		profile_level_id |= sps[2] << 8;
		profile_level_id |= sps[3];    // profile_idc|constraint_setN_flag|level_idc
	}

	char str_sps[100];
	memset(str_sps, 0, 100);
	av_base64_encode(str_sps, 100, (uint8_t*)sps, sps_len);

	char str_pps[100];
	memset(str_pps, 0, 100);
	av_base64_encode(str_pps, 100, (uint8_t*)pps, pps_len);

	/**
	 Single NAI Unit Mode = 0. // Single NAI mode (Only nals from 1-23 are allowed)
	 Non Interleaved Mode = 1£¬// Non-interleaved Mode: 1-23£¬24 (STAP-A)£¬28 (FU-A) are allowed
	 Interleaved Mode = 2,  // 25 (STAP-B)£¬26 (MTAP16)£¬27 (MTAP24)£¬28 (EU-A)£¬and 29 (EU-B) are allowed.
	 Single NAI Unit Mode = 0. // Single NAI mode (Only nals from 1-23 are allowed)
	 Non Interleaved Mode = 1£¬// Non-interleaved Mode: 1-23£¬24 (STAP-A)£¬28 (FU-A) are allowed
	 Interleaved Mode = 2,  // 25 (STAP-B)£¬26 (MTAP16)£¬27 (MTAP24)£¬28 (EU-A)£¬and 29 (EU-B) are allowed.
	 *
	 **/
	int h264_stap_a = 1;

	char demo[] =
		"m=video %d RTP/AVP %d\n"
		"a=rtpmap:%d H264/%d\n"
		"a=fmtp:%d profile-level-id=%06X; packetization-mode=%d; sprop-parameter-sets=%s,%s\n"
		"c=IN IP4 %s";

	snprintf(buff, sizeof(buff), demo, port, payload_type,
		payload_type, time_base,
		payload_type, profile_level_id, h264_stap_a , str_sps, str_pps,
		ip);

	printf("h264 sdp:\n%s\n\n", buff);
	remove(file);
	FILE* fd = fopen(file, "wt");
	if (fd)
	{
		fwrite(buff, strlen(buff), 1, fd);
		fclose(fd);
	}
}

int main() {
	std::string in = File::loadFile("out.h264");
	const char* data = in.c_str();
	size_t size = in.size();

	static const uint8_t start_code[4] = { 0, 0, 0, 1 };

	FILE* fp = fopen("out.264", "wb");

	int got_sps_pps = 0;
	//std::string sps;
	//std::string pps;
	Nalu sps;
	Nalu pps;


	while (size > 0) {
		Nalu nalu;
		int nalu_len = nalu.get_annexb_nalu(data,size);
		size -= nalu_len;
		data += nalu_len;

		if (nalu.nal_unit_type == 7 && got_sps_pps == 0) {
			sps = nalu;
			sps.buf = new char[nalu.len];
			memcpy(sps.buf, nalu.buf, nalu.len);
		}
		if (nalu.nal_unit_type == 8 && got_sps_pps == 0) {
			pps = nalu;
			pps.buf = new char[nalu.len];
			memcpy(pps.buf, nalu.buf, nalu.len);
			h264_sdp_create("h264.sdp", "127.0.0.1", 5004,sps.buf,sps.len, pps.buf, pps.len, 96, 90000, 300000);
			got_sps_pps = 1;
		}


		size_t size_out = 0;
		char buffer[nalu.len +4];
		memset(buffer, 0, nalu.len + 4);  

		memcpy(buffer, start_code, sizeof(start_code));
		size_out += 4;

		memcpy(buffer + size_out, nalu.buf, nalu.len);
		size_out += nalu.len;

		fwrite(buffer, 1, size_out, fp);
	}

	fclose(fp);
	return 0;
}
