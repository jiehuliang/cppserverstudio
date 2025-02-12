#ifndef MEDIA_STREAM_H
#define MEDIA_STREAM_H

#include <string>

class RtspMediaStream {
public:
    RtspMediaStream();
    ~RtspMediaStream();

    // Create a media stream from .264 or .h264 file
    static void createFromEs();

    void h264_sdp_create(const char* file, const char* sps, const int sps_len,
        const char* pps, const int pps_len, int payload_type, int time_base);

private:
    std::string url_;

    std::string stream_;

};

#endif // MEDIA_STREAM_H