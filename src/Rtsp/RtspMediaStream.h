#ifndef MEDIA_STREAM_H
#define MEDIA_STREAM_H

#include <string>

class RtspMediaStream {
public:
    RtspMediaStream(std::string url);
    ~RtspMediaStream();

    // Create a media stream from .264 or .h264 file
    bool createFromEs(int payload_type, int time_base);

    std::string h264_sdp_create(const char* sps, const int sps_len,
        const char* pps, const int pps_len, int payload_type, int time_base);

    std::string getSdp();

private:
    std::string url_;

    std::string stream_;

    std::string sdp_;

};

#endif // MEDIA_STREAM_H