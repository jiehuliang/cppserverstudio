#include <cstdio>
#include "common.h"
#include "Logging.h"

int main() {
	for (int i = 0; i < 10000; ++i) {
		LOG_INFO << generateRandomInt<uint32_t>() << "\r\n";
	}
}