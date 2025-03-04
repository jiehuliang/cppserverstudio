#include "Rtcp.h"
#include "Logging.h"
#include <arpa/inet.h>

void RtcpHeader::setSize(size_t size) {
    length = htons((uint16_t)((size >> 2) - 1));
}

size_t RtcpHeader::getSize(){
    //加上rtcp头长度
    return (1 + ntohs(length)) << 2;
}


static size_t alignSize(size_t bytes) {
    return (size_t) ((bytes + 3) >> 2) << 2;
}

static void setupHeader(RtcpHeader *rtcp, RTCP_TYPE type, size_t report_count, size_t total_bytes){
    rtcp->version = 2;
    rtcp->padding = 0;
    if (report_count > 0x1F){
        LOG_INFO << "rtcp report_count max 31 ,now is: " << report_count;
    }
    //items total counts
    rtcp->report_count = report_count;
    rtcp->pt = (uint8_t) type;
    rtcp->setSize(total_bytes);
}

static void setupPadding(RtcpHeader* rtcp, size_t padding_size) {
    if (padding_size) {
        rtcp->padding = 1;
        ((uint8_t*)rtcp)[rtcp->getSize() - 1] = padding_size & 0xFF;
    }
    else {
        rtcp->padding = 0;
    }
}

std::shared_ptr<RtcpSR> RtcpSR::create(size_t item_count){
    auto real_size = sizeof(RtcpSR) - sizeof(ReportItem) + item_count * sizeof(ReportItem);
    auto bytes = alignSize(real_size);
    auto ptr = (RtcpSR *)new char[bytes];
    setupHeader(ptr, RTCP_TYPE::RTCP_SR, item_count, bytes);
    setupPadding(ptr, bytes - real_size);
    return std::shared_ptr<RtcpSR>(ptr, [](RtcpSR* ptr) {
        delete[](char*) ptr;
        });
}

void RtcpSR::setNtpStamp(struct timeval tv) {
    ntpmsw = htonl(tv.tv_sec + 0x83AA7E80); /* 0x83AA7E80 is the number of seconds from 1900 to 1970 */
    ntplsw = htonl((uint32_t)((double)tv.tv_usec * (double)(((uint64_t)1) << 32) * 1.0e-6));
}

void RtcpSR::setNtpStamp(uint64_t unix_stamp_ms) {
    
}