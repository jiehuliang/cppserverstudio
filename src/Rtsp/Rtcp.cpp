#include "Rtcp.h"

static size_t alignSize(size_t bytes) {
    return (size_t) ((bytes + 3) >> 2) << 2;
}


std::shared_ptr<RtcpSR> RtcpSR::create(size_t item_count){
    auto real_size = sizeof(RtcpSR) - sizeof(ReportItem) + item_count * sizeof(ReportItem);
    auto bytes=alignSize(real_size);
}